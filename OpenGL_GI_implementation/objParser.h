#ifndef OBJPARSER_H
#define OBJPARSER_H
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mtl {
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ns;
    char map_Kd[20];
    char map_normal[20];
}mtl_t;
typedef struct obj {
    int vCount;
    int fCount;
    int vtCount;
    int* fbuff;
    float* fv;
    float* ft;
    float* fn;
    float* fx;
    float* fy;
    mtl_t*  mtl;
}obj_t;

static inline mtl_t*  parseMtl(char* filename)
{
    FILE* f = fopen(filename, "r");
    char buff[100];
    mtl_t* ret = malloc(sizeof(mtl_t));
    while (fgets(buff, 100, f) != NULL){
        if (strncmp("Ka ", buff, 3) == 0){
            sscanf(buff + 3, "%f %f %f", &ret->Ka[0], &ret->Ka[1], &ret->Ka[2]);
        }
        else if (strncmp("Kd ", buff, 3) == 0){
            sscanf(buff + 3, "%f %f %f", &ret->Kd[0], &ret->Kd[1], &ret->Kd[2]);
        }
        else if (strncmp("Ks ", buff, 3) == 0){
            sscanf(buff + 3, "%f %f %f", &ret->Ks[0], &ret->Ks[1], &ret->Ks[2]);
        }
        else if (strncmp("Ns ", buff, 3) == 0){
            sscanf(buff + 3, "%f", &ret->Ns);
        }
        else if (strncmp("map_Kd ", buff, 7) == 0){
            sscanf(buff + 7, "%s", ret->map_Kd);
        }
        else if (strncmp("map_normal ", buff, 11) == 0){
            sscanf(buff + 11, "%s", ret->map_normal);
        }
    }
    return ret;
}

