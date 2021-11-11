#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <windows.h> 
#define MAXNUMI 900
/*==================================================================================*///�ṹ����ӻ�
typedef struct Patient					//������Ϣ
{
	char name[30];						//�������� 
	int age;							//���� 
	int Register;						//����ĹҺţ���ֵ������500
	int ID;								//����ID��˫�򱸷ݣ�
}Patient;
typedef struct Doctor					//ҽ����Ϣ
{
	char name[30];						//ҽ������
	int department;						//����
	int level;							//��ҽ���ȼ�
	int ID;								//ҽ��ID��˫�򱸷ݣ�
}Doctor;
typedef struct Situation				//�������
{
	int month, day, hour, minute;		//����ʱ�� 
	int CheckCode;						//������Щ���
	int checktotal;						//����������� 
	int check[30];						//������࣬�����ʮ��
	float checksum;						//����ܼ�
	int drugtotal;						//ҩƷ�������� 
	int drug[30];						//ҩƷ���࣬�����ʮ��
	float drugsum;						//ҩƷ�ܼ�
}Situation;
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
}Record;
typedef struct PatientIndex 			//���߼�������ڵ�
{
	char name[30];						//�������� 
	int ID;								//����ID
	int times2;							//סԺ֮��Ŀ۷Ѵ���
	int inmonth;						//סԺ��ʼ��
	int inday;							//סԺ��ʼ��
	int outmonth;						//��Ժ��
	int outday;							//��Ժ��
	int deposit;						//Ѻ�𣬳��ν��ɲ�������סԺ����*200 
	int flag[368]; 						//�û�������һ����ÿһ��ҵĺ����� 
	int PatientIndexTOP;				//���߼�������ջ����� 
	struct Record* index[900];
	struct PatientIndex* next;
}PatientIndex;
typedef struct DoctorIndex				//ҽ����������ڵ�
{
	char name[20];						//ҽ������
	int ID;								//ҽ��ID 
	int DoctorTime;						//����ʱ��
	int DoctorLevel;					//ҽ���ȼ� 
	int flag[366];						//��ҽ������һ����ÿһ�챻�ҵĺ����� 
	int DoctorIndexTOP;					//ҽ����������ջ����� 
	struct Record* index[900];
	struct DoctorIndex* next;
}DoctorIndex;
typedef struct DepartmentIndex			//���Ҽ�������ڵ�
{
	int ID;								//�������ƣ���ţ�
	int DepartmentIndexTOP;				//���Ҽ�������ջ�����
	struct Record* index[900];
	struct DepartmentIndex* next;
}DepartmentIndex;
/*==================================================================================*/

