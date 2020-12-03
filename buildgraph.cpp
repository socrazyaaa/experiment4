#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include <time.h>
#include <unistd.h>
#include <dirent.h>

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

int readFileList(char *basePath,FILE* outfile)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
            fprintf(outfile,"%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            fprintf(outfile,"%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base,outfile);
        }
    }
    closedir(dir);
    return 1;
}


int main() {
	clock_t begin_time, end_time;
	begin_time = clock();
	double cost_time;
    char basePath[300];

    FILE* dir_file;
    dir_file = fopen("dir.txt","w");
    ///get the current absoulte path
    memset(basePath,'\0',sizeof(basePath));
    getcwd(basePath, 299);
    printf("the current dir is : %s\n",basePath);

    ///get the file list
    memset(basePath,'\0',sizeof(basePath));
    strcpy(basePath,"./news.sohu.com");
    readFileList(basePath,dir_file);

	FILE* web_url;
	char dir[300];
	char url[300];
	int length;
	int url_offset;
	int hash;
	int node_num = 0;
	int edge_num = 0;
	Hash_node* temp;
	fclose(dir_file);
	dir_file = fopen("dir.txt", "r");
	web_url = fopen("web.txt", "w");
	int url_id = 0;
	fgets(dir, 300, dir_file);
	url_offset = strstr(dir, "news.sohu.com") - dir;
	do {
		strcpy(url, dir + url_offset);
		length = strlen(url);
		if (!(url[length - 2] == 'l' && url[length - 3] == 'm' && url[length - 4] == 't' && url[length - 5] == 'h')) {
			continue;
		}
		url[length - 1] = '\0';
		hash = ELFHash(url, 0, length - 1) % HASHTABLE_SIZE;
		if (hash_table[hash] == NULL) {
			temp = (Hash_node*)malloc(sizeof(Hash_node));
			temp->next = NULL;
			temp->url = (char*)malloc(length);
			strcpy(temp->url, url);
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
			strcpy(temp->url, url);
			temp->url_id = url_id;
		}
		fprintf(web_url, "%s\n", url);
		url_id++;
	} while (fgets(dir, 300, dir_file));
	fclose(dir_file);
	fclose(web_url);

	web_url = fopen("web.txt", "r");
	FILE* outfile;
	FILE* infile;
	int html_length;
	outfile = fopen("graph.bin", "w");
	regex_t reg;
	regcomp(&reg, "news.sohu.com([\\-|A-Za-z0-9\\(\\)\\ \\_\\/\\[\\-]+).([Ss]?html)", REG_EXTENDED);
	regmatch_t pm[2];
	const size_t nmatch = 2;
	url_id = 0;
	int n = 0;
	int pre_length = 1;
	while (fgets(url, 300, web_url)) {
		url[strlen(url) - 1] = '\0';
		if (infile = fopen(url, "rb")) {
			fseek(infile, 0L, SEEK_END);
			html_length = ftell(infile);
			fseek(infile, 0L, 0);
			if (pre_length < html_length) {
				html = (char*)realloc(html, html_length);
				pre_length = html_length;
			}
			fread(html, 1, html_length, infile);
			fclose(infile);
			p = html;
			while (!regexec(&reg, p, nmatch, pm, REG_EXTENDED)) {
				hash = ELFHash(p, pm[0].rm_so, pm[0].rm_eo) % HASHTABLE_SIZE;
				temp = hash_table[hash];
				while (temp != NULL) {
					if (strcmp(temp->url, pm[0].rm_so, pm[0].rm_eo)) {
						fprintf(outfile, "%d %d\n", url_id, temp->url_id);
						edge_num++;
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
	node_num = url_id;

	end_time = clock();
	cost_time = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
	printf("{runtime: %lfs, node_num: %d, edge_num:%d\n}", cost_time, node_num, edge_num);
	sleep(3);
	return 0;
}
