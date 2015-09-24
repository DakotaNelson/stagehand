all: sendreq stager loader

debug: stager-debug

sendreq:
	gcc -Wall sendreq.c -o sendreq

stager:
	gcc -Wall stage.c -o stager

stager-debug:
	gcc -Wall -g stage.c -o stager-debug

loader:
	i686-w64-mingw32-gcc -Wall mudge-stager.c -o loader.exe -lws2_32
	# From Raphael Mudge:
	# gcc -L c:\mingw\lib src\main.c -o loader.exe -lws2_32

clean:
	rm -f sendreq stager stager-debug loader.exe
