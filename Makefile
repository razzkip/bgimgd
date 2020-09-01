

install: build
	systemctl stop bgimgd
	cp bgimgd /usr/local/bin/bgimgd
	systemctl start bgimgd

release: config.h bgimgd.c
	gcc -o bgimgd config.h bgimgd.c

build: config.h bgimgd.c
	gcc -o bgimgd config.h bgimgd.c -g -Wall

clean:
	rm -f *.o bgimgd
