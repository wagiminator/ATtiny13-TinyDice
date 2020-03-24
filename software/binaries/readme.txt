avrdude -c usbtiny -p t13 -U flash:w:tinydice.hex
avrdude -c usbtiny -p t13 -U hfuse:w:0xfb:m -U lfuse:w:0x62:m
