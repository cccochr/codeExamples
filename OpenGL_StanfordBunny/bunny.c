//
// A bunny moving in the night with a full moon
//
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>

//depth of field
#define EYEDX 0.09

//motion blur
#define VPASSES 50 // total passes
#define EMOVE 0.001 //distance between 
#define ADDEACH 0.02 // color change
#define FADE 0.98 //color change

struct Model {
	int vertex_count;
	int face_count;

	GLfloat* v_array;
	GLfloat* n_array;
	float* f_array;
	float* fn_array;
};

char* efgets(char *str, int n, FILE *stream) {
	if (fgets(str, n , stream) == NULL) {
		fprintf(stderr, "Error reading from stream.\n");
		exit(1);
	}
	return str;
}

struct Model readModel(char* plyFile){
	struct Model model;

	char* linebuff = calloc(sizeof(char),500);
	FILE* ply = fopen(plyFile, "r");

	efgets(linebuff, 500, ply);
	while (strncmp(linebuff, "end_header", strlen("end_header")) != 0) {
		if (strncmp(linebuff, "element vertex ", strlen("element vertex ")) == 0) {
			sscanf(linebuff + strlen("element vertex "), "%d", &model.vertex_count);
		}
		else if (strncmp(linebuff, "element face ", strlen("element face ")) == 0) {
			sscanf(linebuff + strlen("element face "), "%d", &model.face_count);
		}
		efgets(linebuff, 500, ply);
	}

//	initialze norman and vertex arrays
	model.v_array = calloc(sizeof(GLfloat), model.vertex_count * 3);
	model.n_array = calloc(sizeof(GLfloat), model.vertex_count * 3);

//	read vertexs
	int v_count = 0;
	for (v_count = 0; v_count < model.vertex_count * 3; v_count += 3) {
		efgets(linebuff, 500, ply);
		sscanf(linebuff, "%f %f %f %f %f %f",
		&model.v_array[v_count], &model.v_array[v_count+1], &model.v_array[v_count+2],
		&model.n_array[v_count], &model.n_array[v_count+1], &model.n_array[v_count+2]);
	}

//	read faces
	model.f_array = calloc(sizeof(float), model.face_count * 9);
	model.fn_array = calloc(sizeof(float), model.face_count * 9);
	int f_count = 0, j=0;
	int vc, v1, v2, v3;
	while (fgets(linebuff, 500, ply) != NULL) {
		sscanf(linebuff, "%d %d %d %d", &vc, &v1, &v2, &v3);
		v1 *= 3;
		v2 *= 3;
		v3 *= 3;
		for(j=0;j<3;j++){
			model.f_array[f_count+j] = model.v_array[v1+j];
			model.fn_array[f_count+j] = model.n_array[v1+j];
		}
		f_count += 3;
		for(j=0;j<3;j++){
			model.f_array[f_count+j] = model.v_array[v2+j];
			model.fn_array[f_count+j] = model.n_array[v2+j];
		}
		f_count += 3;
		for(j=0;j<3;j++){
			model.f_array[f_count+j] = model.v_array[v3+j];
			model.fn_array[f_count+j] = model.n_array[v3+j];
		}
		f_count += 3;
	}
	free(linebuff);
	fclose(ply);

	return model;
}

int drawBunny(struct Model model)
{
//	render ply
	glEnableClientState(GL_VERTEX_ARRAY);
 	glEnableClientState(GL_NORMAL_ARRAY);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glVertexPointer(3, GL_FLOAT, 0, model.f_array);
	glNormalPointer(GL_FLOAT, 0, model.fn_array);
	glDrawArrays(GL_TRIANGLES, 0, model.face_count * 3);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	return 0;
}

char *readFile(char *filename)
{
	char *content = NULL;
	int fd, count;
	fd = open(filename,O_RDONLY);
	count = lseek(fd,0,SEEK_END);
	content = (char *)calloc(1,(count+1));
	lseek(fd,0,SEEK_SET); 
	count = read(fd,content,count*sizeof(char));
	content[count] = '\0';
	close(fd);
	return content;
}

