

install: build
	cp bgimgd /usr/local/bin/bgimgd

release: config.h bgimgd.c
	gcc -o bgimgd config.h bgimgd.c

build: config.def.h bgimgd.c
	gcc -o bgimgd config.def.h bgimgd.c -g -Wall

config: config.def.h
	cp config.def.h config.h
	chmod 444 config.h

clean:
	rm -f *.o bgimgd
