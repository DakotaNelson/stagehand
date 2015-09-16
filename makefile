sendreq:
	gcc -Wall sendreq.c -o sendreq

loader:
	i686-w64-mingw32-gcc -Wall mudge-stager.c -o loader.exe -lws2_32
	# From Raphael Mudge:
	# gcc -L c:\mingw\lib src\main.c -o loader.exe -lws2_32
