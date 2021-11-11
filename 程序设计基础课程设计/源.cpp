#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <windows.h> 
#define MAXNUMI 900
/*==================================================================================*///�ṹ�塢�����������ӻ�
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
	float check[30];					//������࣬�����ʮ��
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
	struct PatientIndex* IndexPtr;
	struct Record* next;
}Record;
typedef struct Index_Patient
{
	struct Record* RecordPtr;
	struct Index_Patient* next;
}Index_Patient;
typedef struct Index_Doctor
{
	struct Record* RecordPtr;
	struct Index_Doctor* next;
}Index_Doctor;
typedef struct Index_Department
{
	struct Record* RecordPtr;
	struct Index_Department* next;
}Index_Department;
typedef struct PatientIndex 			//���߼�������ڵ�
{
	char name[30];						//�������� 
	int age;							//��������
	int ID;								//����ID
	int times2;							//סԺ֮��Ŀ۷Ѵ���
	int inmonth;						//סԺ��ʼ��
	int inday;							//סԺ��ʼ��
	int outmonth;						//��Ժ��
	int outday;							//��Ժ��
	int deposit;						//Ѻ�𣬳��ν��ɲ�������סԺ����*200 
	int flag[368]; 						//�û�������һ����ÿһ��ҵĺ����� 
	int PatientPtrTotal;				//���߼�������ָ�����
	struct Index_Patient* Index_Patienthead;
	struct PatientIndex* next;
}PatientIndex;
typedef struct DoctorIndex				//ҽ����������ڵ�
{
	char name[20];						//ҽ������
	int ID;								//ҽ��ID 
	int DoctorTime;						//����ʱ��
	int DoctorLevel;					//ҽ���ȼ� 
	int flag[366];						//��ҽ������һ����ÿһ�챻�ҵĺ����� 
	int DoctorPtrTotal;					//ҽ����������ָ�����
	int Job_number;						//ҽ������
	struct Index_Doctor* Index_Doctorhead;
	struct DoctorIndex* next;
}DoctorIndex;
typedef struct DepartmentIndex			//���Ҽ�������ڵ�
{
	int ID;								//�������ƣ���ţ�
	int DepartmentPtrTotal;				//���Ҽ�������ָ�����
	struct Index_Department* Index_Departmenthead;
	struct DepartmentIndex* next;
}DepartmentIndex;
int reverse(int number);
int DayDiff(int month_end, int day_end);
int ChangeDate(int m, int d);
int gettime();
int judgeu(Record* p1, Record* p2, Record* p3);
int judgek(int month1, int day1, int hour1, int minute1, int month2, int day2, int hour2, int minute2);
void Examinationfun_Hand(Record* p);
void Examinationfun_Auto(Record* p);
void Examinationfun_Local(Record* p);
void PrintExamination(Record* p);
void Prescribing_medicinefun_Hand(Record* p);
void Prescribing_medicinefun_Auto(Record* p);
void Prescribing_medicinefun_Local(Record* p);
void PrintPrescribing_medicine(Record* p);
void hospitalizationfun_Hand(PatientIndex* p);
void hospitalizationfun_Auto(PatientIndex* p);
void hospitalizationfun_Local(PatientIndex* p);
void Printhospitalization(Record* p);
void Preload1();
void Preload2();
void Preload3();
void Preload4();
void AddRecord_Hand();
void AddRecord_Auto(int NUM);
void ChangeRecord(char name[], int month, int day, int hour, int minute);
void DeleteRecord(char name[], int month, int day, int hour, int minute);
void CheckTimeFrame(int startmonth, int startday, int endmonth, int endday);
void PrintDoctor(int JobNumber);
void PrintDepartment(int Code);
void PrintPatient(char name[]);
void StatisticsBusy(int month, int day);
void Save();
/*==================================================================================*/

time_t t;								//��t����Ϊʱ��������趨ʱ��
struct tm* p;							//struct tm��һ���ṹ�壬����һ���ṹ��ָ��
char PatientName[40][20];				//���л��ߵ����� 
char Department[33][20];				//�������� 
char Doctorlevel[15][16];				//ҽ������
char Examination[30][30];				//�����Ŀ����
char DrugsName[40][30];					//ҩƷ��
int Flag[368] = { 0 };					//��ҽԺ����һ����ÿһ�챻�ҵĺ�����
int TotalPatientID = 0;					//���߱���н����
float DrugsPrice[40];					//ҩƷ�۸� 
double Turnover;						//ҽԺӪҵ�ܶÿ��ִ��ͳ�ƹ���֮ǰӦ��ֵΪ0��
Record* Recordhead;						//���Ƽ�¼����ͷ��� 
PatientIndex* PatientIndexhead;			//���߼�������ͷ��� 
DoctorIndex* DoctorIndexhead;			//ҽ����������ͷ��� 
DepartmentIndex* DepartmentIndexhead;	//���Ҽ�������ͷ��� 
FILE* testfp;							//test�ļ�ָ�루����ʼʱ�򿪣�����ʱ�رգ�
FILE* systemlogfp;						//ϵͳ������־�ļ�ָ�루����ʼʱ�򿪣�����ʱ�رգ�
FILE* localdatafp;						//�����ļ�ָ�루Ԥ���ش򿪹رգ�Save�����򿪹رգ�

