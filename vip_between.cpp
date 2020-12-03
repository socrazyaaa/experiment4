#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <unistd.h>
#include <Windows.h>
#include <iostream>

using namespace std;
#define queue_size 10000000

struct Matrix {
	int col;
	int row;
};
struct Matrix matrix[2800000];

int url_num = 0;
char* url[143667];//url
int OutNum[143667];//����
int InNum[143667];//���
int Matrix_num = 0;//ϡ��������Ԫ�ظ���
int pass_path_num[143667];
int path_num = 0;
int url_edge_start[143667];
int pre[143667];
int distances[143667];
short visit[143667];
double centrality[143667];
double top_centrality[20];
int top_id[20];
int queue[queue_size];

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

void path(int source) {
	int now;
	for (int i = 0; i < url_num; i++) {
		if (pre[i] == -1) {
			continue;
		}
		else if (pre[i] == source) {
			path_num++;
			continue;
		}
		now = i;
		path_num++;
		while (pre[now] != source) {
			pass_path_num[pre[now]]++;
			now = pre[now];
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
	memset(url_edge_start, -1, sizeof(url_edge_start));
	int num = -1;
	while (fscanf_s(infile, "%d %d\n", &source, &destination) != EOF) {
		OutNum[source]++;
		InNum[destination]++;
		matrix[Matrix_num].col = source;
		if (num != source) {
			num = source;
			url_edge_start[num] = Matrix_num;
		}
		matrix[Matrix_num++].row = destination;
	}
	printf("finish read\n");

	memset(pass_path_num, 0, sizeof(pass_path_num));
	for (int i = 0; i < url_num; i++) {
		if (i % 1000 == 0) {
			cout << i << endl;
		}
		memset(distances, -1, sizeof(distances));
		//memset(visit, -1, sizeof(visit));
		memset(pre, -1, sizeof(pre));
		int front = 0;
		int last = 0;
		queue[last++] = i;
		distances[i] = 0;
		int now;
		while (front != last) {
			now = queue[front++];
			front %= queue_size;
			for (int i = url_edge_start[now]; i < Matrix_num && matrix[i].col == now; i++) {
				if (distances[matrix[i].row] == -1) {
					queue[last++] = matrix[i].row;
					last %= queue_size;
					//visit[matrix[i].row] = 0;
					if (last == front) {
						cout << "queue is not enough" << endl;
					}
					distances[matrix[i].row] = distances[now] + 1;
					pre[matrix[i].row] = now;
				}
			}
		}
		path(i);
	}

	for (int i = 0; i < url_num; i++) {
		centrality[i] = (double)pass_path_num[i] / (double)path_num;
	}
	FindTop();
	fclose(infile);
	fopen_s(&outfile, "result.txt", "w");
	for (int i = 0; i < 20; i++) {
		fprintf(outfile, "%s %.16lf\n", url[top_id[i]], top_centrality[i]);
	}

	end_time = clock();
	cost_time = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
	printf("{runtime: %lf}", cost_time);
	return 0;
}
