//
// This shows anti-aliasing using an accumulation buffer; in general, using
// GLUT_MULTISAMPLE will be preferable for anti-aliasing, but the
// accumulation buffer has many uses, and this is a good introduction.
//

// gcc -L/usr/lib64 -O2 basiccube.c -lX11 -lGL -lGLU -lglut -lm -lXmu -o basiccube
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define VPASSES 10
#define JITTER 0.00

double genrand()
{
return(((double)(random()+1))/2147483649.);
}

struct point {
	float x, y, z;
};

void viewvolume_shape()
{
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0,1.0,0.1,20.0);
}

struct point cross(struct point u, struct point v)
{
struct point w;
w.x = u.y*v.z - u.z*v.y;
w.y = -(u.x*v.z - u.z*v.x);
w.z = u.x*v.y - u.y*v.x;
return(w);
}

struct point unit_length(struct point u)
{
double length;
struct point v;
length = sqrt(u.x*u.x+u.y*u.y+u.z*u.z);
v.x = u.x/length;
v.y = u.y/length;
v.z = u.z/length;
return(v);
}

void jitter_view()
{
	float eye[] = {0.5,0.5,2.0};
	float viewpt[] = {0.5,0.5,-1.0};
	float up[] = {0.0,1.0,0.0};

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1,.1,5.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}



void do_lights()
{
float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
float light0_diffuse[] = { 2.0, 2.0, 2.0, 0.0 };
float light0_specular[] = { 2.25, 2.25, 2.25, 0.0 };
float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};

glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);

glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0);
glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0);
glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.2);
glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01);
glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
}

void do_material(float R, float G, float B)
{
float mat_ambient[] = {0.0,0.0,0.0,1.0};
float mat_diffuse[] = {R,G,B,1.0};
float mat_specular[] = {1.0,1.0,1.0,1.0};
float mat_shininess[] = {2.0};

glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}
void draw_stuff()
{
int i;
//struct point front[4]={{0.0,0.0,1.0},{1.0,0.0,1.0},{1.0,1.0,1.0},{0.0,1.0,1.0}};
struct point back[4]={{0.0,0.0,0.0},{0.0,1.0,0.0},{1.0,1.0,0.0},{1.0,0.0,0.0}};
struct point left[4]={{0.0,0.0,0.0},{0.0,0.0,1.0},{0.0,1.0,1.0},{0.0,1.0,0.0}};
struct point right[4]={{1.0,0.0,0.0},{1.0,1.0,0.0},{1.0,1.0,1.0},{1.0,0.0,1.0}};
struct point top[4]={{0.0,1.0,0.0},{0.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,0.0}};
struct point bottom[4]={{0.0,0.0,0.0},{0.0,0.0,1.0},{1.0,0.0,1.0},{1.0,0.0,0.0}};

glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
glBegin(GL_QUADS);
//glNormal3f(0.0,0.0,1.0);
//for(i=0;i<4;i++) glVertex3f(front[i].x,front[i].y,front[i].z);
glNormal3f(0.0,0.0,1.0);
for(i=0;i<4;i++) glVertex3f(back[i].x,back[i].y,back[i].z);
do_material(1.0, 0.0, 0.0);
glNormal3f(1.0,0.0,0.0);
for(i=0;i<4;i++) glVertex3f(left[i].x,left[i].y,left[i].z);
do_material(0.0, 1.0, 0.0);
glNormal3f(1.0,0.0,0.0);
for(i=0;i<4;i++) glVertex3f(right[i].x,right[i].y,right[i].z);
do_material(0.9, 0.9, 0.9);
glNormal3f(0.0,1.0,0.0);
for(i=0;i<4;i++) glVertex3f(top[i].x,top[i].y,top[i].z);
glNormal3f(0.0,1.0,0.0);
for(i=0;i<4;i++) glVertex3f(bottom[i].x,bottom[i].y,bottom[i].z);
glEnd();
glFlush();
}

void go()
{
int view_pass;
glClear(GL_ACCUM_BUFFER_BIT);
for(view_pass=0;view_pass<VPASSES;view_pass++){
	jitter_view();
	draw_stuff();
	glAccum(GL_ACCUM,1.0/(float)(VPASSES));
	}
glAccum(GL_RETURN,1.0);
// x230 insists on a doubly-buffered visual; without this, only last is shown.
glutSwapBuffers();
}

void getout(unsigned char key, int x, int y)
{
switch(key) {
        case 'q':
                exit(1);
        default:
                break;
    }
}

int main(int argc, char **argv)
{
srandom(123456789);
glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_ACCUM);
glutInitWindowSize(768,768);
glutInitWindowPosition(100,50);
glutCreateWindow("my_cool_cube");
glClearColor(0.35,0.35,0.35,0.0);
glClearAccum(0.0,0.0,0.0,0.0);
glEnable(GL_DEPTH_TEST);
viewvolume_shape();
jitter_view();
do_lights();
do_material(0.9, 0.9, 0.9);
glutDisplayFunc(go);
glutKeyboardFunc(getout);
glutMainLoop();
return 0;
}
