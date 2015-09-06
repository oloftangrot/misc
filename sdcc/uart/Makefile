CC = sdcc
CFLAGS = -mmcs51


uart2_1: uart2_1.c
	$(CC) $<


uart2_2: uart2_2.c
	$(CC)  $<
	
slave: uart_multiproc_slave.c
	$(CC) $(CFLAGS) $<

clean:
	rm *.ihx *.lst *.rst *.rel *.mem *.sym *.lk *.map *.asm