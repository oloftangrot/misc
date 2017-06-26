CC = sdcc
CFLAGS = -mmcs51

all: pipeHeatTimer.hex

pipeHeatTimer.hex: pipeHeatTimer.ihx
	packihx $< > $@

pipeHeatTimer.ihx: pipeHeatTimer.c
	$(CC) $(CFLAGS) $<

clean:
	rm *.ihx *.lst *.rst *.rel *.mem *.sym *.lk *.map *.asm *.hex