int judgek(int month1, int day1, int hour1, int minute1, int month2, int day2, int hour2, int minute2)
{
	if (DayDiff(month1, day1) > DayDiff(month2, day2)) return 1;
	else {
		if (DayDiff(month1, day1) < DayDiff(month2, day2))
		{
			return -1;
		}
		else {
			if (hour1 > hour2)
			{
				return 1;
			}
			else {
				if (hour1 < hour2)
				{
					return -1;
				}
				else {
					if (minute1 > minute2) return 1;
					else return -1;
				}
			}
		}
	}
}
int judgeu(Record* p1, Record* p2, Record* p3)
{
	//p1:temp ,p2:NEW,p3:temp->next
	int a = judgek(p1->situation.month, p1->situation.day, p1->situation.hour, p1->situation.minute,
		p2->situation.month, p2->situation.day, p2->situation.hour, p2->situation.minute);
	int b = judgek(p2->situation.month, p2->situation.day, p2->situation.hour, p2->situation.minute,
		p3->situation.month, p3->situation.day, p3->situation.hour, p3->situation.minute);
	if (a < 0 && b < 0)
	{
		return 0;//���м�
	}
	if (a < 0 && b>0)
	{
		return 1;//���ұ� 
	}
	if (a > 0 && b < 0)
	{
		return -1;//�����
	}
}
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
	return 365 + (d2 - d1);
}
int gettime()//ʱ���ú�������ʱ��Ϊ��ĿҪ���ֻ������ʱ��,���֮ǰ���Ĵ����е�ʱ��ʵ�ֲ���ʹ��
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
int ChangeDate(int m, int d)//������ת�������ڣ������жϸ�ҽ���Ƿ������Ƽ�¼�Ƿ����� 
{
	int y = 2019;
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
int CheckWeek(int n, int m)//n�Ƿ���m�г���
{
	while (m > 0)
	{
		if (n == (m % 10)) return 1;//����
		m /= 10;
	}
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
		p->situation.check[temp1] = temp2;//��Ǹü���� 
		p->situation.checksum += temp2;//���ϼ����Ŀ���� 
		n--;
	}
}
void Examinationfun_Auto(Record* p)					//��飨�Զ���ӣ� 
{
	int n, temp1;
	float temp2 = 0;
	fscanf(testfp, "%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(testfp, "%d %f", &temp1, &temp2);//��������Ŀ
		p->situation.check[temp1] = temp2;//��Ǹü���� 
		p->situation.checksum += temp2;//���ϼ����Ŀ���� 
		n--;
	}
}
void Examinationfun_Local(Record* p)				//��飨local��ӣ� 
{
	int n, temp1;
	float temp2 = 0;
	fscanf(localdatafp, "%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(localdatafp, "%d %f", &temp1, &temp2);//��������Ŀ
		p->situation.check[temp1] = temp2;//��Ǹü���� 
		p->situation.checksum += temp2;//���ϼ����Ŀ���� 
		n--;
	}
}
void PrintExamination(Record* p)
{
	int i = 0;
	fprintf(localdatafp, " %d", p->situation.checktotal);
	while (i < 16)
	{
		if (p->situation.check[i] > 0)
		{
			fprintf(localdatafp, " %d %.2f", i, p->situation.check[i]);
		}
		i++;
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
	fscanf(testfp, "%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(testfp, "%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//��Ǹ�ҩƷ���� 
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void Prescribing_medicinefun_Local(Record* p)		//��ҩ��local��ӣ� 
{
	int n, temp1, temp2;
	fscanf(localdatafp, "%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(localdatafp, "%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//��Ǹ�ҩƷ���� 
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void PrintPrescribing_medicine(Record* p)
{
	int i = 0;
	fprintf(localdatafp, " %d", p->situation.drugtotal);
	while (i < 30)
	{
		if (p->situation.drug[i] > 0)
		{
			fprintf(localdatafp, " %d-%d", i, p->situation.drug[i]);
		}
		i++;
	}
}
void hospitalizationfun_Hand(PatientIndex* p)		//סԺ���ֶ���ӣ�
{
	scanf("%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void hospitalizationfun_Auto(PatientIndex* p)		//סԺ���Զ���ӣ�
{
	fscanf(testfp, "%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void hospitalizationfun_Local(PatientIndex* p)		//סԺ��local��ӣ�
{
	fscanf(localdatafp, "%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void Printhospitalization(Record* p)
{
	fprintf(localdatafp, " %d %d-%d %d-%d", p->IndexPtr->deposit, p->IndexPtr->inmonth, p->IndexPtr->inday, p->IndexPtr->outmonth, p->IndexPtr->outday);
}
void Preload1()//Ԥ������Դ�� 
{
	char ch;
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
	char ch;
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
		New->Index_Doctorhead = (Index_Doctor*)malloc(sizeof(Index_Doctor));
		New->Index_Doctorhead->next = NULL;
		memset(New->flag, 0, sizeof(New->flag));
		New->DoctorPtrTotal = 0;
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
	char ch;
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
		New->Index_Departmenthead = (Index_Department*)malloc(sizeof(Index_Department));
		New->Index_Departmenthead->next = NULL;
		New->DepartmentPtrTotal = 0;
		New->ID = i;
		New->next = NULL;
		temp->next = New;
		temp = temp->next;
		i++;
	}
	fclose(fp);
}
void Preload4()//Ԥ����localdata�е�����
{
	char ch;
	if ((localdatafp = fopen("localdata.txt", "r")) == NULL)//�򿪱������ݿ����� 
	{
		printf("cannot open the localdata file\n");
		exit(0);
	}
	int ii;
	int MOD, checkcode;
	Record* New;//��Ҫ������½ڵ�
	Record* temp;//һ���н�
	Record* tempbefore;//�н��ǰ���ڵ�
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	ch = fgetc(localdatafp);
	while (!feof(localdatafp))//��localdata�е�������ȫ����
	{
		temp = Recordhead;
		tempbefore = Recordhead;
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
		fscanf(localdatafp, "%s %d %d %s %d %d-%d-%d-%d %d", New->patient.name, &New->patient.age, &New->patient.Register, New->doctor.name, &New->doctor.department, &New->situation.month, &New->situation.day, &New->situation.hour, &New->situation.minute, &New->situation.CheckCode);
		if (Flag[DayDiff(New->situation.month, New->situation.day)] >= 500)
			continue;//����ĺ��Ѿ��������ˣ���������
		while (temp->next != NULL)//�ҵ�������¼�ò����Ǹ��ڵ����
		{
			temp = temp->next;
			if (judgeu(tempbefore, New, temp) == 0) break;
			tempbefore = tempbefore->next;
		}
		FindDoctor = DoctorIndexhead;
		while (FindDoctor->next != NULL)
		{
			FindDoctor = FindDoctor->next;
			if (strcmp(FindDoctor->name, New->doctor.name) == 0)
			{
				if (FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)] == 20)
					return;//����ĺ��Ѿ��������ˣ���������
				New->doctor.level = FindDoctor->DoctorLevel;
				Index_Doctor* FindTempBefore = FindDoctor->Index_Doctorhead;//FindTemp��ǰ���ڵ�
				Index_Doctor* FindTemp = FindDoctor->Index_Doctorhead;
				Index_Doctor* Temp;
				Temp = (Index_Doctor*)malloc(sizeof(Index_Doctor));
				Temp->RecordPtr = New;
				Temp->next = NULL;
				while (FindTemp->next != NULL)
				{
					FindTemp = FindTemp->next;
					if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
						break;
					FindTempBefore = FindTempBefore->next;
				}
				Temp->next = FindTempBefore->next;
				FindTempBefore->next = Temp;
				New->Doctorptr = &Temp->RecordPtr;
				FindDoctor->DoctorPtrTotal++;
				FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)]++;
				FindDoctor->Job_number = New->doctor.department * 1000 + FindDoctor->ID * 10 + FindDoctor->DoctorLevel;
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
				Index_Department* FindTempBefore = FindDepartment->Index_Departmenthead;//FindTemp��ǰ���ڵ�
				Index_Department* FindTemp = FindDepartment->Index_Departmenthead;
				Index_Department* Temp;
				Temp = (Index_Department*)malloc(sizeof(Index_Department));
				Temp->RecordPtr = New;
				Temp->next = NULL;
				while (FindTemp->next != NULL)
				{
					FindTemp = FindTemp->next;
					if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
						break;
					FindTempBefore = FindTempBefore->next;
				}
				Temp->next = FindTempBefore->next;
				FindTempBefore->next = Temp;
				New->Departmentptr = &Temp->RecordPtr;
				FindDepartment->DepartmentPtrTotal++;
				break;
			}
		}
		FindPatient = PatientIndexhead;
		while (FindPatient->next != NULL)
		{
			FindPatient = FindPatient->next;
			if (strcmp(FindPatient->name, New->patient.name) == 0 && (FindPatient->age == New->patient.age))
			{
				if (FindPatient->flag[DayDiff(New->situation.month, New->situation.day)] == 5)
					return;//����ĺ��Ѿ��������ˣ���������
				Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp��ǰ���ڵ�
				Index_Patient* FindTemp = FindPatient->Index_Patienthead;
				Index_Patient* Temp;
				Temp = (Index_Patient*)malloc(sizeof(Index_Patient));
				Temp->RecordPtr = New;
				Temp->next = NULL;
				while (FindTemp->next != NULL)
				{
					FindTemp = FindTemp->next;
					if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
						break;
					FindTempBefore = FindTempBefore->next;
				}
				Temp->next = FindTempBefore->next;
				FindTempBefore->next = Temp;
				New->Patientptr = &Temp->RecordPtr;
				FindPatient->PatientPtrTotal++;
				FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
				New->IndexPtr = FindPatient;
				break;
			}
		}
		if (FindPatient->next == NULL)//�����л����������Ҳ������˻����ǿ����������½ڵ� 
		{
			PatientIndex* newflag;
			newflag = (PatientIndex*)malloc(sizeof(PatientIndex));
			newflag->times2 = 0;
			newflag->deposit = 0;
			newflag->Index_Patienthead = (Index_Patient*)malloc(sizeof(Index_Patient));
			newflag->Index_Patienthead->next = NULL;
			memset(newflag->flag, 0, sizeof(newflag->flag));
			strcpy(newflag->name, New->patient.name);
			newflag->next = FindPatient->next;
			FindPatient->next = newflag;
			FindPatient = FindPatient->next;
			Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp��ǰ���ڵ�
			Index_Patient* FindTemp = FindPatient->Index_Patienthead;
			Index_Patient* Temp;
			Temp = (Index_Patient*)malloc(sizeof(Index_Patient));
			Temp->RecordPtr = New;
			Temp->next = NULL;
			while (FindTemp->next != NULL)
			{
				FindTemp = FindTemp->next;
				if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
					break;
			}
			Temp->next = FindTempBefore->next;
			FindTempBefore->next = Temp;
			New->Patientptr = &Temp->RecordPtr;
			FindPatient->PatientPtrTotal++;
			FindPatient->ID = TotalPatientID;
			FindPatient->age = New->patient.age;
			TotalPatientID++;
			New->IndexPtr = FindPatient;
		}
		checkcode = New->situation.CheckCode;
		checkcode = reverse(checkcode);//��check���� 
		while (checkcode > 0)
		{
			MOD = checkcode % 10;
			switch (MOD)
			{
			case 1: Examinationfun_Local(New); break;//1-��� 
			case 2: Prescribing_medicinefun_Local(New); break;//2-��ҩ 
			case 3: hospitalizationfun_Local(FindPatient); break;//3-סԺ 
			}
			checkcode /= 10;
		}
		New->next = tempbefore->next;
		tempbefore->next = New;
		Flag[DayDiff(New->situation.month, New->situation.day)]++;
		New->patient.ID = FindPatient->ID;
		ch = fgetc(localdatafp);
	}
	fclose(localdatafp);
}
void AddRecord_Hand()//�������Ƽ�¼�ڵ㣨�ֶ���ӣ� 
{
	int MOD, checkcode;
	int ii;
	Record* New;//��Ҫ������½ڵ�
	Record* temp;//һ���н�
	Record* tempbefore;//�н��ǰ���ڵ�
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	printf("������һ�����Ƽ�¼��\n");
	temp = Recordhead;
	tempbefore = Recordhead;
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
	scanf("%s %d %d %s %d %d-%d-%d-%d %d", New->patient.name, &New->patient.age, &New->patient.Register, New->doctor.name, &New->doctor.department, &New->situation.month, &New->situation.day, &New->situation.hour, &New->situation.minute, &New->situation.CheckCode);
	if (Flag[DayDiff(New->situation.month, New->situation.day)] == 500)
		return;//����ĺ��Ѿ��������ˣ���������
	while (temp->next != NULL)//�ҵ�������¼�ò����Ǹ��ڵ����
	{
		temp = temp->next;
		if (judgeu(tempbefore, New, temp) == 0) break;
		tempbefore = tempbefore->next;
	}
	FindDoctor = DoctorIndexhead;
	while (FindDoctor->next != NULL)
	{
		FindDoctor = FindDoctor->next;
		if (strcmp(FindDoctor->name, New->doctor.name) == 0)
		{
			if (FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)] == 20)
				return;//����ĺ��Ѿ��������ˣ���������
			/*if (!(CheckWeek(ChangeDate(New->situation.month, New->situation.day), FindDoctor->DoctorTime)))
				return;//��ҽ���ڽ��첻����*/
			New->doctor.level = FindDoctor->DoctorLevel;
			Index_Doctor* FindTempBefore = FindDoctor->Index_Doctorhead;//FindTemp��ǰ���ڵ�
			Index_Doctor* FindTemp = FindDoctor->Index_Doctorhead;
			Index_Doctor* Temp;
			Temp = (Index_Doctor*)malloc(sizeof(Index_Doctor));
			Temp->RecordPtr = New;
			Temp->next = NULL;
			while (FindTemp->next != NULL)
			{
				FindTemp = FindTemp->next;
				if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
					break;
				FindTempBefore = FindTempBefore->next;
			}
			Temp->next = FindTempBefore->next;
			FindTempBefore->next = Temp;
			New->Doctorptr = &Temp->RecordPtr;
			FindDoctor->DoctorPtrTotal++;
			FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)]++;
			FindDoctor->Job_number = New->doctor.department * 1000 + FindDoctor->ID * 10 + FindDoctor->DoctorLevel;
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
			Index_Department* FindTempBefore = FindDepartment->Index_Departmenthead;//FindTemp��ǰ���ڵ�
			Index_Department* FindTemp = FindDepartment->Index_Departmenthead;
			Index_Department* Temp;
			Temp = (Index_Department*)malloc(sizeof(Index_Department));
			Temp->RecordPtr = New;
			Temp->next = NULL;
			while (FindTemp->next != NULL)
			{
				FindTemp = FindTemp->next;
				if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
					break;
				FindTempBefore = FindTempBefore->next;
			}
			Temp->next = FindTempBefore->next;
			FindTempBefore->next = Temp;
			New->Departmentptr = &Temp->RecordPtr;
			FindDepartment->DepartmentPtrTotal++;
			break;
		}
	}
	FindPatient = PatientIndexhead;
	while (FindPatient->next != NULL)
	{
		FindPatient = FindPatient->next;
		if (strcmp(FindPatient->name, New->patient.name) == 0 && (FindPatient->age == New->patient.age))
		{
			if (FindPatient->flag[DayDiff(New->situation.month, New->situation.day)] == 5)
				return;//����ĺ��Ѿ��������ˣ���������
			Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp��ǰ���ڵ�
			Index_Patient* FindTemp = FindPatient->Index_Patienthead;
			Index_Patient* Temp;
			Temp = (Index_Patient*)malloc(sizeof(Index_Patient));
			Temp->RecordPtr = New;
			Temp->next = NULL;
			while (FindTemp->next != NULL)
			{
				FindTemp = FindTemp->next;
				if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
					break;
				FindTempBefore = FindTempBefore->next;
			}
			Temp->next = FindTempBefore->next;
			FindTempBefore->next = Temp;
			New->Patientptr = &Temp->RecordPtr;
			FindPatient->PatientPtrTotal++;
			FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
			New->IndexPtr = FindPatient;
			break;
		}
	}
	if (FindPatient->next == NULL)//�����л����������Ҳ������˻����ǿ����������½ڵ� 
	{
		PatientIndex* newflag;
		newflag = (PatientIndex*)malloc(sizeof(PatientIndex));
		newflag->times2 = 0;
		newflag->deposit = 0;
		newflag->PatientPtrTotal = 0;
		newflag->Index_Patienthead = (Index_Patient*)malloc(sizeof(Index_Patient));
		newflag->Index_Patienthead->next = NULL;
		memset(newflag->flag, 0, sizeof(newflag->flag));
		strcpy(newflag->name, New->patient.name);
		newflag->next = FindPatient->next;
		FindPatient->next = newflag;
		FindPatient = FindPatient->next;
		Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp��ǰ���ڵ�
		Index_Patient* FindTemp = FindPatient->Index_Patienthead;
		Index_Patient* Temp;
		Temp = (Index_Patient*)malloc(sizeof(Index_Patient));
		Temp->RecordPtr = New;
		Temp->next = NULL;
		while (FindTemp->next != NULL)
		{
			FindTemp = FindTemp->next;
			if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
				break;
			FindTempBefore = FindTempBefore->next;
		}
		Temp->next = FindTempBefore->next;
		FindTempBefore->next = Temp;
		New->Patientptr = &Temp->RecordPtr;
		FindPatient->PatientPtrTotal++;
		FindPatient->ID = TotalPatientID;
		FindPatient->age = New->patient.age;
		TotalPatientID++;
		New->IndexPtr = FindPatient;
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
	New->next = tempbefore->next;
	tempbefore->next = New;
	Flag[DayDiff(New->situation.month, New->situation.day)]++;
	New->patient.ID = FindPatient->ID;
}
void AddRecord_Auto(int NUM)//�������Ƽ�¼�ڵ㣨�Զ����n���� 
{
	int i;
	int ii;
	int MOD, checkcode;
	Record* New;//��Ҫ������½ڵ�
	Record* temp;//һ���н�
	Record* tempbefore;//�н��ǰ���ڵ�
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	for (i = 0; i < NUM; i++)//���ļ��ж���NUM����¼ 
	{
		temp = Recordhead;
		tempbefore = Recordhead;
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
		fscanf(testfp,"%s %d %d %s %d %d-%d-%d-%d %d", New->patient.name, &New->patient.age, &New->patient.Register, New->doctor.name, &New->doctor.department, &New->situation.month, &New->situation.day, &New->situation.hour, &New->situation.minute, &New->situation.CheckCode);
		if (Flag[DayDiff(New->situation.month, New->situation.day)] >= 500)
			continue;//����ĺ��Ѿ��������ˣ���������
		while (temp->next != NULL)//�ҵ�������¼�ò����Ǹ��ڵ����
		{
			temp = temp->next;
			if (judgeu(tempbefore, New, temp) == 0) break;
			tempbefore = tempbefore->next;
		}
		FindDoctor = DoctorIndexhead;
		while (FindDoctor->next != NULL)
		{
			FindDoctor = FindDoctor->next;
			if (strcmp(FindDoctor->name, New->doctor.name) == 0)
			{
				if (FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)] == 20)
					return;//����ĺ��Ѿ��������ˣ���������
				New->doctor.level = FindDoctor->DoctorLevel;
				Index_Doctor* FindTempBefore = FindDoctor->Index_Doctorhead;//FindTemp��ǰ���ڵ�
				Index_Doctor* FindTemp = FindDoctor->Index_Doctorhead;
				Index_Doctor* Temp;
				Temp = (Index_Doctor*)malloc(sizeof(Index_Doctor));
				Temp->RecordPtr = New;
				Temp->next = NULL;
				while (FindTemp->next != NULL)
				{
					FindTemp = FindTemp->next;
					if (temp->next == NULL) break;
					if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
						break;
					FindTempBefore = FindTempBefore->next;
				}
				Temp->next = FindTempBefore->next;
				FindTempBefore->next = Temp;
				New->Doctorptr = &Temp->RecordPtr;
				FindDoctor->DoctorPtrTotal++;
				FindDoctor->flag[DayDiff(New->situation.month, New->situation.day)]++;
				FindDoctor->Job_number = New->doctor.department * 1000 + FindDoctor->ID * 10 + FindDoctor->DoctorLevel;
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
				Index_Department* FindTempBefore = FindDepartment->Index_Departmenthead;//FindTemp��ǰ���ڵ�
				Index_Department* FindTemp = FindDepartment->Index_Departmenthead;
				Index_Department* Temp;
				Temp = (Index_Department*)malloc(sizeof(Index_Department));
				Temp->RecordPtr = New;
				Temp->next = NULL;
				while (FindTemp->next != NULL)
				{
					FindTemp = FindTemp->next;
					if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
						break;
					FindTempBefore = FindTempBefore->next;
				}
				Temp->next = FindTempBefore->next;
				FindTempBefore->next = Temp;
				New->Departmentptr = &Temp->RecordPtr;
				FindDepartment->DepartmentPtrTotal++;
				break;
			}
		}
		FindPatient = PatientIndexhead;
		while (FindPatient->next != NULL)
		{
			FindPatient = FindPatient->next;
			if (strcmp(FindPatient->name, New->patient.name) == 0 && (FindPatient->age == New->patient.age))
			{
				if (FindPatient->flag[DayDiff(New->situation.month, New->situation.day)] == 5)
					return;//����ĺ��Ѿ��������ˣ���������
				Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp��ǰ���ڵ�
				Index_Patient* FindTemp = FindPatient->Index_Patienthead;
				Index_Patient* Temp;
				Temp = (Index_Patient*)malloc(sizeof(Index_Patient));
				Temp->RecordPtr = New;
				Temp->next = NULL;
				while (FindTemp->next != NULL)
				{
					FindTemp = FindTemp->next;
					if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
						break;
					FindTempBefore = FindTempBefore->next;
				}
				Temp->next = FindTempBefore->next;
				FindTempBefore->next = Temp;
				New->Patientptr = &Temp->RecordPtr;
				FindPatient->PatientPtrTotal++;
				FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
				New->IndexPtr = FindPatient;
				break;
			}
		}
		if (FindPatient->next == NULL)//�����л����������Ҳ������˻����ǿ����������½ڵ� 
		{
			PatientIndex* newflag;
			newflag = (PatientIndex*)malloc(sizeof(PatientIndex));
			newflag->times2 = 0;
			newflag->deposit = 0;
			newflag->Index_Patienthead = (Index_Patient*)malloc(sizeof(Index_Patient));
			newflag->Index_Patienthead->next = NULL;
			memset(newflag->flag, 0, sizeof(newflag->flag));
			strcpy(newflag->name, New->patient.name);
			newflag->next = FindPatient->next;
			FindPatient->next = newflag;
			FindPatient = FindPatient->next;
			Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp��ǰ���ڵ�
			Index_Patient* FindTemp = FindPatient->Index_Patienthead;
			Index_Patient* Temp;
			Temp = (Index_Patient*)malloc(sizeof(Index_Patient));
			Temp->RecordPtr = New;
			Temp->next = NULL;
			while (FindTemp->next != NULL)
			{
				FindTemp = FindTemp->next;
				if (judgek(FindTemp->RecordPtr->situation.month, FindTemp->RecordPtr->situation.day, FindTemp->RecordPtr->situation.hour, FindTemp->RecordPtr->situation.minute, New->situation.month, New->situation.day, New->situation.hour, New->situation.minute) == -1)
					break;
				FindTempBefore = FindTempBefore->next;
			}
			Temp->next = FindTempBefore->next;
			FindTempBefore->next = Temp;
			New->Patientptr = &Temp->RecordPtr;
			FindPatient->PatientPtrTotal++;
			FindPatient->ID = TotalPatientID;
			FindPatient->age = New->patient.age;
			TotalPatientID++;
			New->IndexPtr = FindPatient;
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
		New->next = tempbefore->next;
		tempbefore->next = New;
		Flag[DayDiff(New->situation.month, New->situation.day)]++;
		New->patient.ID = FindPatient->ID;
	}
}
void ChangeRecord(char name[], int month, int day, int hour, int minute)//����һ�������¼����Ҫ��Ϣ����λ��������ȱʡ������ 
{
	Record* flag;			//����ָ�루Ҫ�޸ĵ���һ���ڵ㣩 
	flag = Recordhead;		//��ʼ��ָ���ͷ
	while (flag->next != NULL)
	{
		flag = flag->next;
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))//��λ��������¼
		{
			;//���������¼��������Ϣ
			DeleteRecord(name, month, day, hour, minute);//ɾ������Record
			AddRecord_Hand();//��������һ��Record	
		}
	}
}
void DeleteRecord(char name[], int month, int day, int hour, int minute)//ɾ��һ�������¼����Ҫ��Ϣ����λ��������ȱʡ������ 
{
	int right = 0;				//0�����޴��� 
	Record* flag;				//����ָ�루Ҫɾ������һ���ڵ㣩 
	Record* flagbefore;			//����ָ���ǰ���ڵ� 
	flag = Recordhead;			//��ʼ��ָ���ͷ
	flagbefore = Recordhead;	//��ʼ��ָ���ͷ
	while (flag->next != NULL)
	{
		flag = flag->next;
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))//��λ��������¼
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
		flagbefore = flagbefore->next;
	}
	if (!right) printf("���޴���\n");
}
void CheckTimeFrame(int startmonth, int startday, int endmonth, int endday)//��ѯĳ��ʱ�䷶Χ�ڵľ����¼ 
{
	int checkcode, MOD;
	Record* flag;//����ָ��
	flag = Recordhead;//��ʼ��ָ���ͷ
	while (flag->next != NULL)
	{
		flag = flag->next;
		checkcode = flag->situation.CheckCode;
		if ((judgek(startmonth, startday, 1, 1, flag->situation.month, flag->situation.day, 1, 1) == -1) && (judgek(flag->situation.month, flag->situation.day, 1, 1, endmonth, endday, 1, 1) == -1))//��ʱ�䷶Χ�� 
		{
			printf("����������%s �������䣺%d ҽ��������%s ������ң�%s ҽ������%s �������ڣ�%d��%d��%dʱ%d��\n", flag->patient.name, flag->patient.age, flag->doctor.name, Department[flag->doctor.department], Doctorlevel[flag->doctor.level], flag->situation.month, flag->situation.day, flag->situation.hour, flag->situation.minute);
			checkcode = reverse(checkcode);//��check���� 
			while (checkcode > 0)
			{
				MOD = checkcode % 10;
				switch (MOD)
				{
				case 1: printf("����ܼۣ�%.2f\n", flag->situation.checksum); break;				//1-��� 
				case 2: printf("ҩƷ�ܼۣ�%.2f\n", flag->situation.drugsum); break;					//2-��ҩ 
				case 3: printf("����Ѻ��%d ��Ժ���ڣ�2019-%d-%d ��Ժ���ڣ�2019-%d-%d", flag->IndexPtr->deposit, flag->IndexPtr->inmonth, flag->IndexPtr->inday, flag->IndexPtr->outmonth, flag->IndexPtr->outday); break;	//3-סԺ 
				}
				checkcode /= 10;
			}
		}
		printf("\n");
	}
}
void PrintDoctor(int JobNumber)
{
	DoctorIndex* doctorflag;
	Index_Doctor* indexflag;
	doctorflag = DoctorIndexhead;
	while (doctorflag->next != NULL)
	{
		doctorflag = doctorflag->next;
		indexflag = DoctorIndexhead->Index_Doctorhead;
		if (JobNumber == doctorflag->Job_number)
		{
			while ((indexflag->RecordPtr != NULL) && (indexflag->next != NULL))//�����һ����Ϊ�գ������ָ������ָ
			{
				indexflag = indexflag->next;
				;//������ڸ�ҽ����������Ϣ
			}
		}
	}
}
void PrintDepartment(int Code)
{
	DepartmentIndex* departmentflag;
	Index_Department* indexflag;
	departmentflag = DepartmentIndexhead;
	while (departmentflag->next != NULL)
	{
		departmentflag = departmentflag->next;
		indexflag = departmentflag->Index_Departmenthead;
		if (Code == departmentflag->ID)
		{
			while ((indexflag->RecordPtr != NULL) && (indexflag->next != NULL))//�����һ����Ϊ�գ������ָ������ָ
			{
				indexflag = indexflag->next;
				;//������ڸ����ҵ�������Ϣ
			}
			break;//���Ҳ�����������ֱ��break
		}
	}
}
void PrintPatient(char name[])
{
	int doublename = 0;
	int ageflag;
	PatientIndex* patientflag;
	Index_Patient* indexflag;
	patientflag = PatientIndexhead;
	while (patientflag->next != NULL)
	{
		patientflag = patientflag->next;
		if (strcmp(name, patientflag->name) == 0)
			doublename++;
	}
	if (doublename > 1)//������������
	{
		printf("�����������ߣ�������û��ߵ����䣺");
		scanf("%d", &ageflag);
	}
	patientflag = PatientIndexhead;
	while (patientflag->next != NULL)
	{
		patientflag = patientflag->next;
		indexflag = patientflag->Index_Patienthead;
		if (strcmp(name, patientflag->name) == 0)
		{
			while ((indexflag->RecordPtr != NULL) && (indexflag->next != NULL))//�����һ����Ϊ�գ������ָ������ָ
			{
				indexflag = indexflag->next;
				if (indexflag->RecordPtr->patient.age == ageflag)
				{
					Record* i = indexflag->RecordPtr;
					printf("\n\t________________________________________________________________________________________\n");
					printf("������Ϣ:");
					printf("\n\t����:%s\t����:%d\t�Һ�:", i->patient.name, i->patient.age);
					if (i->situation.month < 10) {
						printf("0%d", i->situation.month);
					}
					else printf("%d", i->situation.month);
					if (i->situation.day < 10) {
						printf("0%d", i->situation.day);

					}
					else printf("%d", i->situation.day);
					printf("%d", i->patient.ID);
					//                    printf("%d",i->Doctor)//ҽ������������ô���ʣ�

					printf("%d", i->patient.Register);
					printf("\nҽ����Ϣ:");
					printf("\n\t����:%s\t����:%s\t����:%s\t����:%d", i->doctor.name, Doctorlevel[i->doctor.level], Department[i->doctor.department]);
					printf("\n\t����ʱ��:%d");//����ʱ��û��ʵ��
					printf("\n�����Ϣ:\n");
					int scx = 0;

					for (int j = 0; j < i->situation.checktotal; j++) {
						while (i->situation.check[scx] == 0) { scx++; }
						printf("\t%s", Examination[scx]);
						scx++;
					}
					int k = i->situation.checksum * 100;
					int a = k / 100;
					int b = (k / 10) % 10;
					int c = k % 10;
					printf("\n\t����ܶ�:%dԪ%d��%d��", a, b, c);
					printf("\nҩƷ��Ϣ:\n");
					int cs = 0;
					for (int l = 0; l < i->situation.drugtotal; l++)
					{
						while (i->situation.drug[cs] == 0) { cs++; }
						printf("\t%s*%d", DrugsName[cs], i->situation.drug[cs]);
						cs++;
					}
					int k2 = i->situation.drugsum * 100;
					int a2 = k2 / 100;
					int b2 = (k2 / 10) % 10;
					int c2 = k2 % 10;
					printf("\n\tҩƷ�ܶ�:%dԪ%d��%d��", a2, b2, c2);
					printf("\nסԺ��Ϣ:\n");
					printf("\t��Ժʱ��:2019��%d��%d��\tԤ�Ƴ�Ժʱ��:2019��%d��%d��", patientflag->inmonth, patientflag->inday, patientflag->outmonth, patientflag->outday);
					printf("\n\tѺ�����:%d", patientflag->deposit);
					printf("\n\t________________________________________________________________________________________\n");
				}
			}
		}
	}
}
void StatisticsBusy(int month, int day)//��ѯĳһ���ҽ����æ�̶�
{
	float maxratio = -1, minratio = 10;//��ʼ�������ֵ��С����Сֵ���
	float ratio;//�н����
	float temp;//ͳ�����ҽ���������ж�������¼
	char maxname[30], minname[30];
	DoctorIndex* doctorflag;
	Index_Doctor* indexflag;
	doctorflag = DoctorIndexhead;
	while (doctorflag->next != NULL)
	{
		doctorflag = doctorflag->next;
		temp = 0;
		indexflag = doctorflag->Index_Doctorhead;
		while (indexflag->next != NULL)
		{
			indexflag = indexflag->next;
			if ((indexflag->RecordPtr->situation.month == month) && (indexflag->RecordPtr->situation.day == day))
			{
				temp = temp + 1.0;
			}
		}
		ratio = temp / 20;
		if (ratio > maxratio)
		{
			maxratio = ratio;
			strcpy(maxname, doctorflag->name);
		}
		if (ratio < minratio)
		{
			minratio = ratio;
			strcpy(minname, doctorflag->name);
		}
		printf("%sҽ������һ��ķ�æ��Ϊ��%.2f\n", doctorflag->name, ratio);
	}
	printf("��һ������æ��ҽ���ǣ�%s���䷱æ��Ϊ��%.2f\n", maxname, maxratio);
	printf("��һ�������е�ҽ���ǣ�%s���䷱æ��Ϊ��%.2f\n", minname, minratio);
}
void Save()//�ѵ�ǰϵͳ������Ϣ���� 
{
	int checkcode, MOD;
	int light = 0;
	if ((localdatafp = fopen("localdata.txt", "w")) == NULL)//��ϵͳ���������ļ� 
	{
		printf("cannot open the localdata file\n");
		exit(0);
	}
	Record* temp;
	temp = Recordhead;
	while (temp->next != NULL)
	{
		if (light == 1)
			fprintf(localdatafp, "\n");
		temp = temp->next;
		fprintf(localdatafp, "%s %d %d %s %d %d-%d-%d-%d %d", temp->patient.name, temp->patient.age, temp->patient.Register, temp->doctor.name, temp->doctor.department, temp->situation.month, temp->situation.day, temp->situation.hour, temp->situation.minute, temp->situation.CheckCode);
		checkcode = temp->situation.CheckCode;
		checkcode = reverse(checkcode);//��check���� 
		while (checkcode > 0)
		{
			MOD = checkcode % 10;
			switch (MOD)
			{
			case 1: PrintExamination(temp); break;//1-��� 
			case 2: PrintPrescribing_medicine(temp); break;//2-��ҩ 
			case 3: Printhospitalization(temp); break;//3-סԺ 
			}
			checkcode /= 10;
		}
		light = 1;
	}
	fclose(localdatafp);
}
int main()
{
	char ch;
	int nn = 5;
	if ((testfp = fopen("test.txt", "r")) == NULL)//�򿪲������� 
	{
		printf("cannot open the test file\n");
		exit(0);
	}
	ch = fgetc(testfp);
	if ((systemlogfp = fopen("systemlog.txt", "a")) == NULL)//�򿪲�����־
	{
		printf("cannot open the systemlog file\n");
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
	Preload4();																	//Ԥ����localdata�е�����
	AddRecord_Auto(5);														
	//CheckTimeFrame(7, 15, 9, 20);
	//Save();																		//������ζ�Ҫִ��һ��Save()��
	fclose(testfp);																//�ر�test�ļ�ָ��
	fclose(systemlogfp);														//�ر�systemlog�ļ�ָ��
	return 0;
}