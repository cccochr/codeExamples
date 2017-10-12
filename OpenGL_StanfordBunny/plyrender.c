#include "plyrender.h"

char* efgets(char *str, int n, FILE *stream) {
	if (fgets(str, n , stream) == NULL) {
		fprintf(stderr, "Error reading from stream.\n");
		exit(1);
	}
	return str;
}

struct Model readModel(char* plyFile){
	struct Model model;

	char* data_g;
	char* linebuff = calloc(sizeof(char),500);
	FILE* ply = fopen(plyFile, "r");

	efgets(linebuff, 500, ply);
	while (strncmp(linebuff, "end_header", strlen("end_header")) != 0) {
		if (strncmp(linebuff, "element vertex ", strlen("element vertex ")) == 0) {
			sscanf(linebuff + strlen("element vertex "), "%d", &model.vertex_count_g);
		}
		else if (strncmp(linebuff, "element face ", strlen("element face ")) == 0) {
			sscanf(linebuff + strlen("element face "), "%d", &model.face_count_g);
		}
		efgets(linebuff, 500, ply);
	}

//	initialze norman and vertex arrays
	model.v_array = calloc(sizeof(GLfloat), model.vertex_count_g * 3);
	model.n_array = calloc(sizeof(GLfloat), model.vertex_count_g * 3);

//	read vertexs
	int v_count = 0;
	for (v_count = 0; v_count < model.vertex_count_g * 3; v_count += 3) {
		efgets(linebuff, 500, ply);
		sscanf(linebuff, "%f %f %f %f %f %f",
			&model.v_array[v_count], &model.v_array[v_count+1], &model.v_array[v_count+2],
			&model.n_array[v_count], &model.n_array[v_count+1], &model.n_array[v_count+2]);
	}

//	read faces
	model.f_array = calloc(sizeof(float), model.face_count_g * 9);
	model.fn_array = calloc(sizeof(float), model.face_count_g * 9);
	int f_count, i = 1, j=0;
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
	glDrawArrays(GL_TRIANGLES, 0, model.face_count_g * 3);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	return 0;
}
