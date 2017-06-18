CC := sdcc
CFLAGS := -mstm8 --std-c99 -DSDCC -DSTM8S103 -I../lib-sdcc-stm8s/inc
OBJECT := spi_trial1
LIBSRC := ../lib-sdcc-stm8s/src

all: spi_trial1.ihx

spi_trial1.ihx: spi_trial1.c stm8s_spi.rel stdafx.rel stm8s_gpio.rel stm8s_clk.rel
	$(CC) $(CFLAGS) spi_trial1.c stm8s_spi.rel stm8s_gpio.rel stdafx.rel stm8s_clk.rel

stm8s_spi.rel: $(LIBSRC)/stm8s_spi.c
	$(CC) $(CFLAGS) -c $(LIBSRC)/stm8s_spi.c

stdafx.rel: stdafx.c stdafx.h
	$(CC) $(CFLAGS) -c stdafx.c

stm8s_gpio.rel: $(LIBSRC)/stm8s_gpio.c
	$(CC) $(CFLAGS) -c $(LIBSRC)/stm8s_gpio.c

stm8s_clk.rel:  $(LIBSRC)/stm8s_clk.c
	$(CC) $(CFLAGS) -c $(LIBSRC)/stm8s_clk.c

flash: $(OBJECT).ihx
	stm8flash -c stlinkv2 -p stm8s103f3 -w $(OBJECT).ihx

factory_defaults.bin:
	echo "00 00 ff 00 ff 00 ff 00 ff 00 ff" | xxd -r -p > factory_defaults.bin

factory_default: factory_defaults.bin
	stm8flash -c stlinkv2 -p stm8s103f3 -s opt -w factory_defaults.bin

clean:
	rm -f *.asm *.ihx *.cdb *.lst *.map *.lk *.rel *.rst *.sym *.bin $(OBJECT)
