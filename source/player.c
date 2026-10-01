/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "player.h"
#include "spectrum.h"
#include <asndlib.h>
#include <gccore.h>
#include <math.h>
#include <mp3player.h>
#include <ogc/lwp_watchdog.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* libmad owns decoding; only this module accesses the audio APIs or FILE. */
static FILE *stream_file;
static mutex_t lock;
static PlayerSnapshot current;
static bool active, paused, observed_running;
static u64 started;
static s32 read_audio(void *context, void *buffer, s32 size) {
    FILE *file = context;
    size_t n = fread(buffer, 1, (size_t)size, file);
    if (ferror(file)) {
        LWP_MutexLock(lock);
        snprintf(current.error, sizeof(current.error), "Cannot read this file");
        LWP_MutexUnlock(lock);
    }
    return (s32)n;
}
static void analyze(struct mad_stream *stream, struct mad_frame *frame) {
    (void)stream;
    unsigned channels = MAD_NCHANNELS(&frame->header), samples = MAD_NSBSAMPLES(&frame->header);
    float bands[MPII3_BANDS];
    for (unsigned band = 0; band < MPII3_BANDS; band++) {
        float energy = 0;
        for (unsigned ch = 0; ch < channels; ch++)
            for (unsigned s = 0; s < samples; s++) {
                float v = (float)frame->sbsample[ch][s][band] / (float)MAD_F_ONE;
                energy += v * v;
            }
        bands[band] = spectrum_level(energy / (float)(channels * samples));
    }
    LWP_MutexLock(lock);
    memcpy(current.bands, bands, sizeof(bands));
    current.decoded_frames++;
    current.seconds += (float)(32 * samples) / (float)frame->header.samplerate;
    LWP_MutexUnlock(lock);
}
void player_init(void) {
    LWP_MutexInit(&lock, false);
    memset(&current, 0, sizeof(current));
    current.volume = 180;
    ASND_Init();
    MP3Player_Init();
    MP3Player_Volume(current.volume);
}
void player_stop(void) {
    /* Resume output before joining, so a paused decoder can drain its buffers. */
    ASND_Pause(0);
    paused = false;
    if (active) {
        /* PlayFile creates a worker; let it enter before calling the library's Stop. */
        while (!MP3Player_IsPlaying() && !observed_running &&
               ticks_to_millisecs(gettime() - started) < 1000)
            usleep(1000);
        MP3Player_Stop();
        active = false;
    }
    if (stream_file) {
        fclose(stream_file);
        stream_file = NULL;
    }
    LWP_MutexLock(lock);
    current.state = PLAYER_STOPPED;
    memset(current.bands, 0, sizeof(current.bands));
    LWP_MutexUnlock(lock);
}
bool player_play(const char *path) {
    player_stop();
    LWP_MutexLock(lock);
    current.error[0] = 0;
    current.decoded_frames = 0;
    current.seconds = 0;
    LWP_MutexUnlock(lock);
    stream_file = fopen(path, "rb");
    if (!stream_file) {
        current.state = PLAYER_ERROR;
        snprintf(current.error, sizeof(current.error), "Cannot open this file");
        return false;
    }
    started = gettime();
    observed_running = false;
    current.state = PLAYER_STARTING;
    if (MP3Player_PlayFile(stream_file, read_audio, analyze) < 0) {
        fclose(stream_file);
        stream_file = NULL;
        current.state = PLAYER_ERROR;
        snprintf(current.error, sizeof(current.error), "Cannot start decoder");
        return false;
    }
    active = true;
    return true;
}
void player_toggle_pause(void) {
    if (!active || !MP3Player_IsPlaying())
        return;
    paused = !paused;
    ASND_Pause(paused ? 1 : 0);
}
void player_set_volume(int value) {
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    current.volume = (unsigned)value;
    MP3Player_Volume((unsigned)value);
}
void player_snapshot(PlayerSnapshot *out) {
    bool running = MP3Player_IsPlaying();
    if (running)
        observed_running = true;
    LWP_MutexLock(lock);
    if (active) {
        if (running)
            current.state = paused ? PLAYER_PAUSED : PLAYER_PLAYING;
        else if (observed_running || current.decoded_frames ||
                 ticks_to_millisecs(gettime() - started) > 1000) {
            current.state =
                current.decoded_frames && !current.error[0] ? PLAYER_FINISHED : PLAYER_ERROR;
            if (current.state == PLAYER_ERROR && !current.error[0])
                snprintf(current.error, sizeof(current.error), "Not a readable MP3");
            memset(current.bands, 0, sizeof(current.bands));
        }
    }
    *out = current;
    LWP_MutexUnlock(lock);
}
void player_shutdown(void) {
    player_stop();
    ASND_End();
    LWP_MutexDestroy(lock);
}
