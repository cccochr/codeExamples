CFLAGS=-I/usr/include -I/usr/X11R6/include -L/usr/lib -L/usr/X11R6/lib -O2
GLFLAGS=-lX11 -lGLEW -lGL -lGLU -lglut -lm -lXmu -lXi

all:  bunny

bunny: bunny.c bunny.frag bunny.vert
	gcc $(CFLAGS) -O2 bunny.c $(GLFLAGS) -o bunny

debug: bunny.c bunny.frag bunny.vert
	gcc -g -Wall $(CFLAGS) -O2 bunny.c $(GLFLAGS) -o bunny

verttest: verttest.c
	gcc -g -Wall -I/usr/include -I/usr/X11R6/include -L/usr/lib -L/usr/X11R6/lib -O2 verttest.c -lX11 -lGL -lGLU -lglut -lm -lXmu -lXi -o verttest -g
clean:
	rm -f bunny verttest