void set_light()
{



	float back_position[] = {0.0,1.5,-2.5,1.0};
	float back_direction[] = {0.0,0.0, 0.45};

	float key_position[] = {1.5,1.0,1.5,1.0}; 
	float key_direction[] = {0.0,0.0,-0.45};

	float fill_position[] = {-1.5,1.0,1.5,1.0};
	float fill_direction[] = {0.0,0.0,-0.45};

	float ambient_color[] = {0.0,0.0,0.0,1.0};
	float diffuse_color[] = {1.0,1.0,1.0,1.0};
	float specular_color[] = {1.0,1.0,1.0,1.0};

	// Back light
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient_color); 
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse_color); 
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular_color); 
	glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,0.1); 
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0); 
	glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0); 
	glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.2); 
	glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01); 
	glLightfv(GL_LIGHT0,GL_POSITION,back_position);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,back_direction);


	// Fill light
	glLightfv(GL_LIGHT1,GL_AMBIENT,ambient_color); 
	glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuse_color); 
	glLightfv(GL_LIGHT1,GL_SPECULAR,specular_color); 
	glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,0.1); 
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,180.0); 
	glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,1.0); 
	glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0.2); 
	glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.01); 
	glLightfv(GL_LIGHT1,GL_POSITION,fill_position);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,fill_direction);


	// Key light
	glLightfv(GL_LIGHT2,GL_AMBIENT,ambient_color); 
	glLightfv(GL_LIGHT2,GL_DIFFUSE,diffuse_color); 
	glLightfv(GL_LIGHT2,GL_SPECULAR,specular_color); 
	glLightf(GL_LIGHT2,GL_SPOT_EXPONENT,0.1); 
	glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,180.0); 
	glLightf(GL_LIGHT2,GL_CONSTANT_ATTENUATION,1.0); 
	glLightf(GL_LIGHT2,GL_LINEAR_ATTENUATION,0.2); 
	glLightf(GL_LIGHT2,GL_QUADRATIC_ATTENUATION,0.01); 
	glLightfv(GL_LIGHT2,GL_POSITION,key_position);
	glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION,key_direction);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
}

void vv(float xt)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5+xt*(1.0/5.0),0.5+xt*(1.0/5.0),-0.5,0.5,1.0,15.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(xt,0.0,0.0);
}

void set_moon_material()
{
	float mat_diffuse[] = {0.6,0.6,0.55,1.0};

	float mat_specular[] = {0.4,0.3,0.4,1.0};
	float mat_shininess[] = {10.0};


	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

void set_ground_material()
{
        float mat_diffuse[] = {0.2,0.7,0.2,1.0};
	float mat_specular[] = {1.0,1.0,1.0,1.0};
	float mat_shininess[] = {1.0};

	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

void drawMoon()
{
        set_moon_material();
        glPushMatrix();
	glTranslatef(3.0,3.0,-10.0);
	glRotatef(3.0,1.0,0.0,0.0);
	glutSolidSphere(1,50,50);
        glPopMatrix();
}

void drawGround()
{
        set_ground_material();
        glTranslatef(0.0,-29.75,-27.0);
	glutSolidCube(50);
}


void set_material()
{

	float mat_diffuse[] = {0.45,0.4,0.4,1.0};
	float mat_specular[] = {1.0,1.0,1.0,1.0};
	float mat_shininess[] = {100.0};


	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

void view_volume()
{
	float eye[] = {0.0,0.05,0.0};
	float viewpt[] = {0.0,0.0,-1.0};
	float up[] = {0.0,1.0,0.0};

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1,.1,5.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}

void renderScene(void)
{
	glClearColor(0.0,0.0,0.05,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glTranslatef(0.08,-0.15,-0.45);
	struct Model model = readModel("bunnyN.ply");
	set_material();

	// Motion Blur
	glAccum(GL_ACCUM,ADDEACH);
	int view_pass;
	for(view_pass=0;view_pass<VPASSES;view_pass++){
		glAccum(GL_MULT,FADE); // multiply color by .98
		if(view_pass<VPASSES/4 || (view_pass>VPASSES/2 && view_pass<3*VPASSES/4)) {
			glTranslatef(-0.002,EMOVE,0.001*EMOVE*EMOVE); //move bunny up
		} 
		else {
			glTranslatef(-0.002,-EMOVE,0.001*EMOVE*EMOVE); //move bunny down
		}
		drawBunny(model);
		//glutSwapBuffers(); // uncomment to see movement
		glAccum(GL_ACCUM,ADDEACH);
		}
	drawBunny(model);

	free(model.v_array);
	free(model.n_array);
	free(model.f_array);
	free(model.fn_array);

	glAccum(GL_ACCUM,ADDEACH*25.0);
	glAccum(GL_RETURN,1.0);

	// Depth of Field
	float xt;
	glClear(GL_ACCUM_BUFFER_BIT);
	for(xt=-EYEDX; xt<EYEDX ;xt+=EYEDX/50.0){
		vv(xt);
		drawMoon();
		glAccum(GL_ACCUM,0.1);
	}
	
	// Ground
	drawGround();
	glutSwapBuffers();
}

unsigned int set_shaders()
{
	char *vs, *fs;
	GLuint v, f, p;
	int result = -1;
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	vs = readFile("bunny.vert");
	fs = readFile("bunny.frag");
	glShaderSource(v,1,(const char **)&vs,NULL);
	glShaderSource(f,1,(const char **)&fs,NULL);
	free(vs);
	free(fs);
	glCompileShader(v);
	glCompileShader(f);
	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);
	glLinkProgram(p);
	glUseProgram(p);
	return(p);
}

int main(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(768,768);
	glutCreateWindow("The Swift Bunny in the Night");
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE_ARB);
	view_volume();
	set_light();
	set_shaders();
	glutKeyboardFunc(NULL);
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return 0;
}
