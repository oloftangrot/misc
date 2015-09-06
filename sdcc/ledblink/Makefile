CC = sdcc
CFLAGS = -mmcs51

all: ledblink.hex

ledblink.hex: ledblink.ihx
	packihx $< > $@

ledblink.ihx: ledblink.c
	$(CC) $(CFLAGS) $<

clean:
	rm *.ihx *.lst *.rst *.rel *.mem *.sym *.lk *.map *.asm *.hex