static inline obj_t* parseObj(char *filename)
{
    int fCount = 0;
    int vCount = 0;
    int vnCount = 0;
    int vxCount = 0;
    int vyCount = 0;
    int vtCount = 0;

    FILE* file = fopen(filename, "r");
    char buff[100];

//    first pass, gather information
    while (fgets(buff, 4, file) != NULL) {
        if (strncmp(buff, "f ", 2) == 0){
            fCount += 12;
        }
        else if (strncmp(buff, "v ", 2) == 0){
            vCount += 3;
        }
        else if (strncmp(buff, "vn ", 3) == 0){
            vnCount += 3;
        }
        else if (strncmp(buff, "vx ", 3) == 0){
            vxCount += 3;
        }
        else if (strncmp(buff, "vy ", 3) == 0){
            vyCount += 3;
        }
        else if (strncmp(buff, "vt ", 3) == 0){
            vtCount += 2;
        }
    }

    int* fbuff = (int*)calloc(sizeof(int), fCount);
    float* fv = (float*)calloc(sizeof(float), fCount);
    float* ft = (float*)calloc(sizeof(float), fCount/12 * 4 * 2);
    float* fn = (float*)calloc(sizeof(float), fCount);
    float* fx = (float*)calloc(sizeof(float), fCount);
    float* fy = (float*)calloc(sizeof(float), fCount);
    float* vbuff = (float*)calloc(sizeof(float), vCount);
    float* vnbuff = (float*)calloc(sizeof(float), vnCount);
    float*  vxbuff = (float*)calloc(sizeof(float), vxCount);
    float* vybuff = (float*)calloc(sizeof(float), vyCount);
    float* vtbuff = (float*)calloc(sizeof(float), vtCount);

    fCount = vCount = vnCount = vxCount = vyCount = vtCount = 0;
    
//    second pass, parse v/vn/vx/vy/vt
    rewind(file);

    mtl_t* mtl = NULL;

    while (fgets(buff, 100, file) != NULL) {
        if (strncmp(buff, "f ", 2) == 0){
            sscanf(buff + 2, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                fbuff + fCount + 0,
                fbuff + fCount + 1,
                fbuff + fCount + 2,
                fbuff + fCount + 3,
                fbuff + fCount + 4,
                fbuff + fCount + 5,
                fbuff + fCount + 6,
                fbuff + fCount + 7,
                fbuff + fCount + 8,
                fbuff + fCount + 9,
                fbuff + fCount + 10,
                fbuff + fCount + 11);
            fCount += 12;
        }
        else if (strncmp(buff, "v ", 2) == 0){
            sscanf(buff + 2, "%e %e %e",
                vbuff + vCount + 0,
                vbuff + vCount + 1,
                vbuff + vCount + 2);
            vCount += 3;
        }
        else if (strncmp(buff, "vn ", 3) == 0){
            sscanf(buff + 3, "%e %e %e",
                vnbuff + vnCount + 0,
                vnbuff + vnCount + 1,
                vnbuff + vnCount + 2);
            vnCount += 3;
        }
        else if (strncmp(buff, "vx ", 3) == 0){
            sscanf(buff + 3, "%e %e %e",
                vxbuff + vxCount + 0,
                vxbuff + vxCount + 1,
                vxbuff + vxCount + 2);
            vxCount += 3;
        }
        else if (strncmp(buff, "vy ", 3) == 0){
            sscanf(buff + 3, "%e %e %e",
                vybuff + vyCount + 0,
                vybuff + vyCount + 1,
                vybuff + vyCount + 2);
            vyCount += 3;
        }
        else if (strncmp(buff, "vt ", 3) == 0){
            sscanf(buff + 3, "%e %e",
                vtbuff + vtCount + 0,
                vtbuff + vtCount + 1);
            vtCount += 2;
        }
        else if (strncmp(buff, "mtllib ", 7) == 0){
            strtok(buff, "\n");
            mtl = parseMtl(buff + 7);
        }
    }
    int i = 0;
//    translate face indicies to actual points
    int j = 0;
    int k = 0;
    int ttemp = 0;
    for(i = 0; i < fCount; i += 12) {
        for(k = 0; k < 3; k++) {
            fv[i + 0 + k] = *(vbuff + 3*(fbuff[i + 0] - 1) + k);
            fv[i + 3 + k] = *(vbuff + 3*(fbuff[i + 3] - 1) + k);
            fv[i + 6 + k] = *(vbuff + 3*(fbuff[i + 6] - 1) + k);
            fv[i + 9 + k] = *(vbuff + 3*(fbuff[i + 9] - 1) + k);
            fn[i + 0 + k] = *(vnbuff + 3*(fbuff[i + 0 + 2] - 1) + k);
            fn[i + 3 + k] = *(vnbuff + 3*(fbuff[i + 3 + 2] - 1) + k);
            fn[i + 6 + k] = *(vnbuff + 3*(fbuff[i + 6 + 2] - 1) + k);
            fn[i + 9 + k] = *(vnbuff + 3*(fbuff[i + 9 + 2] - 1) + k);
            if (k < 2) {
                ft[ttemp + 0 + k] =
                    *(vtbuff + 2*(fbuff[i + 0 + 1] - 1) + k);
                ft[ttemp + 2 + k] =
                    *(vtbuff + 2*(fbuff[i + 3 + 1] - 1) + k);
                ft[ttemp + 4 + k] =
                    *(vtbuff + 2*(fbuff[i + 6 + 1] - 1) + k);
                ft[ttemp + 6 + k] =
                    *(vtbuff + 2*(fbuff[i + 9 + 1] - 1) + k);
            }
            fx[i + 0 + k] = *(vbuff + 3*(fbuff[i + 0] - 1) + k);
            fx[i + 3 + k] = *(vbuff + 3*(fbuff[i + 3] - 1) + k);
            fx[i + 6 + k] = *(vbuff + 3*(fbuff[i + 6] - 1) + k);
            fx[i + 9 + k] = *(vbuff + 3*(fbuff[i + 9] - 1) + k);
            fy[i + 0 + k] = *(vbuff + 3*(fbuff[i + 0] - 1) + k);
            fy[i + 3 + k] = *(vbuff + 3*(fbuff[i + 3] - 1) + k);
            fy[i + 6 + k] = *(vbuff + 3*(fbuff[i + 6] - 1) + k);
            fy[i + 9 + k] = *(vbuff + 3*(fbuff[i + 9] - 1) + k);
        }
        ttemp += 8;
    }    

    free(vbuff);
    free(vnbuff);
    free(vxbuff);
    free(vybuff);
    free(vtbuff);
    obj_t* ret = malloc(sizeof(obj_t));
    ret->fbuff = fbuff;
    ret->vCount = vCount / 3;
    ret->fCount = fCount / 12;
    ret->vtCount = vtCount / 2;
    ret->fv = fv;
    ret->ft = ft;
    ret->fn = fn;
    ret->fx = fx;
    ret->fy = fy;
    ret->mtl = mtl;
    return ret;
}

static inline void destruct(obj_t* obj)
{
    free(obj->fbuff);
    free(obj->fv);
    free(obj->ft);
    free(obj->fn);
    free(obj->fx);
    free(obj->fy);
    free(obj->mtl);
    free(obj);
}

#endif
