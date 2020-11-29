#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <unistd.h>
#include <Windows.h>
#include <iostream>

using namespace std;

struct Matrix {
	int col;
	int row;
};
struct Matrix matrix[2800000];

int url_num = 0;
char* url[143667];//url
int OutNum[143667];//出度
int InNum[143667];//入度
int Matrix_num = 0;//稀疏矩阵非零元素个数
int pass_path[143667];
int path_num = 0;



int main() {
	clock_t begin_time, end_time;
	begin_time = clock();
	double cost_time;

	FILE* infile;
	FILE* outfile;
	char str[260];
	int length;
	fopen_s(&infile, "web.txt", "r");
	while (fgets(str, 260, infile)) {
		length = strlen(str);
		str[length - 1] = '\0';
		url[url_num] = (char*)malloc(length);
		strcpy_s(url[url_num++], length, str);
	}
	fclose(infile);
	fopen_s(&infile, "graph.bin", "r");
	int source, destination;
	while (fscanf_s(infile, "%d %d\n", &source, &destination) != EOF) {
		OutNum[source]++;
		InNum[destination]++;
		matrix[Matrix_num].col = source;
		matrix[Matrix_num++].row = destination;
	}
	printf("finish read\n");


	return 0;
}