char PatientName[40][20];				//���л��ߵ����� 
char Department[33][20];				//�������� 
char Doctorlevel[15][16];				//ҽ������
char Examination[30][30];				//�����Ŀ����
char DrugsName[40][30];					//ҩƷ��
int Flag[368] = { 0 };					//��ҽԺ����һ����ÿһ�챻�ҵĺ�����
int TotalPatientID = 0;					//���߱���н����
int TotalRecord = 0;					//��ҽԺϵͳ�������������������ͳ��ҽ����æ�̶ȣ�
float DrugsPrice[40];					//ҩƷ�۸� 
double Turnover;						//ҽԺӪҵ�ܶÿ��ִ��ͳ�ƹ���֮ǰӦ��ֵΪ0��
Record* Recordhead;						//���Ƽ�¼����ͷ��� 
PatientIndex* PatientIndexhead;			//���߼�������ͷ��� 
DoctorIndex* DoctorIndexhead;			//ҽ����������ͷ��� 
DepartmentIndex* DepartmentIndexhead;	//���Ҽ�������ͷ��� 
FILE* nfp;								//test�ļ�ָ�루����ʼʱ�򿪣�����ʱ�رգ�

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
	int day_start =1;
	int m1, d1;
	int m2, d2;
	m1 = (month_start + 9) % 12;
	d1 = (m1 * 306 + 5) / 10 + (day_start - 1);
	m2 = (month_end + 9) % 12;
	d2 = (m2 * 306 + 5) / 10 + (day_end - 1);
	return (d2 - d1);
}
void Examinationfun_Hand(Record* p)					//��飨�ֶ���ӣ� 
{
	int n, temp1;
	float temp2 = 0;
	scanf("%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		scanf("%d %f", &temp1, &temp2);//��������Ŀ
		p->situation.check[temp1] = 1;//��Ǹü���� 
		p->situation.checksum += temp2;//���ϼ����Ŀ���� 
		n--;
	}
}
void Examinationfun_Auto(Record* p)					//��飨�Զ���ӣ� 
{
	int n, temp1;
	float temp2 = 0;
	fscanf(nfp,"%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(nfp,"%d %f", &temp1, &temp2);//��������Ŀ
		p->situation.check[temp1] = 1;//��Ǹü���� 
		p->situation.checksum += temp2;//���ϼ����Ŀ���� 
		n--;
	}
}
void Prescribing_medicinefun_Hand(Record* p)		//��ҩ���ֶ���ӣ� 
{
	int n, temp1, temp2;
	scanf("%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		scanf("%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//��Ǹ�ҩƷ���� 
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void Prescribing_medicinefun_Auto(Record* p)		//��ҩ���Զ���ӣ� 
{
	int n, temp1, temp2;
	fscanf(nfp,"%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(nfp,"%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//��Ǹ�ҩƷ���� 
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void hospitalizationfun_Hand(PatientIndex* p)		//סԺ���ֶ���ӣ�
{
	scanf("%d %d-%d %d-%d", &p->deposit,&p->inmonth,&p->inday,&p->outmonth,&p->outday);
}
void hospitalizationfun_Auto(PatientIndex* p)		//סԺ���Զ���ӣ�
{
	fscanf(nfp,"%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
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
		fscanf(fp, "%s",name);
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
void AddRecord_Hand()//�������Ƽ�¼�ڵ㣨�ֶ���ӣ� 
{
	int MOD, checkcode;
	int ii;
	Record* New;//��Ҫ������½ڵ�
	Record* temp;//һ���н�
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	printf("������һ�����Ƽ�¼��\n");
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
	scanf("%s %d %d %s %d %d-%d-%d-%d %d", New->patient.name, &New->patient.age, &New->patient.Register, New->doctor.name, &New->doctor.department, &New->situation.month, &New->situation.day,& New->situation.hour, &New->situation.minute, &New->situation.CheckCode);	
	if (Flag[DayDiff(New->situation.month, New->situation.day)] == 500) 
		return Recordhead;//����ĺ��Ѿ��������ˣ���������
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
		strcpy(newflag->name,New->patient.name);
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
		fscanf(nfp,"%s %d %d %s %d %d-%d-%d-%d %d", New->patient.name, &New->patient.age, &New->patient.Register, New->doctor.name, &New->doctor.department, &New->situation.month, &New->situation.day, &New->situation.hour, &New->situation.minute, &New->situation.CheckCode);
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
	int checkcode,MOD;
	Record* flag;//����ָ��
	flag = Recordhead->next;//��ʼ��ָ���ͷ
	while (flag != NULL)
	{
		checkcode = flag->situation.CheckCode;
		if ((startmonth <= flag->situation.month) && (startday <= flag->situation.day) && (flag->situation.month <= endmonth) && (flag->situation.day <= endday))//��ʱ�䷶Χ�� 
		{
			printf("����������%s �������䣺%d ҽ��������%s ������ң�%s ҽ������%s �������ڣ�%d��%d��%dʱ%d��\n",flag->patient.name,flag->patient.age,flag->doctor.name, Department[flag->doctor.department],Doctorlevel[flag->doctor.level],flag->situation.month,flag->situation.day,flag->situation.hour,flag->situation.minute);
			checkcode = reverse(checkcode);//��check���� 
			while (checkcode > 0)
			{
				MOD = checkcode % 10;
				switch (MOD)
				{
					case 1: printf("����ܼۣ�%.2f\n",flag->situation.checksum); break;				//1-��� 
					case 2: printf("ҩƷ�ܼۣ�%.2f\n",flag->situation.drugsum); break;		//2-��ҩ 
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
void PrintPatient(char name[])
{
	int i;
	PatientIndex* flag;
	flag = PatientIndexhead;
	while (flag->next != NULL)
	{
		flag = flag->next;
		if (strcmp(name, flag->name) == 0)
		{
			for (i = 0; i < MAXNUMI; i++)
			{
				if (flag->index[i] != NULL)
					;//������ڸû��ߵ����Ƽ�¼
			}
			break;//�û��ߵ������˲��ؼ���������
		}

	}
}
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
/*int gettime()//ʱ���ú�������ʱ��Ϊ��ĿҪ���ֻ������ʱ��,���֮ǰ���Ĵ����е�ʱ��ʵ�ֲ���ʹ��
{
	int year=1900+p->tm_year;
	int month=1+p->tm_mon;
	int day=p->tm_mday;
	int hour=p->tm_hour;
	int minute=p->tm_min;
	int second=p->tm_sec;
	int time=month*1000000+day*10000+hour*100+minute;
	return time;
}*/
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
	Preload3();																	//Ԥ������������
	AddRecord_Hand();															//�ֶ�����һ�����Ƽ�¼�ڵ�
	Save();																		//������ζ�Ҫִ��һ��Save()��
	fclose(nfp);																//�ر�test�ļ�ָ��
	return 0;
}