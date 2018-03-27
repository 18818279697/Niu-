
#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//函数返回fname指定文件的全部内容，如果打不开文件，则返回NULL，并显示打开文件错误 
char *getfileall(char *fname)
{
	FILE *fp;
	char *str;
	char txt[1000];
	int filesize;
	if ((fp = fopen(fname, "r")) == NULL) {
		printf("打开文件%s错误\n", fname);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);

	filesize = ftell(fp);
	str = (char *)malloc(filesize);
	str[0] = 0;

	rewind(fp);
	while ((fgets(txt, 1000, fp)) != NULL) {
		strcat(str, txt);
	}
	fclose(fp);
	return str;
}
int main(int argc, char *argv[])
{
	printf("%d,%s\n",argc,argv[1]);
	char *p;
	char s[100];
	printf("input path\n");
	scanf("%s",s);
	char *fname =s;
	p = getfileall(fname);
	if (p != NULL) puts(p);
	return 0;
}
//nnn