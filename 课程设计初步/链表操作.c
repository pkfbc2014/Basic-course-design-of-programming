#include <stdio.h>
#include <string.h>
#include <stdlib.h>
const int SIZE = 5;//5��ѧ����Ϣ�ڵ���Ϊ��������
typedef struct student
{
    int number;//ѧ��
    char name[30];//����
    int score;//�ɼ�
    struct student* next;//ָ����һ�ڵ��ָ��
}student;

student* input(student* stu)//�����٣���������
{
    int i;
    student* p;//��Ҫ������½ڵ�
    student* q;//һ������
    printf("������ѧ����ѧ�ţ��������ɼ����Կո�ָ���:\n");
    q = stu;//q��ʼָ���ͷ
    for (i = 0; i < SIZE; i++)
    {
        p = (student*)malloc(sizeof(student));//Ϊ�����������Ϣ�ڵ�����ռ�
        scanf("%d %s %d", &p->number, p->name, &p->score);
        p->next = q->next;//��֤q->nextһ����NULL���ų�ָ��յ����
        q->next = p;
        q = q->next;
    }
    return stu;
}

void output(student* stu)//�����ڣ���ͷ���������������
{
    student* p = stu->next;
    while (p != NULL)
    {
        printf("%d %s %d\n", p->number, p->name, p->score);
        p = p->next;
    }
}

student* check(student* stu)//�����ۣ��������м���ĳ���ڵ㣬����������������������������ҵ���һ���ɼ�Ϊ66��ѧ����Ϣ���������������Ϊ����
{
    student* p = stu->next;
    while (p != NULL)
    {
        if (p->score == 66)
        {
            return p;//�ҵ��˷���ָ��p
        }
        else
        {
            p = p->next;//�������������
        }
    }
    return p;
}

student* insert_s(student* stu, student* temp)//�����ܣ��������в���һ���ڵ�temp�����������������
{
    int flag = 1;//����д��2���������ҵ������ڵ㣬����Ϊʲô��
    student* p = stu->next;
    while (flag)//�ҵ��ڶ��ڵ㣬����ָ��pָ��
    {
        p = p->next;
        flag--;
    }
    temp->next = p->next;
    p->next = temp;//ע�⣺���к���һ�в��ܵ���������ᵼ�½ڵ㶪ʧ��
    return stu;
}

student* del(student* stu)//�����ݣ�ɾ�������е�ĳ���ڵ㣬�����������������
{
    int flag = 1;
    student* p = stu->next;
    student* q;//q��Ҫɾ������һ���ڵ㣨�ڱ�����ָ�����ڵ㣩
    while (flag)//�ҵ��ڶ��ڵ㣬����ָ��pָ��
    {
        p = p->next;
        flag--;
    }
    q = p->next;
    q = q->next;
    free(p->next);//�ͷŽڵ�
    p->next = q;
    return stu;
}

student* exchange(student* stu)
{
    int flag1 = 1;//�ڶ��ڵ�
    int flag2 = 4;//����ڵ�
    int flag11 = flag1-1;//�ڶ��ڵ�ǰ���ڵ�
    int flag22 = flag2-1;//����ڵ�ǰ���ڵ�
    student* p = stu->next;//ǰ���ڵ㣨�ڱ�����ָ�ڶ��ڵ㣩
    student* q = stu->next;//�󷽽ڵ㣨�ڱ�����ָ����ڵ㣩
    student* pp = stu->next;//ǰ���ڵ��ǰ���ڵ㣨�ڱ�����ָ��һ�ڵ㣩
    student* qq = stu->next;//�󷽽ڵ��ǰ���ڵ㣨�ڱ�����ָ���Ľڵ㣩
    student* temp;//�н����
    while (flag1)//��λ�ڵ�p
    {
        p = p->next;
        flag1--;
    }
    while (flag2)//��λ�ڵ�q
    {
        q = q->next;
        flag2--;
    }
    while (flag11)//��λ�ڵ�pp
    {
        pp = pp->next;
        flag11--;
    }
    while (flag22)//��λ�ڵ�qq
    {
        qq = qq->next;
        flag22--;
    }
    /*=================*///����p->next��q->next
    temp = p->next;
    p->next = q->next;
    q->next = temp;
    /*=================*///����pp->next��qq->next
    pp->next = q;
    qq->next = p;
    /*=================*///����p��q
    p = pp->next;
    q = qq->next;
    /*=================*/
    return stu;
}

int main()
{
    student* stu;//�����ͷָ��
    student* s;//�н�ָ��1
    student* ss;//�н�ָ��2
    student* temp;
    /*======================================================*///������ͷ
    stu = (student*)malloc(sizeof(student));//Ϊ��ͷ�ڵ����ռ�
    stu->next = NULL;
    /*======================================================*/
    //�����٣���������������sָ��
    s = input(stu);

    //�����ڣ�����������
    printf("\n");
    output(s);

    //�����ۣ��������м���ĳ���ڵ㣬����sָ��
    ss = check(s);
    printf("\n%d %s %d\n", ss->number, ss->name, ss->score);//����ض��ڵ��������Ϣ

    //������:�ڽ����õ�����ڶ����ڵ�����ڵ�temp�����������
    temp = (student*)malloc(sizeof(student));
    scanf("%d %s %d", &temp->number, temp->name, &temp->score);
    ss = insert_s(s, temp);
    printf("\n");
    output(ss);

    //�����ݣ�ɾ���Ѿ������õ������еĵ����ڵ㣬���������
    ss = del(s);
    printf("\n");
    output(ss);

    //�����ޣ����������õ������еĵڶ��͵���ڵ�,���������
    ss = exchange(s);
    printf("\n");
    output(ss);
    return 0;
}