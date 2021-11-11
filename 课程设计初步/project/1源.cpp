#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#define MAXNUMI 900
/*==================================================================================*///�ṹ����ӻ�
time_t t;//��t����Ϊʱ�����                                         //�趨ʱ��
struct tm* p;//struct tm��һ���ṹ�壬����һ���ṹ��ָ��
float turnover;
int Departmentnumber = 0;
int Examinationnumber = 0;
int Drugnumber = 0;
int judge(int a)//�ж�ĳ�����м���
{

	if (a == 2) {
		return 29;
	}
	else if (a == 1 || a == 3 || a == 5 || a == 7 || a == 8 || a == 10 || a == 12) {
		return 31;
	}
	else return 30;
}
int count(int a, int b, int c, int d)//��ȡ��������֮�������
{
	int k = 0;
	if (a < c) {
		k += judge(a) - b;
		a++;
		while (a < c) {
			k += judge(a);
			a++;
		}
		k += d;
	}
	else {
		k += d - b;
	}return k;
}
typedef struct Patient					//������Ϣ
{
	int ID;
	char name[30];						//��������
	int age;							//����
	int Register;   //����ĹҺţ���ֵ������500


} Patient;
typedef struct Doctor					//ҽ����Ϣ
{
	char name[30];						//ҽ������
	int department;	//����
	int ID;
	int level;
} Doctor;
typedef struct Situation				//�������
{
	int CheckCode;						//������Щ���
	int month, day, hour, minute;		//����ʱ��
	int checktotal;						//�����������
	int check[30];						//������࣬�����ʮ��
	float checksum;						//����ܼ�
	int drugtotal;						//ҩƷ��������
	int drug[30];						//ҩƷ���࣬�����ʮ��
	float drugsum;						//ҩƷ�ܼ�
} Situation;
typedef struct Record					//���Ƽ�¼
{
	struct Patient patient;
	struct Doctor doctor;
	struct Situation situation;
	/*���Ƽ�¼��Ӧҽ����������ڵ��е����Ƽ�¼ָ�������Ԫ�ص�ַ,
	Ϊ�ˡ�ɾ�������Ƽ�¼ʱ����Ӧ���������ָ�����ÿգ�ָ��ʹ��ǰҪ�ж��Ƿ�ǿ�*/
	struct Record** Doctorptr;
	struct Record** Departmentptr;
	struct Record** Patientptr;
	struct Record* next;
} Record;
typedef struct PatientIndex 			//���߼�������ڵ�
{
	int ID;
	char IDcard[30];
	char name[30];						//��������
	int times2;							//סԺ֮��Ŀ۷Ѵ���
	int inmonth;						//סԺ��ʼ��
	int inday;							//סԺ��ʼ��
	int outmonth;						//��Ժ��
	int outday;							//��Ժ��
	int deposit;						//Ѻ�𣬳��ν��ɲ�������סԺ����*200
	int flag[368]; 						//�û�������һ����ÿһ��ҵĺ�����
	struct Record* index[900];
	int PatientIndexTOP;				//���߼�������ջ�����
	struct PatientIndex* next;
} PatientIndex;
typedef struct DoctorIndex				//ҽ����������ڵ�
{
	char name[20];						//ҽ������
	int ID;								//ҽ��ID
	int DoctorTime;						//����ʱ��
	int DoctorLevel;					//ҽ���ȼ�
	int flag[366];						//��ҽ������һ����ÿһ�챻�ҵĺ�����
	struct Record* index[900];
	struct DoctorIndex* next;
	int DoctorIndexTOP;					//ҽ����������ջ�����
} DoctorIndex;
typedef struct DepartmentIndex			//���Ҽ�������ڵ�
{
	int ID;								//�������ƣ���ţ�
	struct Record* index[900];
	struct DepartmentIndex* next;
	int DepartmentIndexTOP;				//���Ҽ�������ջ�����
} DepartmentIndex;
/*==================================================================================*/
int judgechinese(char* p)//���ú���ASCII�������ֽڵ����λ����1�������ж��Ƿ����뺺��
{
	int i = 0;
	while (p[i] != '\0') {
		if ((p[i] >> 8) == 0) //��p�ֽڽ�����λ���㣬����8λ�������������λ����0����˵��ԭ�����ֽ����λΪ0������1��ôҲ�Ͳ��Ǻ��ֵ�һ���ֽڡ�
		{
			return 0;
		}

		i++;
	}
	return 1;
}
char PatientName[40][20];				//���л��ߵ�����
char Department[33][20];				//��������
char Doctorlevel[15][16];				//ҽ������
char Examination[30][30];				//�����Ŀ����
char DrugsName[40][30];					//ҩƷ��
int Flag[368] = { 0 };						//��ҽԺ����һ����ÿһ�챻�ҵĺ�����
float DrugsPrice[40];					//ҩƷ�۸�
int TotalPatientID = 0;					//���߱���н����
Record* Recordhead;						//���Ƽ�¼����ͷ���
PatientIndex* PatientIndexhead;			//���߼�������ͷ���
DoctorIndex* DoctorIndexhead;			//ҽ����������ͷ���
DepartmentIndex* DepartmentIndexhead;	//���Ҽ�������ͷ���
int PatientIndexTOP = 0;				//���߼�������ջ�����
int DoctorIndexTOP = 0;					//ҽ����������ջ�����
int DepartmentIndexTOP = 0;				//���Ҽ�������ջ�����
int TotalRecord = 0;					//��ҽԺϵͳ�������������������ͳ��ҽ����æ�̶ȣ�
FILE* nfp;//test�ļ�
int reverse(int number)//������number���򷵻�
{
	int a, sum = 0;
	while (number)
	{
		a = number % 10;
		sum = sum * 10 + a;
		number /= 10;
	}
	return sum;
}
int DayDiff(int month_end, int day_end)//�������1��1��ʱ���
{
	int month_start = 1;
	int day_start = 1;
	int m1, d1;
	int m2, d2;
	m1 = (month_start + 9) % 12;
	d1 = (m1 * 306 + 5) / 10 + (day_start - 1);
	m2 = (month_end + 9) % 12;
	d2 = (m2 * 306 + 5) / 10 + (day_end - 1);
	return (d2 - d1);
}
void Examinationfun_Hand(Record* p)		//��飬�ֶ����		 
{
	int n, temp1;
	float temp2 = 0;
	printf("\n\t�����뻼�߽��м�����Ŀ��:");
	while(scanf("%d", &p->situation.checktotal)!=1||p->situation.checktotal> Examinationnumber||p->situation.checktotal<0) {
		printf("\n\t���������������������룺");
	}
	n = p->situation.checktotal;
	printf("\n\t�����Ŀ:");
	int i = 0;
	printf("\n\t|____________________________________________�����Ŀ�б�_____________________________________________|\t\n\n");
	for (; i < Examinationnumber; i++)
	{

		printf("\t%d.%s       ", i, Examination[i]);
		if (i % 4 == 0 && i != 0)
		{
			printf("\n");
		}
	}
	printf("\n\t|_______________________________________________________________________________________________|\t\n");

	while (n > 0)
	{
		printf("\n\t��ѡ���߽��м�����Ŀ��");
		while(scanf("%d", &temp1)!=1||temp1>Examinationnumber-1||temp1<0) {
			printf("\n\t���������������������룺");
		}
		printf("\n\t�����뻼�߼����ã�");
		while(scanf("%f", &temp2)!=1||temp2<0) {
			printf("\n\t���������������������룺");
		}//��������Ŀ
		turnover += temp2;
		p->situation.check[temp1] = 1;//��Ǹü����
		p->situation.checksum += temp2;//���ϼ����Ŀ����
		n--;
	}
}//��飨�ֶ���ӣ�
void Examinationfun_Auto(Record* p)					//��飨�Զ���ӣ�
{
	int n, temp1;
	float temp2 = 0;
	fscanf(nfp, "%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(nfp, "%d %f", &temp1, &temp2);//��������Ŀ
		p->situation.check[temp1] = 1;//��Ǹü����
		p->situation.checksum += temp2;//���ϼ����Ŀ����
		n--;
	}
}
void Prescribing_medicinefun_Hand(Record* p)	//��ҩ���ֶ���ӣ�
{
	int n, temp1, temp2;
	printf("\n\t�����뻼������ҩƷ��������:");
	while(scanf("%d", &p->situation.drugtotal)!=1||p->situation.drugtotal<0||p->situation.drugtotal>Drugnumber) {
		printf("\n\t���������������������룺");
	}
	n = p->situation.drugtotal;
	int i = 0;
	printf("\n\t|_________________________________________ҩƷ�б�__________________________________________________|\t\n\n");
	for (; i < Drugnumber; i++)
	{
		printf("\t%d.%s      ", i, DrugsName[i]);
		if (i % 5 == 0 && i != 0)
			printf("\n\n");
	}
	printf("\n\t|___________________________________________________________________________________________________|\t\n\n");
	while (n > 0)
	{
		printf("\n\t��ѡ��һ���������ҩƷ:");
		while(scanf("%d", &temp1)!=1||temp1<0||temp1>Drugnumber-1) {
			printf("\n\t���������������������룺");
		}
		printf("\n\t�����뻼����������ҩƷ������:");
		while(scanf("%d",  &temp2)!=1||temp2<0||temp2>100) {
			printf("\n\t���������������������룺");
		}
		turnover += DrugsPrice[temp1] * temp2;
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		p->situation.drug[temp1] = temp2;//��Ǹ�ҩƷ����
		n--;
	}
}
void Prescribing_medicinefun_Auto(Record* p)		//��ҩ���Զ���ӣ�
{
	int n, temp1, temp2;
	fscanf(nfp, "%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(nfp, "%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//��Ǹ�ҩƷ����
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void hospitalizationfun_Hand(PatientIndex* q)			//סԺ���ֶ���ӣ�
{
	int k = 0;
	printf("\n\t�����뻼����Ժ����:");
	printf("\n\n\t��:");
	while (scanf("%d", &q->inmonth) != 1 || q->inmonth > 12 || q->inmonth < 0) {
		printf("\n\t���������������������룺");
	}
	printf("\n\t��:");
	while (scanf("%d", &q->inday) != 1 || q->inday < 0||q->inday>31) {
		printf("\n\t���������������������룺");
	}
	printf("\n\t�����뻼��Ԥ�Ƴ�Ժ����:");
	printf("\n\n\t��:");
	while (scanf("%d", &q->outmonth) != 1 || q->outmonth > 12 || q->outmonth < 0) {
		printf("\n\t���������������������룺");
	}
	printf("\n\t��:");
	while (scanf("%d", &q->outday) != 1 || q->outday < 0 || q->outday>31) {
		printf("\n\t���������������������룺");
	}
	k = count(q->inmonth, q->inday, q->outmonth, q->outday) * 200;
	printf("\n\t�����뻼�߽��ɵ�Ѻ����Ŀ:");
	scanf("%d", &(q->deposit));
	int j = q->deposit;
	while (j - k < 0 || j < 1000) {
		printf("\n\t����Ѻ����Ŀ���㣡����������:");
		scanf("%d", &(q->deposit));
		j = q->deposit;
	}
}
void hospitalizationfun_Auto(PatientIndex* p)		//סԺ���Զ���ӣ�
{
	fscanf(nfp, "%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void Preload1()//Ԥ������Դ��
{
	FILE* fp;
	int i = 0;
	if ((fp = fopen("Database\\Drugs.txt", "r")) == NULL)//����ҩƷ��Ϣ
	{
		printf("cannot open the Drugs file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		Drugnumber++;
		fscanf(fp, "%s %f", DrugsName[i], &DrugsPrice[i]);
		i++;
	}
	fclose(fp);
	i = 0;

	if ((fp = fopen("Database\\Department.txt", "r")) == NULL)//���ؿ�������
	{
		printf("cannot open the Department file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		Departmentnumber++;
		fscanf(fp, "%s", Department[i]);
		i++;
	}
	fclose(fp);
	i = 0;

	if ((fp = fopen("Database\\Doctorlevel.txt", "r")) == NULL)//����ҽ������
	{
		printf("cannot open the Doctorlevel file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		fscanf(fp, "%s", Doctorlevel[i]);
		i++;
	}
	fclose(fp);
	i = 0;

	if ((fp = fopen("Database\\Examination.txt", "r")) == NULL)//���ؼ����Ŀ����
	{
		printf("cannot open the Examination file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		Examinationnumber++;
		fscanf(fp, "%s", Examination[i]);
		i++;
	}
	fclose(fp);
}

void Preload2()//Ԥ����ҽ������
{
	FILE* fp;
	int i = 0;
	int ii;
	DoctorIndex* New;//��Ҫ������½ڵ�
	DoctorIndex* temp;//һ���н�
	temp = DoctorIndexhead;//temp��ʼָ���ͷ
	if ((fp = fopen("Database\\DoctorMessage.txt", "r")) == NULL)
	{
		printf("cannot open the DoctorMessage file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		New = (DoctorIndex*)malloc(sizeof(DoctorIndex));
		for (ii = 0; ii < MAXNUMI; ii++)
		{
			New->index[ii] = NULL;
		}
		memset(New->flag, 0, sizeof(New->flag));
		New->DoctorIndexTOP = 0;
		fscanf(fp, "%s %d %d", New->name, &New->DoctorTime, &New->DoctorLevel);
		New->ID = i;
		i++;
		New->next = NULL;
		temp->next = New;
		temp = temp->next;
	}
	fclose(fp);
}
void Preload3()//Ԥ������������
{
	FILE* fp;
	int i = 0;
	int ii;
	char name[20];//��������
	DepartmentIndex* New;//��Ҫ������½ڵ�
	DepartmentIndex* temp;//һ���н�
	temp = DepartmentIndexhead;//temp��ʼָ���ͷ
	if ((fp = fopen("Database\\Department.txt", "r")) == NULL)
	{
		printf("cannot open the Department file\n");
		exit(0);
	}
	while (!feof(fp))
	{
		fscanf(fp, "%s", name);
		New = (DepartmentIndex*)malloc(sizeof(DepartmentIndex));
		New->DepartmentIndexTOP = 0;
		for (ii = 0; ii < MAXNUMI; ii++)
		{
			New->index[ii] = NULL;
		}
		New->ID = i;
		New->next = NULL;
		temp->next = New;
		temp = temp->next;
		i++;
	}
	fclose(fp);
}



int judgecheckcode(int a) {
	int i;
	while (a != 0) {
		i = a % 10;
		if (i != 1 && i != 2 && i != 3) {
			return 0;
		}
		a /= 10;
	}
	return 1;
}//�ж�checkcode�����Ƿ�Ϸ�
Record* AddRecord_Hand(Record* Recordhead)//�������Ƽ�¼�ڵ㣨�ֶ���ӣ�
{
	int MOD, checkcode;
	int ii;
	char huancun[100];
	Record* New;//��Ҫ������½ڵ�
	Record* temp;//һ���н�
	temp = Recordhead;
	New = (Record*)malloc(sizeof(Record));
	New->Departmentptr = NULL;
	New->Doctorptr = NULL;
	New->Patientptr = NULL;
	New->situation.checktotal = 0;
	New->situation.checksum = 0;
	New->situation.drugtotal = 0;
	New->situation.drugsum = 0;
	memset(New->situation.check, 0, sizeof(New->situation.check));
	memset(New->situation.drug, 0, sizeof(New->situation.drug));
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;

	printf("\n\t�����뻼������:");
	while (scanf("%s", New->patient.name) != 1||judgechinese(New->patient.name)==0 ){
		printf("\t���������������������룺");
		}
	printf("\n\t�����뻼������:");
	while (scanf("%d", &New->patient.age) != 1 || New->patient.age <= 0 || New->patient.age >= 120) {
		
		printf("\t���������������������룺");
	}
	printf("\n\t������ҽ������:");
	gets(huancun);
	while (scanf("%s", New->doctor.name) != 1||judgechinese(New->doctor.name)==0) {
		printf("\t���������������������룺");
	}
	printf("\n\t|_______________________________________________________________________________________________|\t\n\n");
	for (int i = 0; i < Departmentnumber; i++)
	{
		printf("\t%d.%s   ", i, Department[i]);
		if (i % 5 == 0 && i != 0)
		{
			printf("\n\n");
		}
	}
	printf("\n\n\t|_______________________________________________________________________________________________|\t\n");

	printf("\n\t��ѡ���߾���Ŀ���:");
	while (scanf("%d", &New->doctor.department) != 1 || New->doctor.department<0 || New->doctor.department>Departmentnumber-1) {
		printf("\t���������������������룺");
	}
	printf("\n\t���뻼�߾����ʱ��:\n");
	printf("\n\t��:");
	while (scanf("%d", &New->situation.month) != 1 || New->situation.month < 1 || New->situation.month>12) 
		{
			printf("\t���������������������룺");
		}

	printf("\n\t��:");
	while (scanf("%d", &New->situation.day) != 1 || New->situation.day < 0 || New->situation.day>31) {
		printf("\t���������������������룺");
	}
	printf("\n\tʱ:");
	while(scanf("%d", &New->situation.hour)!=1||New->situation.hour>23||New->situation.hour<0) {
		printf("\t���������������������룺");
	}
	printf("\n\t��:");
	while(scanf("%d", &New->situation.minute)!=1||New->situation.minute>60||New->situation.minute<0) {
		printf("\t���������������������룺");
	}
	printf("\n\t|_______________________________________________________________________________________________|\t\n");
	printf("\n\t1.���         2.��ҩ       3.סԺ\n");
	printf("\n\t|_______________________________________________________________________________________________|\t\n");
	printf("\n\t��ѡ���ߵ���������:");

	while(scanf("%d", &New->situation.CheckCode)!=1||judgecheckcode(New->situation.CheckCode)==0) {
		printf("\t���������������������룺");
	}
	if (Flag[DayDiff(New->situation.month, New->situation.day)] == 500)
		return Recordhead;//����ĺ��Ѿ��������ˣ���������
	while (temp->next != NULL)//�ҵ�������¼�ò����Ǹ��ڵ����
	{
		temp = temp->next;
		if (temp->next == NULL)
			break;
		if (((temp->situation.month <= New->situation.month) && (temp->situation.day <= New->situation.day) && (temp->situation.hour <= New->situation.hour) && (temp->situation.minute <= New->situation.minute)) && ((temp->next->situation.month >= New->situation.month) && (temp->next->situation.day >= New->situation.day) && (temp->next->situation.hour >= New->situation.hour) && (temp->next->situation.minute >= New->situation.minute)))
			break;
	}
	FindDoctor = DoctorIndexhead;
	while (FindDoctor->next != NULL)
	{
		FindDoctor = FindDoctor->next;
		if (strcmp(FindDoctor->name, New->doctor.name) == 0)
		{
			if (FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)] == 20)
				return Recordhead;//����ĺ��Ѿ��������ˣ���������
			New->doctor.level = FindDoctor->DoctorLevel;
			FindDoctor->index[FindDoctor->DoctorIndexTOP] = New;
			New->Doctorptr = &FindDoctor->index[FindDoctor->DoctorIndexTOP];
			FindDoctor->DoctorIndexTOP++;
			FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)]++;
			New->doctor.ID = FindDoctor->ID;
			break;
		}
	}
	FindDepartment = DepartmentIndexhead;
	while (FindDepartment->next != NULL)
	{
		FindDepartment = FindDepartment->next;
		if (FindDepartment->ID == New->doctor.department)
		{
			FindDepartment->index[FindDepartment->DepartmentIndexTOP] = New;
			New->Departmentptr = &FindDepartment->index[FindDepartment->DepartmentIndexTOP];
			FindDepartment->DepartmentIndexTOP++;
			break;
		}
	}
	FindPatient = PatientIndexhead;
	while (FindPatient->next != NULL)
	{
		FindPatient = FindPatient->next;
		if (strcmp(FindPatient->name, New->patient.name) == 0)
		{
			if (FindPatient->flag[DayDiff(New->situation.month, New->situation.day)] == 5)
				return Recordhead;//����ĺ��Ѿ��������ˣ���������
			FindPatient->index[FindPatient->PatientIndexTOP] = New;
			New->Patientptr = &FindPatient->index[FindPatient->PatientIndexTOP];
			FindPatient->PatientIndexTOP++;
			FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
			break;
		}
	}
	if (FindPatient->next == NULL)//�����л����������Ҳ������˻����ǿ����������½ڵ�
	{
		PatientIndex* newflag;
		newflag = (PatientIndex*)malloc(sizeof(PatientIndex));
		newflag->times2 = 0;
		newflag->deposit = 0;
		newflag->PatientIndexTOP = 0;
		memset(newflag->flag, 0, sizeof(newflag->flag));
		for (ii = 0; ii < MAXNUMI; ii++)
		{
			newflag->index[ii] = NULL;
		}
		strcpy(newflag->name, New->patient.name);
		newflag->next = FindPatient->next;
		FindPatient->next = newflag;
		FindPatient = FindPatient->next;
		FindPatient->index[FindPatient->PatientIndexTOP] = New;
		New->Patientptr = &FindPatient->index[FindPatient->PatientIndexTOP];
		FindPatient->PatientIndexTOP++;
		FindPatient->ID = TotalPatientID;
		TotalPatientID++;
	}
	checkcode = New->situation.CheckCode;
	checkcode = reverse(checkcode);//��check����
	while (checkcode > 0)
	{
		MOD = checkcode % 10;
		switch (MOD)
		{
		case 1: Examinationfun_Hand(New); break;//1-���
		case 2: Prescribing_medicinefun_Hand(New); break;//2-��ҩ
		case 3: hospitalizationfun_Hand(FindPatient); break;//3-סԺ
		}
		checkcode /= 10;
	}
	New->next = temp->next;
	temp->next = New;
	Flag[DayDiff(New->situation.month, New->situation.day)]++;
	New->patient.ID = FindPatient->ID;
	TotalRecord++;
}


void AddRecord_Auto(int NUM)//�������Ƽ�¼�ڵ㣨�Զ����n����
{
	int i;
	int ii;
	int MOD, checkcode;
	Record* New;//��Ҫ������½ڵ�
	Record* temp;//һ���н�
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	temp = Recordhead;
	New = (Record*)malloc(sizeof(Record));
	for (i = 0; i < NUM; i++)//���ļ��ж���NUM����¼
	{
		temp = Recordhead;
		New = (Record*)malloc(sizeof(Record));
		New->Departmentptr = NULL;
		New->Doctorptr = NULL;
		New->Patientptr = NULL;
		New->situation.checktotal = 0;
		New->situation.checksum = 0;
		New->situation.drugsum = 0;
		New->situation.drugtotal = 0;
		memset(New->situation.check, 0, sizeof(New->situation.check));
		memset(New->situation.drug, 0, sizeof(New->situation.drug));
		fscanf(nfp, "%s %d %d %s %d %d-%d-%d-%d %d", New->patient.name, &New->patient.age, &New->patient.Register, New->doctor.name, &New->doctor.department, &New->situation.month, &New->situation.day, &New->situation.hour, &New->situation.minute, &New->situation.CheckCode);
		if (Flag[DayDiff(New->situation.month, New->situation.day)] >= 500)
			continue;//����ĺ��Ѿ��������ˣ���������
		while (temp->next != NULL)//�ҵ�������¼�ò����Ǹ��ڵ����
		{
			temp = temp->next;
			if (temp->next == NULL) break;
			if (((temp->situation.month <= New->situation.month) && (temp->situation.day <= New->situation.day) && (temp->situation.hour <= New->situation.hour) && (temp->situation.minute <= New->situation.minute)) && ((temp->next->situation.month >= New->situation.month) && (temp->next->situation.day >= New->situation.day) && (temp->next->situation.hour >= New->situation.hour) && (temp->next->situation.minute >= New->situation.minute)))
				break;
		}
		FindDoctor = DoctorIndexhead;
		while (FindDoctor->next != NULL)
		{
			FindDoctor = FindDoctor->next;
			if (strcmp(FindDoctor->name, New->doctor.name) == 0)
			{
				if (FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)] == 20)
				{
				}
			};//����ĺ��Ѿ��������ˣ���������
			New->doctor.level = FindDoctor->DoctorLevel;
			FindDoctor->index[FindDoctor->DoctorIndexTOP] = New;
			New->Doctorptr = &FindDoctor->index[FindDoctor->DoctorIndexTOP];
			FindDoctor->DoctorIndexTOP++;
			FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)]++;
			New->doctor.ID = FindDoctor->ID;
			break;
		}
	}
	FindDepartment = DepartmentIndexhead;
	while (FindDepartment->next != NULL)
	{
		FindDepartment = FindDepartment->next;
		if (FindDepartment->ID == New->doctor.department)
		{
			FindDepartment->index[FindDepartment->DepartmentIndexTOP] = New;
			New->Departmentptr = &FindDepartment->index[FindDepartment->DepartmentIndexTOP];
			FindDepartment->DepartmentIndexTOP++;
			break;
		}
	}
	FindPatient = PatientIndexhead;
	while (FindPatient->next != NULL)
	{
		FindPatient = FindPatient->next;
		if (strcmp(FindPatient->name, New->patient.name) == 0)
		{
			if (FindPatient->flag[DayDiff(New->situation.month, New->situation.day)] == 5)
			{
			}
		};//����ĺ��Ѿ��������ˣ���������
		FindPatient->index[FindPatient->PatientIndexTOP] = New;
		New->Patientptr = &FindPatient->index[FindPatient->PatientIndexTOP];
		FindPatient->PatientIndexTOP++;
		FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
		break;
	}

	if (FindPatient->next == NULL)//�����л����������Ҳ������˻����ǿ����������½ڵ�
	{
		PatientIndex* newflag;
		newflag = (PatientIndex*)malloc(sizeof(PatientIndex));
		newflag->times2 = 0;
		newflag->deposit = 0;
		memset(newflag->flag, 0, sizeof(newflag->flag));
		for (ii = 0; ii < MAXNUMI; ii++)
		{
			newflag->index[ii] = NULL;
		}
		strcpy(newflag->name, New->patient.name);
		newflag->next = FindPatient->next;
		FindPatient->next = newflag;
		FindPatient = FindPatient->next;
		FindPatient->index[FindPatient->PatientIndexTOP] = New;
		New->Patientptr = &FindPatient->index[FindPatient->PatientIndexTOP];
		FindPatient->PatientIndexTOP++;
		FindPatient->ID = TotalPatientID;
		TotalPatientID++;
	}
	checkcode = New->situation.CheckCode;
	checkcode = reverse(checkcode);//��check����
	while (checkcode > 0)
	{
		MOD = checkcode % 10;
		switch (MOD)
		{
		case 1: Examinationfun_Auto(New); break;//1-���
		case 2: Prescribing_medicinefun_Auto(New); break;//2-��ҩ
		case 3: hospitalizationfun_Auto(FindPatient); break;//3-סԺ
		}
		checkcode /= 10;
	}
	New->next = temp->next;
	temp->next = New;
	Flag[DayDiff(New->situation.month, New->situation.day)]++;
	New->patient.ID = FindPatient->ID;
	TotalRecord++;
}



void ChangeRecord(char name[], int month, int day, int hour, int minute, Record* Recordhead)//����һ�������¼����Ҫ��Ϣ����λ��������ȱʡ������
{
	Record* flag;			//����ָ�루Ҫ�޸ĵ���һ���ڵ㣩
	flag = Recordhead;		//��ʼ��ָ���ͷ
	while (flag->next != NULL)
	{
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))
		{
			//���������¼��������Ϣ�����û�ѡ��ѡ��ʲô��ʲô
		}
		flag = flag->next;
	}
}
void DeleteRecord(char name[], int month, int day, int hour, int minute, Record* Recordhead)//ɾ��һ�������¼����Ҫ��Ϣ����λ��������ȱʡ������
{
	int right = 0;			//0�����޴���
	Record* flag;			//����ָ�루Ҫɾ������һ���ڵ㣩
	Record* flagbefore;		//����ָ���ǰ���ڵ�
	flag = Recordhead->next;	//��ʼ��ָ���������һ��
	flagbefore = Recordhead;	//��ʼ��ָ���ͷ
	while (flag != NULL)
	{
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))
		{
			*(flag->Doctorptr) = NULL;
			flag->Doctorptr = NULL;
			*(flag->Patientptr) = NULL;
			flag->Patientptr = NULL;
			*(flag->Departmentptr) = NULL;
			flag->Departmentptr = NULL;
			flag = flag->next;
			free(flagbefore->next);
			flagbefore->next = flag;
			right = 1;
			break;
		}
		flag = flag->next;
		flagbefore = flagbefore->next;
	}
	if (!right) printf("���޴���\n");
}
void CheckTimeFrame(int startmonth, int startday, int endmonth, int endday, Record* Recordhead)//��ѯĳ��ʱ�䷶Χ�ڵľ����¼
{
	int checkcode, MOD;
	Record* flag;//����ָ��
	flag = Recordhead->next;//��ʼ��ָ���ͷ
	while (flag != NULL)
	{
		checkcode = flag->situation.CheckCode;
		if ((startmonth <= flag->situation.month) && (startday <= flag->situation.day) && (flag->situation.month <= endmonth) && (flag->situation.day <= endday))//��ʱ�䷶Χ��
		{
			printf("����������%s �������䣺%d ҽ��������%s ������ң�%s ҽ������%s �������ڣ�%d��%d��%dʱ%d��\n", flag->patient.name, flag->patient.age, flag->doctor.name, Department[flag->doctor.department], Doctorlevel[flag->doctor.level], flag->situation.month, flag->situation.day, flag->situation.hour, flag->situation.minute);
			checkcode = reverse(checkcode);//��check����
			while (checkcode > 0)
			{
				MOD = checkcode % 10;
				switch (MOD)
				{
				case 1: printf("����ܼۣ�%.2f\n", flag->situation.checksum); break;				//1-���
				case 2: printf("ҩƷ�ܼۣ�%.2f\n", flag->situation.drugsum); break;		//2-��ҩ
////////////////////case 3: printf("����Ѻ��%d ��Ժ���ڣ�2019-%d-%d ��Ժ���ڣ�2019-%d-%d",); break;	//3-סԺ
						//��β�������˵�סԺ��Ϣ��������������
				}
				checkcode /= 10;
			}
		}
		flag = flag->next;
		printf("\n");
	}
}
void PrintDoctor(char name[])
{
	int i;
	DoctorIndex* flag;
	flag = DoctorIndexhead;
	while (flag->next != NULL)
	{
		flag = flag->next;
		if (strcmp(name, flag->name) == 0)
		{
			for (i = 0; i < MAXNUMI; i++)
			{
				if (flag->index[i] != NULL)
					;//������ڸ�ҽ�������Ƽ�¼
			}
			break;//��ҽ���������˲��ؼ���������
		}
	}
}
void PrintDepartment(int Code)
{
	int i;
	DepartmentIndex* flag;
	flag = DepartmentIndexhead;
	while (flag->next != NULL)
	{
		flag = flag->next;
		if (Code == flag->ID)
		{
			for (i = 0; i < MAXNUMI; i++)
			{
				if (flag->index[i] != NULL)
					;//������ڸ����ҵ����Ƽ�¼
			}
			break; //�����ҵ������˲��ؼ���������
		}
	}
}
//void PrintPatient(char name[])
//{
//	int i;
//	PatientIndex* flag;
//	flag = PatientIndexhead;
//	while (flag->next != NULL)
//	{
//		flag = flag->next;
//		if (strcmp(name, flag->name) == 0)
//		{
//			for (i = 0; i < MAXNUMI; i++)
//			{
//				if (flag->index[i] != NULL)
//					;//������ڸû��ߵ����Ƽ�¼
//			}
//			break;//�û��ߵ������˲��ؼ���������
//		}
//
//	}
//}
void StatisticsBusy()
{
	float max = -1, min = 10;//��ʼ�������ֵ��С����Сֵ���
	float ratio;
	char maxname[30], minname[30];
	DoctorIndex* flag;
	flag = DoctorIndexhead;
	while (flag->next != NULL)
	{
		flag = flag->next;
		ratio = flag->DoctorIndexTOP / TotalRecord;
		if (ratio > max)
		{
			max = ratio;
			strcpy(maxname, flag->name);
		}

		if (ratio < min)
		{
			min = ratio;
			strcpy(minname, flag->name);
		}
		printf("%sҽ���ķ�æ��Ϊ��%.2f\n", flag->name, ratio);
	}
	printf("��æ��ҽ���ǣ�%s���䷱æ��Ϊ��%.2f\n", maxname, max);
	printf("���е�ҽ���ǣ�%s���䷱æ��Ϊ��%.2f\n", minname, min);
}
int gettime()//ʱ���ú���
{

	int year = 1900 + p->tm_year;
	int month = 1 + p->tm_mon;
	int day = p->tm_mday;
	int hour = p->tm_hour;
	int minute = p->tm_min;
	int second = p->tm_sec;
	int time = month * 1000000 + day * 10000 + hour * 100 + minute;
	return time;

}
int checkdate(struct PatientIndex* i)//��⻼���Ƿ���Ҫ�ɷѵĺ���
{
	if (i->inmonth && i->outmonth == 0) {
		return 0;
	}
	int a1 = gettime();
	int a2 = a1 / 10000;
	int c = a2 / 100;
	int d = a2 % 100;
	if (c > i->outmonth || (c == i->outmonth && d > i->outday)) {
		i->inmonth = i->outmonth = i->inday = i->outday = 0;
		return 0;
	}
	int k = count(i->inmonth, i->inday, c, d);
	if (k > i->times2) {
		int temp = i->times2;
		i->times2 = k;
		return k - temp;
	}
	return 0;


}



