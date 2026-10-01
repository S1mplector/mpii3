.section .rodata
.balign 32
.global mpii3_font
.global mpii3_font_end
mpii3_font:
.incbin "assets/ui.ttf"
mpii3_font_end:
.balign 32
.global mpii3_icon
mpii3_icon:
.incbin "assets/icon.png"
