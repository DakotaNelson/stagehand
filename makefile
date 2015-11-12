BASE = /home/dnelson/projects/stagehand

all: sendreq stager loader

debug: stager-debug

sendreq:
	gcc -Wall sendreq.c -o sendreq

winstager: b64lib
	i686-w64-mingw32-gcc \
		-I$(BASE)/libb64-1.2/include/b64 \
		-L$(BASE)/libb64-1.2/src \
		-Os -s -Wall \
		winstage.c \
		-lb64 -lws2_32 -lwinhttp -lcrypt32 \
		-o winstage.exe

b64lib:
	$(MAKE) -C libb64-1.2

stager:
	gcc -Wall stage.c -o stager

stager-debug:
	cppcheck stage.c
	gcc -Wall -g stage.c -o stager-debug

loader:
	i686-w64-mingw32-gcc -Wall mudge-stager.c -o loader.exe -lws2_32
	# From Raphael Mudge:
	# gcc -L c:\mingw\lib src\main.c -o loader.exe -lws2_32

clean:
	rm -f sendreq stager stager-debug loader.exe winstage.exe