float getturnover(PatientIndex* i)//(Float)�����۳�Ѻ���Ӫҵ��鿴����
{


	if ((gettime() % 10000) / 100 >= 8) {

		while (i != NULL)
		{
			turnover += 200 * checkdate(i);
			i->deposit -= 200 * checkdate(i);
			if (i->deposit < 1000)
			{
				printf("\n\t%d�Ż���%sסԺѺ���Ѳ���1000Ԫ����֪ͨ����סԺѺ��!", i->ID, i->name);
			}
			i = i->next;

		}
		return turnover;
	}
	else { return turnover; }

}
float printturnover(float i) {
	int k = (int)i * 100;
	int a = k / 100;
	int b = (k / 10) % 10;
	int c = k % 10;
	printf("\n\tҽԺ��Ӫҵ��Ϊ:%dԪ%d��%d��", a, b, c);
	return 0;
}
int PrintInPatient(PatientIndex* i)//��ӡסԺ���߱�
{

	printf("\n\t|_________________________________________סԺ���߱�__________________________________________________|\t\n\n");
	int k = 0;
	while (i != NULL) {
		if (i->inmonth != 0 && i->outmonth != 0) {
			printf("\t%d�Ż���:%s        ", i->ID, i->name);
			if (k % 5 == 0 && k != 0) {
				printf("\n");
			}
			k++;
		}
		i = i->next;

	}
	printf("\n\t|_____________________________________________________________________________________________________|\t\n\n");

	return 0;
}
void Save()//�ѵ�ǰϵͳ������Ϣ����
{
	;//�������Ƽ�¼
	;//������־
}

