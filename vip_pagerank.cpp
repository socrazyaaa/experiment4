#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
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
int Matrix_num = 0;//稀疏矩阵非零元素个数
double pagerank[143667];//特征向量
double pagerank_last[143667];

int top_id[20];
double top_PageRank[20];

int cmp(const void* a, const void* b) {
	return ((Matrix*)a)->row - ((Matrix*)b)->row;
}

void FindTop()
{
	int i = 0;
	int j = 0;
	for (i = 0; i < url_num; i++)
	{
		for (j = 0; j < 20; j++)
		{
			if (pagerank[i] > top_PageRank[j])
			{
				for (int q = 20 - 1; q > j; q--)
				{
					top_PageRank[q] = top_PageRank[q - 1];
					top_id[q] = top_id[q - 1];
				}
				top_PageRank[j] = pagerank[i];
				top_id[j] = i;
				break;
			}
		}
	}
}

int main() {
	clock_t begin_time, end_time;
	begin_time = clock();
	double cost_time;

	FILE* infile;
	FILE* outfile;
	char str[260];
	int length;
	infile = fopen("web.txt", "r");
	while (fgets(str, 260, infile)) {
		length = strlen(str);
		str[length - 1] = '\0';
		url[url_num] = (char*)malloc(length);
		strcpy(url[url_num++], str);
	}
	fclose(infile);
	infile = fopen("graph.bin", "r");
	int source, destination;
	while (fscanf(infile, "%d %d\n", &source, &destination) != EOF) {
		OutNum[source]++;
		matrix[Matrix_num].col = source;
		matrix[Matrix_num++].row = destination;
	}
	qsort(matrix, Matrix_num, sizeof(Matrix), cmp);
	double precision = 0.0001;//精度
	double sum;
	int iter_num = 0;
	for (int i = 0; i < url_num; i++) {
		pagerank[i] = 1.0;
	}
	double delta = 1;
	while (1) {
		iter_num++;
		int j = 0;
		delta = 0.0;
		double pagerank_sum = 0;
		for (int i = 0; i < url_num; i++) {
			pagerank_sum += pagerank[i];
		}
		for (int i = 0; i < url_num; i++) {
			sum = 0;
			while (matrix[j].row == i && j < Matrix_num) {
				sum += pagerank[matrix[j].col] / OutNum[matrix[j].col];
				j++;
			}
			sum = pagerank_sum * 0.15 / url_num + 0.85 * sum;
			delta += (pagerank[i] - sum) * (pagerank[i] - sum);
			pagerank[i] = sum;
		}
		if (delta <= precision * precision) {
			break;
		}
	}
	FindTop();
	fclose(infile);
	outfile = fopen("result.txt", "w");
	for (int i = 0; i < 20; i++) {
		fprintf(outfile, "%s %.16lf\n", url[top_id[i]], top_PageRank[i]);
	}

	end_time = clock();
	cost_time = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
	printf("{runtime: %lfs, iter_num: %d}", cost_time, iter_num);
	sleep(3);
	return 0;
}
