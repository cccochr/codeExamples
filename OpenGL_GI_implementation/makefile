CFLAGS=-I/usr/include -I/usr/X11R6/include -L/usr/lib -L/usr/X11R6/lib -lm -g -O0
GLFLAGS=-lX11 -lGLEW -lGL -lGLU -lglut -lm -lXmu -lXi

all:  teapot

teapot: teapot.c teapot.frag teapot.vert objParser.h reflectance_math.h
	gcc  -O2 teapot.c $(CFLAGS) $(GLFLAGS) -o teapot

debug: teapot.c teapot.frag teapot.vert reflectance_math.c reflectance_math.h
	gcc -g -Wall $(CFLAGS) teapot.c $(GLFLAGS) -o teapot

mathtest: reflectance_math.c reflectance_math.h mathtest.c
	gcc -g -Wall mathtest.c reflectance_math.c $(CFLAGS) -o mathtest

clean:
	rm -f teapot obj mathtest

obj: objParser.c
	gcc -g -Wall objParser.c -o obj