int ChangeDate(int y, int m, int d)//������ת�������ڣ������жϸ�ҽ���Ƿ������Ƽ�¼�Ƿ�����
{
	if (m == 1 || m == 2)
	{
		m += 12;
		y--;
	}
	int iWeek = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
	switch (iWeek)
	{
	case 0: return 1; break;
	case 1: return 2; break;
	case 2: return 3; break;
	case 3: return 4; break;
	case 4: return 5; break;
	case 5: return 6; break;
	case 6: return 7; break;
	}
}

int main()
{
	char huancun[100];
	FILE* turnoverfp;
	time(&t);
	p = localtime(&t);//��õ��ص�ʱ��


	if ((nfp = fopen("test.txt", "r")) == NULL)//�򿪲�������
	{
		printf("cannot open the test file\n");
		exit(0);
	}
	/*=========================================================================*///�����ĸ������ͷ
	Recordhead = (Record*)malloc(sizeof(Record));
	Recordhead->next = NULL;
	PatientIndexhead = (PatientIndex*)malloc(sizeof(PatientIndex));
	PatientIndexhead->next = NULL;
	DoctorIndexhead = (DoctorIndex*)malloc(sizeof(DoctorIndex));
	DoctorIndexhead->next = NULL;
	DepartmentIndexhead = (DepartmentIndex*)malloc(sizeof(DepartmentIndex));
	DepartmentIndexhead->next = NULL;
	/*=========================================================================*/
	Preload1();																	//Ԥ������Դ��
	Preload2();																	//Ԥ����ҽ������
	Preload3();
	/*=========================================================================*/
	if (fopen("Database\\turnover.txt", "r+") != NULL)                                    //¼��Ӫҵ��
	{
		turnoverfp = fopen("Database\\turnover.txt", "r+");
		fscanf(turnoverfp, "%f", &turnover);
	}
	else
	{
		printf("Ӫҵ���ļ��޷�������");
		exit(0);
	}


	/*=========================================================================*/
	system("color F4");                                             //�趨��������
//0 = ��ɫ 8 = ��ɫ��������ɫ����
//1 = ��ɫ 9 = ����ɫ
//2 = ��ɫ A = ����ɫ
//3 = ����ɫ B = ������ɫ
//4 = ��ɫ C = ����ɫ
//5 = ��ɫ D = ����ɫ
//6 = ��ɫ E = ����ɫ
//7 = ��ɫ F = ����ɫ ��һλΪ����ɫ �ڶ�λΪ����ɫ
	/*=========================================================================*/
	while (1)
	{



		printf(

			"\n\t|_________________________���Ƽ�¼����ϵͳ________________________________________________|\t\n"
			"\t|1:¼�����Ƽ�¼                                                                           |\t\n"
			"\t|2:���ļ��е������Ƽ�¼                                                                   |\t\n"
			"\t|3:��ϵͳ��ɾ�����Ƽ�¼                                                                   |\t\n"
			"\t|4:�鿴����������Ϣ                                                                       |\t\n"
			"\t|5:��ѯҽ��������Ϣ                                                                       |\t\n"
			"\t|6:�鿴Ӫҵ��                                                                             |\t\n"
			"\t|7:�鿴סԺ����                                                                           |\t\n"
			"\t|8:�鿴ҽ���������                                                                       |\t\n"
			"\t|9:�˳�����                                                                               |\t\n"
			"\t|____________________________________%d-%d-%d %d:%d:%d___________________________________|\t\n\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec
		);
		int func;
		printf("\t��������Ҫ���еĲ�����");
		while (scanf("%d", &func) != 1 || func < 1 || func>9)
		{
			printf("\n\t���������������������룺");

		}
		gets(huancun);
		switch (func)
		{
		case 1:
			AddRecord_Hand(Recordhead);
		case 2:
			int m;
			printf("\n\t����������¼�����Ƽ�¼������:");
			while(scanf("%d", &m)!=1||m<0) {
				printf("\n\t���������������������룺");

			}
			AddRecord_Auto(m);

		case 3:

		case 4:
		case 5:
		case 6:printturnover(getturnover(PatientIndexhead));
		case 7:PrintInPatient(PatientIndexhead);
		case 8:
		case 9:
			//
			//�����ļ�����������

			//
			//�ر��ļ�

						//�˴�Ӧ���ٸ���case 8�Ĵ���

			fclose(turnoverfp);

		}

	}




	Save();//������ζ�Ҫִ��һ��Save()
	fclose(nfp);
	return 0;
}
