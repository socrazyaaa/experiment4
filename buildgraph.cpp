#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include <time.h>
//#include <unistd.h>
#include <Windows.h>
#include <iostream>

using namespace std;

#define HASHTABLE_SIZE 1000

struct Hash_node {
	char* url;
	int url_id;
	struct Hash_node* next;
};
Hash_node* hash_table[1000];
char* html = (char*)malloc(1);
char* p;

unsigned int ELFHash(char* str,int start,int end)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	while (start < end)
	{
		hash = (hash << 4) + (str[start++]);
		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	return (hash & 0x7FFFFFFF);
}

int strcmp(char* str, int start, int end) {
	if ((end - start) != strlen(str)) {
		return 0;
	}
	while (*str && start < end) {
		if (*str++ != p[start++]) {
			return 0;
		}
	}
	return 1;
}

int main() {
	clock_t begin_time, end_time;
	begin_time = clock();
	double cost_time;

	system("DIR /D news.sohu.com /b /s /a:a > dir.txt");
	FILE* dir_file;
	FILE* web_url;
	char dir[300];
	char url[300];
	int length;
	int url_offset;
	int hash;
	Hash_node* temp;
	fopen_s(&dir_file, "dir.txt", "r");
	fopen_s(&web_url, "web.txt", "w");
	int url_id = 0;
	fgets(dir, 300, dir_file);
	url_offset = strstr(dir, "news.sohu.com") - dir;
	cout << "url_offset:" << url_offset << endl;
	do {
		strcpy_s(url, strlen(dir + url_offset) + 1, dir + url_offset);
		length = strlen(url);
		if (!(url[length - 2] == 'l' && url[length - 3] == 'm' && url[length - 4] == 't' && url[length - 5] == 'h')) {
			continue;
		}
		for (int i = 0; i < length; i++) {
			if (url[i] == '\\') {
				url[i] = '/';
			}
		}
		url[length - 1] = '\0';
		hash = ELFHash(url, 0, length - 1) % HASHTABLE_SIZE;
		if (hash_table[hash] == NULL) {
			temp = (Hash_node*)malloc(sizeof(Hash_node));
			temp->next = NULL;
			temp->url = (char*)malloc(length);
			strcpy_s(temp->url, length, url);
			temp->url_id = url_id;
			hash_table[hash] = temp;
		}
		else {
			temp = hash_table[hash];
			while (temp->next != NULL) {
				temp = temp->next;
			}
			temp->next = (Hash_node*)malloc(sizeof(Hash_node));
			temp = temp->next;
			temp->next = NULL;
			temp->url = (char*)malloc(length);
			strcpy_s(temp->url, length, url);
			temp->url_id = url_id;
		}
		fprintf(web_url, "%s\n", url);
		url_id++;
	} while (fgets(dir, 300, dir_file));
	fclose(dir_file);
	fclose(web_url);

	fopen_s(&web_url, "web.txt", "r");
	FILE* outfile;
	FILE* infile;
	int html_length;
	//char destination[300];
	fopen_s(&outfile, "graph.bin", "w");
	regex_t reg;
	regcomp(&reg, "news.sohu.com([A-Za-z0-9\\-\\_\\%\\[\\&\\?\\/\\=\\.]+)", REG_EXTENDED);
	regmatch_t pm[2];
	const size_t nmatch = 2;
	url_id = 0;
	int n = 0;
	while (fgets(url, 300, web_url)) {
		if (n++ % 1000 == 0) {
			cout << n << endl;
		}
		url[strlen(url) - 1] = '\0';
		if (!fopen_s(&infile, url, "r")) {
			fseek(infile, 0L, SEEK_END);
			html_length = ftell(infile);
			fseek(infile, 0L, 0);
			free(html);
			html = (char*)malloc(html_length);
			fread(html, 1, html_length, infile);
			fclose(infile);
			p = html;
			while (!regexec(&reg, p, nmatch, pm, REG_EXTENDED)) {
				/*strncpy(destination, p + pm[0].rm_so, pm[0].rm_eo - pm[0].rm_so);
				destination[pm[0].rm_eo - pm[0].rm_so] = '\n';
				destination[pm[0].rm_eo - pm[0].rm_so + 1] = '\0';*/
				//cout << destination;
				if (!(p[pm[0].rm_eo - 1] == 'l' && p[pm[0].rm_eo - 2] == 'm' && p[pm[0].rm_eo - 3] == 't' && p[pm[0].rm_eo - 4] == 'h')) {
					p += pm[0].rm_eo;
					continue;
				}
				hash = ELFHash(p, pm[0].rm_so, pm[0].rm_eo) % HASHTABLE_SIZE;
				temp = hash_table[hash];
				while (temp != NULL) {
					if (strcmp(temp->url, pm[0].rm_so, pm[0].rm_eo)) {
						fprintf(outfile, "%d %d\n", url_id, temp->url_id);
						break;
					}
					temp = temp->next;
				}
				p += pm[0].rm_eo;
			}
		}
		url_id++;
	}
	regfree(&reg);

	end_time = clock();
	cost_time = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
	printf("{runtime: %lf}", cost_time);
	return 0;
}