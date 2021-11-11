
//对文件的中文内容进行读出      按字符方式读取文件，每读一个字符判断一下是否为汉字编码(<0)，若是则存入字符数组，不是则跳过读取下一个字符，直到文件结束。 

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

