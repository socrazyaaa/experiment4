#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define stack_size 500000
#define queue_size 500000

struct Matrix {
	int col;
	int row;
};
struct Matrix matrix[2800000];
struct Pred {
	int num;
	int max_length;
	int* value;
};
struct Pred pred[143667];

int url_num = 0;
char* url[143667];//url
int Matrix_num = 0;//稀疏矩阵非零元素个数
double centrality[143667];
int queue[queue_size];
int stack[stack_size];
int sigma[143667];
int distances[143667];
int url_edge_start[143667];
double delta[143667];
double top_centrality[20];
int top_id[20];

void FindTop()
{
	int i = 0;
	int j = 0;
	for (i = 0; i < url_num; i++)
	{
		for (j = 0; j < 20; j++)
		{
			if (centrality[i] > top_centrality[j])
			{
				for (int q = 20 - 1; q > j; q--)
				{
					top_centrality[q] = top_centrality[q - 1];
					top_id[q] = top_id[q - 1];
				}
				top_centrality[j] = centrality[i];
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
	for (int i = 0; i < 143667; i++) {
		url_edge_start[i] = -1;
	}
	int num = -1;
	while (fscanf_s(infile, "%d %d\n", &source, &destination) != EOF) {
		matrix[Matrix_num].col = source;
		if (num != source) {
			num = source;
			url_edge_start[num] = Matrix_num;
		}
		matrix[Matrix_num++].row = destination;
	}

	memset(centrality, 0, sizeof(centrality));
	for (int j = 0; j < 143667; j++) {
		pred[j].max_length = 10;
		pred[j].value = (int*)malloc(sizeof(int) * 10);
	}
	for (int i = 0; i < url_num; i++) {
		int top = -1;
		for (int j = 0; j < 143667; j++) {
			pred[j].num= 0;
		}
		int front = 0, last = 0;
		memset(sigma, 0, sizeof(sigma));
		memset(distances, -1, sizeof(distances));
		sigma[i] = 1;
		distances[i] = 0;
		queue[last++] = i;
		last %= queue_size;
		while (last != front) {
			stack[++top] = queue[front++];
			if (top > stack_size) {
				printf("stack is not enough\n");
			}
			front %= queue_size;
			for (int j = url_edge_start[stack[top]]; j < Matrix_num && matrix[j].col == stack[top]; j++) {
				if (distances[matrix[j].row] < 0) {
					queue[last++] = matrix[j].row;
					last %= queue_size;
					if (last == front) {
						printf("queue is not enough\n");
					}
					distances[matrix[j].row] = distances[stack[top]] + 1;
				}
				if (distances[matrix[j].row] == distances[stack[top]] + 1) {
					sigma[matrix[j].row] += sigma[stack[top]];
					pred[matrix[j].row].value[pred[matrix[j].row].num++] = stack[top];
					if (pred[matrix[j].row].num == pred[matrix[j].row].max_length) {
						pred[matrix[j].row].max_length *= 2;
						pred[matrix[j].row].value = (int*)realloc(pred[matrix[j].row].value, sizeof(int) * pred[matrix[j].row].max_length);
					}
				}
			}
		}
		for (int j = 0; j < 143667; j++) {
			delta[j] = 0.0;
		}
		while (top != -1) {
			for (int j = 0; j < pred[stack[top]].num; j++) {
				delta[pred[stack[top]].value[j]] += (1 + delta[stack[top]]) * (double)sigma[pred[stack[top]].value[j]] / (double)sigma[stack[top]];
			}
			if (stack[top] != i) {
				centrality[stack[top]] += delta[stack[top]];
			}
			top--;
		}
	}

	FindTop();
	fclose(infile);
	fopen_s(&outfile, "result.txt", "w");
	for (int i = 0; i < 20; i++) {
		fprintf(outfile, "%s %.16lf\n", url[top_id[i]], top_centrality[i]);
	}

	end_time = clock();
	cost_time = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
	printf("{runtime: %lfs}", cost_time);
	sleep(3);
	return 0;
}