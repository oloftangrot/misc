all: main1

main1:
	gcc -Os -fno-pie -D_FORTIFY_SOURCE=0 -fno-stack-protector -o main1 main1.c

dump:
	readelf -a main1 | grep self_destruct	

clean:
	rm main1
