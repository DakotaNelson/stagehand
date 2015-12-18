BASE = /home/dnelson/projects/stagehand

all: winstager

debug: stager-debug

winstager:
	i686-w64-mingw32-gcc \
		-Os -s -Wall \
		winstage-generated.c \
		-lws2_32 -lwinhttp -lcrypt32 \
		-o winstage.exe
		strip winstage.exe
		upx winstage.exe

stager-debug:
	cppcheck winstage-generated.c
	i686-w64-mingw32-gcc \
		-Wall -g \
		winstage-generated.c \
		-lws2_32 -lwinhttp -lcrypt32 \
		-o winstage-debug.exe

clean:
	rm -f winstage.exe winstage-debug.exe
