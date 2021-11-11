#include <stdio.h>
#include <string.h>
#include <stdlib.h>
const int SIZE = 5;//5个学生信息节点作为测试样例
typedef struct student
{
    int number;//学号
    char name[30];//姓名
    int score;//成绩
    struct student* next;//指向下一节点的指针
}student;

student* input(student* stu)//操作①：创建链表
{
    int i;
    student* p;//将要插入的新节点
    student* q;//一个引子
    printf("请输入学生的学号，姓名，成绩（以空格分隔）:\n");
    q = stu;//q初始指向表头
    for (i = 0; i < SIZE; i++)
    {
        p = (student*)malloc(sizeof(student));//为即将插入的信息节点申请空间
        scanf("%d %s %d", &p->number, p->name, &p->score);
        p->next = q->next;//保证q->next一定是NULL，排除指向空的情况
        q->next = p;
        q = q->next;
    }
    return stu;
}

void output(student* stu)//操作②：从头遍历链表（并输出）
{
    student* p = stu->next;
    while (p != NULL)
    {
        printf("%d %s %d\n", p->number, p->name, p->score);
        p = p->next;
    }
}

student* check(student* stu)//操作③：在链表中检索某个节点，并带回主函数输出（在这里我以找到第一个成绩为66的学生信息，带回主函数输出为例）
{
    student* p = stu->next;
    while (p != NULL)
    {
        if (p->score == 66)
        {
            return p;//找到了返回指针p
        }
        else
        {
            p = p->next;//否则继续往下找
        }
    }
    return p;
}

student* insert_s(student* stu, student* temp)//操作④：在链表中插入一个节点temp，并带回主函数输出
{
    int flag = 1;//不能写成2，那样会找到第三节点，想想为什么？
    student* p = stu->next;
    while (flag)//找到第二节点，并用指针p指向
    {
        p = p->next;
        flag--;
    }
    temp->next = p->next;
    p->next = temp;//注意：此行和上一行不能调换，否则会导致节点丢失！
    return stu;
}

student* del(student* stu)//操作⑤：删除链表中的某个节点，并带回主函数，输出
{
    int flag = 1;
    student* p = stu->next;
    student* q;//q是要删除的那一个节点（在本题中指第三节点）
    while (flag)//找到第二节点，并用指针p指向
    {
        p = p->next;
        flag--;
    }
    q = p->next;
    q = q->next;
    free(p->next);//释放节点
    p->next = q;
    return stu;
}

student* exchange(student* stu)
{
    int flag1 = 1;//第二节点
    int flag2 = 4;//第五节点
    int flag11 = flag1-1;//第二节点前驱节点
    int flag22 = flag2-1;//第五节点前驱节点
    student* p = stu->next;//前方节点（在本题中指第二节点）
    student* q = stu->next;//后方节点（在本题中指第五节点）
    student* pp = stu->next;//前方节点的前驱节点（在本题中指第一节点）
    student* qq = stu->next;//后方节点的前驱节点（在本题中指第四节点）
    student* temp;//中介变量
    while (flag1)//定位节点p
    {
        p = p->next;
        flag1--;
    }
    while (flag2)//定位节点q
    {
        q = q->next;
        flag2--;
    }
    while (flag11)//定位节点pp
    {
        pp = pp->next;
        flag11--;
    }
    while (flag22)//定位节点qq
    {
        qq = qq->next;
        flag22--;
    }
    /*=================*///交换p->next和q->next
    temp = p->next;
    p->next = q->next;
    q->next = temp;
    /*=================*///交换pp->next和qq->next
    pp->next = q;
    qq->next = p;
    /*=================*///交换p和q
    p = pp->next;
    q = qq->next;
    /*=================*/
    return stu;
}

int main()
{
    student* stu;//定义表头指针
    student* s;//中介指针1
    student* ss;//中介指针2
    student* temp;
    /*======================================================*///建立表头
    stu = (student*)malloc(sizeof(student));//为表头节点分配空间
    stu->next = NULL;
    /*======================================================*/
    //操作①：建立单链表，并用s指向
    s = input(stu);

    //操作②：输出这个链表
    printf("\n");
    output(s);

    //操作③：在链表中检索某个节点，并用s指向
    ss = check(s);
    printf("\n%d %s %d\n", ss->number, ss->name, ss->score);//输出特定节点的所有信息

    //操作④:在建立好的链表第二三节点间插入节点temp，并输出检验
    temp = (student*)malloc(sizeof(student));
    scanf("%d %s %d", &temp->number, temp->name, &temp->score);
    ss = insert_s(s, temp);
    printf("\n");
    output(ss);

    //操作⑤：删除已经建立好的链表中的第三节点，并输出检验
    ss = del(s);
    printf("\n");
    output(ss);

    //操作⑥：交换建立好的链表中的第二和第五节点,并输出检验
    ss = exchange(s);
    printf("\n");
    output(ss);
    return 0;
}