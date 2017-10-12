// teapot.c
// Cornell Box with Utah Teapot
//
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <unistd.h>

#define VPASSES 10
#define JITTER 0.00
#define DIFFPASSES 200

#include "objParser.h"
#include "reflectance_math.h"

#define WHITE_WALL 0
#define RED_WALL 1
#define GREEN_WALL 2
#define FLOOR 3
#define LIGHT 4

struct material mat_arr[5] = {
  {.am={0,0,0,1},.diff={0.65,0.65,0.7,1.0},.spec={1.0,1.0,1.0,1.0},.shine={2.0}},//WHITE_WALL
  {.am={0,0,0,1},.diff={1.0,0.0,0.0,1.0},.spec={1.0,1.0,1.0,1.0},.shine={10.0}},//RED_WALL
  {.am={0,0,0,1},.diff={0.0,1.0,0.0,1.0},.spec={1.0,1.0,1.0,1.0},.shine={10.0}},//GREEN_WALL
  {.am={0,0,0,1},.diff={0.7,0.7,0.7,1.0},.spec={1.0,1.0,1.0,1.0},.shine={10.0}},//FLOOR
  {.am={0,0,0,1},.diff={1.0,1.0,1.0,1.0},.spec={1.0,1.0,1.0,1.0},.shine={2.0}}//LIGHT
};

struct surface box[6] = {
    //back
    {{{0.0,0.0,0.0},{0.0,5.0,0.0},{5.0,5.0,0.0},{5.0,0.0,0.0}},{0,0,1.0},WHITE_WALL},
    //left
    {{{0.0,0.0,0.0},{0.0,0.0,5.0},{0.0,5.0,5.0},{0.0,5.0,0.0}},{1,0,0},RED_WALL},
    //right
    {{{5.0,0.0,0.0},{5.0,5.0,0.0},{5.0,5.0,5.0},{5.0,0.0,5.0}},{-1,0,0},GREEN_WALL},
    //top
    {{{0.0,5.0,0.0},{0.0,5.0,5.0},{5.0,5.0,5.0},{5.0,5.0,0.0}},{0,-1,0},WHITE_WALL},
    //bottom
    {{{0.0,0.0,0.0},{5.0,0.0,0.0},{5.0,0.0,5.0},{0.0,0.0,5.0}},{0,1,0},FLOOR},
    //light tile
    {{{2.0,4.9,2.0},{2.0,4.9,3.0},{3.0,4.9,3.0},{3.0,4.9,2.0}},{0,-1,0},LIGHT},
};

struct point light_pos = {2.5, 4.92, 2.5};

obj_t* teapot;

unsigned int wallShaderProgram, teapotShaderProgram, lightShaderProgram;

char* efgets(char *str, int n, FILE *stream) {
    if (fgets(str, n , stream) == NULL) {
        fprintf(stderr, "Error reading from stream.\n");
        exit(1);
    }
    return str;
}

void load_texture(char *filename)
{
    FILE *fopen(), *fptr;
    char buf[512], *parse;
    int im_size, im_width, im_height, max_color;
    unsigned char *texture_bytes;

    fptr=fopen(filename,"r");
    efgets(buf,512,fptr);
    do{
        efgets(buf,512,fptr);
    } while(buf[0]=='#');
    parse = strtok(buf," \t");
    im_width = atoi(parse);

    parse = strtok(NULL," \n");
    im_height = atoi(parse);

    efgets(buf,512,fptr);
    parse = strtok(buf," \n");
    max_color = atoi(parse);

    im_size = im_width*im_height;
    texture_bytes = (unsigned char *)calloc(3,im_size);
    if (fread(texture_bytes,3,im_size,fptr) == 0) {
        fprintf(stderr, "Error reading size.\n");
        exit(1);
    }
    fclose(fptr);

    glBindTexture(GL_TEXTURE_2D,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im_width,im_height,0,GL_RGB,
                GL_UNSIGNED_BYTE,texture_bytes);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    cfree(texture_bytes);
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

void set_area_light(struct ray ray)
{
    float light_position[] = {ray.o.x, ray.o.y, ray.o.z, 1.0};
    float light_direction[] = {ray.o.x, 0, ray.o.z, 1.0};

    float ambient_color[] = {0.0,0.0,0.0,1.0};
    float diffuse_color[] = {1.0,1.0,1.0,1.0};
    float specular_color[] = {1.0,1.0,0,1.0};

    glLightfv(GL_LIGHT1,GL_AMBIENT,ambient_color);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuse_color);
    glLightfv(GL_LIGHT1,GL_SPECULAR,specular_color);
    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,30.0);
    glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.3);
    glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0.3);
    glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.3);
    glLightfv(GL_LIGHT1,GL_POSITION,light_position);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,light_direction);

    glEnable(GL_LIGHT1);
}

void set_light_at(struct spotlight_info spot)
{
    float light_position[] = {spot.r.o.x, spot.r.o.y, spot.r.o.z, 1.0};
    float light_direction[] = {spot.r.v.x, spot.r.v.y, spot.r.v.z, 1.0};

    float ambient_color[] = {0.0,0.0,0.0,1.0};
    float specular_color[] = {1.0,1.0,0,1.0};

    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient_color);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,mat_arr[spot.mtl_id].diff);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular_color);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,30.0);
    glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.3);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.3);
    glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.3);
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light_direction);

    float diffuse_color[] = {1.0,1.0,1.0,1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void view_volume()
{
    float eye[] = {2.5,2.5,10.0};
    float viewpt[] = {2.5,2.5,2.5};
    float up[] = {0.0,1.0,0.0};

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,1,1,30.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}

void select_material(unsigned int mat_id){
    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_arr[mat_id].am);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_arr[mat_id].diff);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_arr[mat_id].spec);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_arr[mat_id].shine);
}

