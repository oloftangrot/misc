CC := sdcc
CFLAGS := -mstm8 --std-c99
OBJECT := timer-trial1

all: timer-trial1

%.ihx: %.c
	$(CC) $(CFLAGS) $<

flash: $(OBJECT).ihx
	stm8flash -c stlinkv2 -p stm8s103f3 -w $(OBJECT).ihx

factory_defaults.bin:
	echo "00 00 ff 00 ff 00 ff 00 ff 00 ff" | xxd -r -p > factory_defaults.bin

factory_default: factory_defaults.bin
	stm8flash -c stlinkv2 -p stm8s103f3 -s opt -w factory_defaults.bin

clean:
	rm -f *.asm *.ihx *.cdb *.lst *.map *.lk *.rel *.rst *.sym *.bin $(OBJECT)
