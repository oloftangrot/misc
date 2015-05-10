
all: serlnk_crc_trial stxext_trial posix_async_trial packet_trial

serlnk_crc_trial: serlnk_crc_trial.c serlnk_crc_ccitt.c serlnk_crc_ccitt.h
	gcc -Wall -o $@ serlnk_crc_trial.c serlnk_crc_ccitt.c

stxext_trial: stxetx.h stxetx.c stxetx_trial.c buffer.c
	gcc -Wall -o $@ stxetx.c stxetx_trial.c buffer.c

posix_async_trial: async_port.h posix_async_port.c posix_async_trial.c
	gcc -Wall -o $@ posix_async_port.c posix_async_trial.c

packet_trial: posix_async_trial simple_packet.c packet_trial.c packet_driver.h serlnk_crc_trial
	gcc -Wall -o $@ packet_trial.c simple_packet.c serlnk_crc_ccitt.c posix_async_port.c

clean:
	rm serlnk_crc_trial stxext_trial posix_async_trial packet_trial