void drawSurface(struct surface s){
    select_material(s.mtl_id);
    glBegin(GL_QUADS);
    glNormal3f(s.n.x, s.n.y, s.n.z);
    int i;
    for(i=0;i<4;i++) glVertex3f(s.coords[i].x,s.coords[i].y,s.coords[i].z);
    glEnd();
}

void drawFaceBox()
{
    int i;
    for(i=0; i<5; i++){
        drawSurface(box[i]);
    }
}

void drawLight()
{
    drawSurface(box[5]);
}

void drawTeapot()
{
    glTranslatef(2.5,0.1,2);
    glRotatef(-140.0, 0.0, 1.0, 0.0);

    glMaterialfv(GL_FRONT,GL_AMBIENT,teapot->mtl->Ka);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,teapot->mtl->Kd);
    glMaterialfv(GL_FRONT,GL_SPECULAR,teapot->mtl->Ks);
    glMaterialf(GL_FRONT,GL_SHININESS,teapot->mtl->Ns);

    glEnableClientState(GL_VERTEX_ARRAY);
     glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glVertexPointer(3, GL_FLOAT, 0, teapot->fv);
    glNormalPointer(GL_FLOAT, 0, teapot->fn);
    glTexCoordPointer(2, GL_FLOAT, 0, teapot->ft);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,1);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS, 0, teapot->fCount * 4);
    glDisable(GL_TEXTURE_2D);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void renderScene(void)
{
    glPushMatrix();
    glUseProgram(teapotShaderProgram);
    drawTeapot();
    glPopMatrix();
    glPushMatrix();
    glUseProgram(wallShaderProgram);
    drawFaceBox();
    glPopMatrix();
    glPushMatrix();
    glUseProgram(lightShaderProgram);
    drawLight();
    glPopMatrix();
}

void renderDiffuse(void){
    glClear(GL_ACCUM_BUFFER_BIT);

    int i;
    for(i=0;i<DIFFPASSES;){
            struct point dir;
            double scale;
            if(r01() > 0.8) scale = 1.0;
            else            scale = -1.0;

        struct point light_position = {(float)(rand()%(100)+200)/100, 4.7, (float)(rand()%(100)+200)/100};

        struct ray ray = {light_position, hemisphere_point(r01(), r01(), scale)};
        //struct ray ray = {light_position, hemisphere_point(halton_sequence(rand(),17), halton_sequence(rand(),17), scale)};

        volatile struct spotlight_info spot = {.r={{0,0,0},{0,0,0}}, .hit=false,.dist=10000,.mtl_id=0};
        int j;
        for(j=0;j<5;j++){
            struct spotlight_info temp_spot = find_intersect(box[j], ray);
            if(temp_spot.hit && temp_spot.dist < spot.dist){
                spot = temp_spot;
            }
        }
        if(spot.hit){
            set_light_at(spot);
            set_area_light(ray);
            renderScene();
            glAccum(GL_ACCUM,1.0/(float)DIFFPASSES);
            i++;
        }
    }
    glAccum(GL_RETURN,1.0);
    glutSwapBuffers();
}

unsigned int set_shaders(char* vert, char* frag)
{
    char *vs, *fs;
    GLuint v, f, p;
    int result = -1;
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    vs = readFile(vert);
    fs = readFile(frag);
    glShaderSource(v,1,(const char **)&vs,NULL);
    glShaderSource(f,1,(const char **)&fs,NULL);
    free(vs);
    free(fs);
    glCompileShader(v);
    glGetShaderiv(v,GL_COMPILE_STATUS,&result);
    //fprintf(stderr,"vert: %d\n",result);
    glCompileShader(f);
    glGetShaderiv(f,GL_COMPILE_STATUS,&result);
    //fprintf(stderr,"frag: %d\n",result);
    p = glCreateProgram();
    glAttachShader(p,f);
    glAttachShader(p,v);
    glLinkProgram(p);
    return(p);
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

void setUniform(unsigned int p)
{
    int location;
    location = glGetUniformLocation(p,"mytexture");
    glUniform1i(location,0);
}

int main(int argc, char **argv)
{
    srandom(123456789);
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_MULTISAMPLE|GLUT_ACCUM|GLUT_DOUBLE);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(1280,1024);
    glutCreateWindow("Cornell Box with Utah Teapot");
    glClearColor(0.3,0.3,0.3,0.0);

    teapot = parseObj("teapot.605.obj");
    glMaterialfv(GL_FRONT,GL_AMBIENT,teapot->mtl->Ka);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,teapot->mtl->Kd);
    glMaterialfv(GL_FRONT,GL_SPECULAR,teapot->mtl->Ks);
    glMaterialf(GL_FRONT,GL_SHININESS,teapot->mtl->Ns);
    load_texture(teapot->mtl->map_Kd);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    view_volume();
    glutDisplayFunc(renderDiffuse);
    setUniform(wallShaderProgram = set_shaders("walls.vert","walls.frag"));
    setUniform(teapotShaderProgram = set_shaders("teapot.vert","teapot.frag"));
    setUniform(lightShaderProgram = set_shaders("light.vert","light.frag"));
    glutKeyboardFunc(getout);
    glutMainLoop();
    return 0;
}

