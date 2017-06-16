SDCC=sdcc
SDLD=sdld
OBJECTS= blink2.ihx blinky.ihx uart.ihx factory_defaults.bin

.PHONY: all clean flash

all: $(OBJECTS)

clean:
	rm *.lst *.map *.rel *.bin *.sym *.rst *.lk *.cdb *.asm

flash: $(OBJECT).ihx
	stm8flash -c stlinkv2 -p stm8s103f3 -w $(OBJECT).ihx

#	stm8flash -cstlink -pstm8l150 -w $(OBJECT).ihx

%.ihx: %.c
	$(SDCC) -lstm8 -mstm8 --out-fmt-ihx $(CFLAGS) $(LDFLAGS) $<

factory_defaults.bin:
	echo "00 00 ff 00 ff 00 ff 00 ff 00 ff" | xxd -r -p > factory_defaults.bin

factory_default: factory_defaults.bin
	stm8flash -c stlinkv2 -p stm8s103f3 -s opt -w factory_defaults.bin
