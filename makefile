BASE = /home/dnelson/projects/stagehand

all: winstager

debug: stager-debug

winstager:
	i686-w64-mingw32-gcc \
		-Os -s -pipe -fomit-frame-pointer -Wall \
		winstage_compressed.c \
		-lws2_32 -lwinhttp -lcrypt32 \
		-o winstage.exe

stager:
	gcc -Wall stage.c -o stager

stager-debug:
	cppcheck winstage.c
	i686-w64-mingw32-gcc \
		-Wall -g \
		winstage.c \
		-lws2_32 -lwinhttp -lcrypt32 \
		-o winstage-debug.exe

clean:
	rm -f winstage.exe winstage-debug.exe
