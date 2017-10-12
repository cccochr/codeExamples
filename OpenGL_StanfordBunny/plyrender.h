#ifndef PLYRENDER_H
#define PLYRENDER_H
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

struct Model {
  int vertex_count_g;
  int face_count_g;

  GLfloat* v_array;
	GLfloat* n_array;

  float* f_array;
  float* fn_array;
};
char* efgets(char *str, int n, FILE *stream);
struct Model readModel(char* plyFile);
int drawBunny(struct Model model);
#endif
