
//���ļ����������ݽ��ж���      ���ַ���ʽ��ȡ�ļ���ÿ��һ���ַ��ж�һ���Ƿ�Ϊ���ֱ���(<0)������������ַ����飬������������ȡ��һ���ַ���ֱ���ļ������� 

#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char s[1000],ch;
    FILE *fp;
    int i=0;
    if((fp=fopen("test.txt","r"))==NULL)
		{
        printf("Open the file failure...\n");
        exit(0);
    }
    
    while((ch=fgetc(fp))!=EOF)
        if(ch<0)
            s[i++]=ch;
    fclose(fp);
    s[i]='\0';
    printf("%s\n",s);
    return 0;
}

