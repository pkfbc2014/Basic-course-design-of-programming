#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <windows.h> 
#define MAXNUMI 900
/*==================================================================================*///结构体、函数声明大杂烩
typedef struct Patient					//患者信息
{
	char name[30];						//患者姓名 
	int age;							//年龄 
	int Register;						//当天的挂号，数值不超过500
	int ID;								//患者ID（双向备份）
}Patient;
typedef struct Doctor					//医生信息
{
	char name[30];						//医生姓名
	int department;						//科室
	int level;							//该医生等级
	int ID;								//医生ID（双向备份）
}Doctor;
typedef struct Situation				//诊疗情况
{
	int month, day, hour, minute;		//就诊时间 
	int CheckCode;						//做了哪些检查
	int checktotal;						//检查种类数量 
	float check[30];					//检查种类，最多三十种
	float checksum;						//检查总价
	int drugtotal;						//药品种类数量 
	int drug[30];						//药品种类，最多三十种
	float drugsum;						//药品总价
}Situation;
typedef struct Record					//诊疗记录
{
	struct Patient patient;
	struct Doctor doctor;
	struct Situation situation;
	/*诊疗记录对应医生检索链表节点中的诊疗记录指针数组的元素地址,
	为了【删除】诊疗记录时，对应检索链表的指针能置空；指针使用前要判断是否非空*/
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
typedef struct PatientIndex 			//患者检索链表节点
{
	char name[30];						//患者姓名 
	int age;							//患者年龄
	int ID;								//患者ID
	int times2;							//住院之后的扣费次数
	int inmonth;						//住院开始月
	int inday;							//住院开始日
	int outmonth;						//出院月
	int outday;							//出院日
	int deposit;						//押金，初次缴纳不得少于住院天数*200 
	int flag[368]; 						//该患者在这一年中每一天挂的号总数 
	int PatientPtrTotal;				//患者检索链表指针个数
	struct Index_Patient* Index_Patienthead;
	struct PatientIndex* next;
}PatientIndex;
typedef struct DoctorIndex				//医生检索链表节点
{
	char name[20];						//医生姓名
	int ID;								//医生ID 
	int DoctorTime;						//出诊时间
	int DoctorLevel;					//医生等级 
	int flag[366];						//该医生在这一年中每一天被挂的号总数 
	int DoctorPtrTotal;					//医生检索链表指针个数
	int Job_number;						//医生工号
	struct Index_Doctor* Index_Doctorhead;
	struct DoctorIndex* next;
}DoctorIndex;
typedef struct DepartmentIndex			//诊室检索链表节点
{
	int ID;								//诊室名称（编号）
	int DepartmentPtrTotal;				//诊室检索链表指针个数
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

time_t t;								//将t声明为时间变量，设定时间
struct tm* p;							//struct tm是一个结构体，声明一个结构体指针
char PatientName[40][20];				//所有患者的名字 
char Department[33][20];				//科室名称 
char Doctorlevel[15][16];				//医生级别
char Examination[30][30];				//检查项目名称
char DrugsName[40][30];					//药品名
int Flag[368] = { 0 };					//该医院在这一年中每一天被挂的号总数
int TotalPatientID = 0;					//患者编号中介变量
float DrugsPrice[40];					//药品价格 
double Turnover;						//医院营业总额（每次执行统计功能之前应赋值为0）
Record* Recordhead;						//诊疗记录链表头结点 
PatientIndex* PatientIndexhead;			//患者检索链表头结点 
DoctorIndex* DoctorIndexhead;			//医生检索链表头结点 
DepartmentIndex* DepartmentIndexhead;	//诊室检索链表头结点 
FILE* testfp;							//test文件指针（程序开始时打开，结束时关闭）
FILE* systemlogfp;						//系统操作日志文件指针（程序开始时打开，结束时关闭）
FILE* localdatafp;						//本地文件指针（预加载打开关闭，Save函数打开关闭）

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
		return 0;//放中间
	}
	if (a < 0 && b>0)
	{
		return 1;//放右边 
	}
	if (a > 0 && b < 0)
	{
		return -1;//放左边
	}
}
int reverse(int number)//将整数number逆序返回 
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
int DayDiff(int month_end, int day_end)//算出距离1月1日时间差
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
int gettime()//时间获得函数，此时间为题目要求的只有月日时分,配合之前发的代码中的时间实现部分使用
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
int ChangeDate(int m, int d)//把日期转换成星期，用于判断该医生是否出诊（诊疗记录是否有误） 
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
int CheckWeek(int n, int m)//n是否在m中出现
{
	while (m > 0)
	{
		if (n == (m % 10)) return 1;//出现
		m /= 10;
	}
}
void Examinationfun_Hand(Record* p)					//检查（手动添加） 
{
	int n, temp1;
	float temp2 = 0;
	scanf("%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		scanf("%d %f", &temp1, &temp2);//输入检查项目
		p->situation.check[temp1] = temp2;//标记该检查项 
		p->situation.checksum += temp2;//加上检查项目费用 
		n--;
	}
}
void Examinationfun_Auto(Record* p)					//检查（自动添加） 
{
	int n, temp1;
	float temp2 = 0;
	fscanf(testfp, "%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(testfp, "%d %f", &temp1, &temp2);//输入检查项目
		p->situation.check[temp1] = temp2;//标记该检查项 
		p->situation.checksum += temp2;//加上检查项目费用 
		n--;
	}
}
void Examinationfun_Local(Record* p)				//检查（local添加） 
{
	int n, temp1;
	float temp2 = 0;
	fscanf(localdatafp, "%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(localdatafp, "%d %f", &temp1, &temp2);//输入检查项目
		p->situation.check[temp1] = temp2;//标记该检查项 
		p->situation.checksum += temp2;//加上检查项目费用 
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
void Prescribing_medicinefun_Hand(Record* p)		//开药（手动添加） 
{
	int n, temp1, temp2;
	scanf("%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		scanf("%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//标记该药品数量 
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void Prescribing_medicinefun_Auto(Record* p)		//开药（自动添加） 
{
	int n, temp1, temp2;
	fscanf(testfp, "%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(testfp, "%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//标记该药品数量 
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void Prescribing_medicinefun_Local(Record* p)		//开药（local添加） 
{
	int n, temp1, temp2;
	fscanf(localdatafp, "%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(localdatafp, "%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//标记该药品数量 
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
void hospitalizationfun_Hand(PatientIndex* p)		//住院（手动添加）
{
	scanf("%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void hospitalizationfun_Auto(PatientIndex* p)		//住院（自动添加）
{
	fscanf(testfp, "%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void hospitalizationfun_Local(PatientIndex* p)		//住院（local添加）
{
	fscanf(localdatafp, "%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void Printhospitalization(Record* p)
{
	fprintf(localdatafp, " %d %d-%d %d-%d", p->IndexPtr->deposit, p->IndexPtr->inmonth, p->IndexPtr->inday, p->IndexPtr->outmonth, p->IndexPtr->outday);
}
void Preload1()//预加载资源库 
{
	char ch;
	FILE* fp;
	int i = 0;
	if ((fp = fopen("Database\\Drugs.txt", "r")) == NULL)//加载药品信息 
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

	if ((fp = fopen("Database\\Department.txt", "r")) == NULL)//加载科室名称 
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

	if ((fp = fopen("Database\\Doctorlevel.txt", "r")) == NULL)//加载医生级别 
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

	if ((fp = fopen("Database\\Examination.txt", "r")) == NULL)//加载检查项目名称 
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
void Preload2()//预创建医生链表
{
	char ch;
	FILE* fp;
	int i = 0;
	int ii;
	DoctorIndex* New;//将要插入的新节点
	DoctorIndex* temp;//一个中介
	temp = DoctorIndexhead;//temp初始指向表头
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
void Preload3()//预创建诊室链表 
{
	char ch;
	FILE* fp;
	int i = 0;
	int ii;
	char name[20];//诊室名字
	DepartmentIndex* New;//将要插入的新节点
	DepartmentIndex* temp;//一个中介
	temp = DepartmentIndexhead;//temp初始指向表头
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
void Preload4()//预加载localdata中的数据
{
	char ch;
	if ((localdatafp = fopen("localdata.txt", "r")) == NULL)//打开本地数据库数据 
	{
		printf("cannot open the localdata file\n");
		exit(0);
	}
	int ii;
	int MOD, checkcode;
	Record* New;//将要插入的新节点
	Record* temp;//一个中介
	Record* tempbefore;//中介的前驱节点
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	ch = fgetc(localdatafp);
	while (!feof(localdatafp))//将localdata中的数据完全读入
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
			continue;//今天的号已经被挂满了，不能增加
		while (temp->next != NULL)//找到这条记录该插在那个节点后面
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
					return;//今天的号已经被挂满了，不能增加
				New->doctor.level = FindDoctor->DoctorLevel;
				Index_Doctor* FindTempBefore = FindDoctor->Index_Doctorhead;//FindTemp的前驱节点
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
				Index_Department* FindTempBefore = FindDepartment->Index_Departmenthead;//FindTemp的前驱节点
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
					return;//今天的号已经被挂满了，不能增加
				Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp的前驱节点
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
		if (FindPatient->next == NULL)//在已有患者链表中找不到此人或者是空链表，创建新节点 
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
			Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp的前驱节点
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
		checkcode = reverse(checkcode);//将check逆序 
		while (checkcode > 0)
		{
			MOD = checkcode % 10;
			switch (MOD)
			{
			case 1: Examinationfun_Local(New); break;//1-检查 
			case 2: Prescribing_medicinefun_Local(New); break;//2-开药 
			case 3: hospitalizationfun_Local(FindPatient); break;//3-住院 
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
void AddRecord_Hand()//增加诊疗记录节点（手动添加） 
{
	int MOD, checkcode;
	int ii;
	Record* New;//将要插入的新节点
	Record* temp;//一个中介
	Record* tempbefore;//中介的前驱节点
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	printf("请输入一条诊疗记录：\n");
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
		return;//今天的号已经被挂满了，不能增加
	while (temp->next != NULL)//找到这条记录该插在那个节点后面
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
				return;//今天的号已经被挂满了，不能增加
			/*if (!(CheckWeek(ChangeDate(New->situation.month, New->situation.day), FindDoctor->DoctorTime)))
				return;//该医生在今天不出诊*/
			New->doctor.level = FindDoctor->DoctorLevel;
			Index_Doctor* FindTempBefore = FindDoctor->Index_Doctorhead;//FindTemp的前驱节点
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
			Index_Department* FindTempBefore = FindDepartment->Index_Departmenthead;//FindTemp的前驱节点
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
				return;//今天的号已经被挂满了，不能增加
			Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp的前驱节点
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
	if (FindPatient->next == NULL)//在已有患者链表中找不到此人或者是空链表，创建新节点 
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
		Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp的前驱节点
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
	checkcode = reverse(checkcode);//将check逆序 
	while (checkcode > 0)
	{
		MOD = checkcode % 10;
		switch (MOD)
		{
		case 1: Examinationfun_Hand(New); break;//1-检查 
		case 2: Prescribing_medicinefun_Hand(New); break;//2-开药 
		case 3: hospitalizationfun_Hand(FindPatient); break;//3-住院 
		}
		checkcode /= 10;
	}
	New->next = tempbefore->next;
	tempbefore->next = New;
	Flag[DayDiff(New->situation.month, New->situation.day)]++;
	New->patient.ID = FindPatient->ID;
}
void AddRecord_Auto(int NUM)//增加诊疗记录节点（自动添加n条） 
{
	int i;
	int ii;
	int MOD, checkcode;
	Record* New;//将要插入的新节点
	Record* temp;//一个中介
	Record* tempbefore;//中介的前驱节点
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	for (i = 0; i < NUM; i++)//从文件中读入NUM条记录 
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
			continue;//今天的号已经被挂满了，不能增加
		while (temp->next != NULL)//找到这条记录该插在那个节点后面
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
					return;//今天的号已经被挂满了，不能增加
				New->doctor.level = FindDoctor->DoctorLevel;
				Index_Doctor* FindTempBefore = FindDoctor->Index_Doctorhead;//FindTemp的前驱节点
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
				Index_Department* FindTempBefore = FindDepartment->Index_Departmenthead;//FindTemp的前驱节点
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
					return;//今天的号已经被挂满了，不能增加
				Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp的前驱节点
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
		if (FindPatient->next == NULL)//在已有患者链表中找不到此人或者是空链表，创建新节点 
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
			Index_Patient* FindTempBefore = FindPatient->Index_Patienthead;//FindTemp的前驱节点
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
		checkcode = reverse(checkcode);//将check逆序 
		while (checkcode > 0)
		{
			MOD = checkcode % 10;
			switch (MOD)
			{
			case 1: Examinationfun_Auto(New); break;//1-检查 
			case 2: Prescribing_medicinefun_Auto(New); break;//2-开药 
			case 3: hospitalizationfun_Auto(FindPatient); break;//3-住院 
			}
			checkcode /= 10;
		}
		New->next = tempbefore->next;
		tempbefore->next = New;
		Flag[DayDiff(New->situation.month, New->situation.day)]++;
		New->patient.ID = FindPatient->ID;
	}
}
void ChangeRecord(char name[], int month, int day, int hour, int minute)//更改一条就诊记录（需要信息来定位，可以用缺省参数） 
{
	Record* flag;			//遍历指针（要修改的那一个节点） 
	flag = Recordhead;		//初始化指向表头
	while (flag->next != NULL)
	{
		flag = flag->next;
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))//定位到这条记录
		{
			;//输出这条记录的所有信息
			DeleteRecord(name, month, day, hour, minute);//删除这条Record
			AddRecord_Hand();//重新增加一条Record	
		}
	}
}
void DeleteRecord(char name[], int month, int day, int hour, int minute)//删除一条就诊记录（需要信息来定位，可以用缺省参数） 
{
	int right = 0;				//0：查无此人 
	Record* flag;				//遍历指针（要删除的那一个节点） 
	Record* flagbefore;			//遍历指针的前驱节点 
	flag = Recordhead;			//初始化指向表头
	flagbefore = Recordhead;	//初始化指向表头
	while (flag->next != NULL)
	{
		flag = flag->next;
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))//定位到这条记录
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
	if (!right) printf("查无此人\n");
}
void CheckTimeFrame(int startmonth, int startday, int endmonth, int endday)//查询某段时间范围内的就诊记录 
{
	int checkcode, MOD;
	Record* flag;//遍历指针
	flag = Recordhead;//初始化指向表头
	while (flag->next != NULL)
	{
		flag = flag->next;
		checkcode = flag->situation.CheckCode;
		if ((judgek(startmonth, startday, 1, 1, flag->situation.month, flag->situation.day, 1, 1) == -1) && (judgek(flag->situation.month, flag->situation.day, 1, 1, endmonth, endday, 1, 1) == -1))//在时间范围内 
		{
			printf("患者姓名：%s 患者年龄：%d 医生姓名：%s 就诊科室：%s 医生级别：%s 就诊日期：%d月%d日%d时%d分\n", flag->patient.name, flag->patient.age, flag->doctor.name, Department[flag->doctor.department], Doctorlevel[flag->doctor.level], flag->situation.month, flag->situation.day, flag->situation.hour, flag->situation.minute);
			checkcode = reverse(checkcode);//将check逆序 
			while (checkcode > 0)
			{
				MOD = checkcode % 10;
				switch (MOD)
				{
				case 1: printf("检查总价：%.2f\n", flag->situation.checksum); break;				//1-检查 
				case 2: printf("药品总价：%.2f\n", flag->situation.drugsum); break;					//2-开药 
				case 3: printf("缴纳押金：%d 入院日期：2019-%d-%d 出院日期：2019-%d-%d", flag->IndexPtr->deposit, flag->IndexPtr->inmonth, flag->IndexPtr->inday, flag->IndexPtr->outmonth, flag->IndexPtr->outday); break;	//3-住院 
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
			while ((indexflag->RecordPtr != NULL) && (indexflag->next != NULL))//如果下一个不为空，且这个指针有所指
			{
				indexflag = indexflag->next;
				;//输出属于该医生的诊疗信息
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
			while ((indexflag->RecordPtr != NULL) && (indexflag->next != NULL))//如果下一个不为空，且这个指针有所指
			{
				indexflag = indexflag->next;
				;//输出属于该诊室的诊疗信息
			}
			break;//诊室不可能重名，直接break
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
	if (doublename > 1)//存在重名患者
	{
		printf("存在重名患者，请输入该患者的年龄：");
		scanf("%d", &ageflag);
	}
	patientflag = PatientIndexhead;
	while (patientflag->next != NULL)
	{
		patientflag = patientflag->next;
		indexflag = patientflag->Index_Patienthead;
		if (strcmp(name, patientflag->name) == 0)
		{
			while ((indexflag->RecordPtr != NULL) && (indexflag->next != NULL))//如果下一个不为空，且这个指针有所指
			{
				indexflag = indexflag->next;
				if (indexflag->RecordPtr->patient.age == ageflag)
				{
					Record* i = indexflag->RecordPtr;
					printf("\n\t________________________________________________________________________________________\n");
					printf("患者信息:");
					printf("\n\t姓名:%s\t年龄:%d\t挂号:", i->patient.name, i->patient.age);
					if (i->situation.month < 10) {
						printf("0%d", i->situation.month);
					}
					else printf("%d", i->situation.month);
					if (i->situation.day < 10) {
						printf("0%d", i->situation.day);

					}
					else printf("%d", i->situation.day);
					printf("%d", i->patient.ID);
					//                    printf("%d",i->Doctor)//医生工号在哪怎么访问？

					printf("%d", i->patient.Register);
					printf("\n医生信息:");
					printf("\n\t姓名:%s\t级别:%s\t科室:%s\t工号:%d", i->doctor.name, Doctorlevel[i->doctor.level], Department[i->doctor.department]);
					printf("\n\t出诊时间:%d");//出诊时间没有实现
					printf("\n检查信息:\n");
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
					printf("\n\t检查总额:%d元%d角%d分", a, b, c);
					printf("\n药品信息:\n");
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
					printf("\n\t药品总额:%d元%d角%d分", a2, b2, c2);
					printf("\n住院信息:\n");
					printf("\t入院时间:2019年%d月%d日\t预计出院时间:2019年%d月%d日", patientflag->inmonth, patientflag->inday, patientflag->outmonth, patientflag->outday);
					printf("\n\t押金余额:%d", patientflag->deposit);
					printf("\n\t________________________________________________________________________________________\n");
				}
			}
		}
	}
}
void StatisticsBusy(int month, int day)//查询某一天的医生繁忙程度
{
	float maxratio = -1, minratio = 10;//初始化：最大值最小，最小值最大
	float ratio;//中介比率
	float temp;//统计这个医生在这天有多少条记录
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
		printf("%s医生在这一天的繁忙率为：%.2f\n", doctorflag->name, ratio);
	}
	printf("这一天中最忙的医生是：%s，其繁忙率为：%.2f\n", maxname, maxratio);
	printf("这一天中最闲的医生是：%s，其繁忙率为：%.2f\n", minname, minratio);
}
void Save()//把当前系统所有信息保存 
{
	int checkcode, MOD;
	int light = 0;
	if ((localdatafp = fopen("localdata.txt", "w")) == NULL)//打开系统永存数据文件 
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
		checkcode = reverse(checkcode);//将check逆序 
		while (checkcode > 0)
		{
			MOD = checkcode % 10;
			switch (MOD)
			{
			case 1: PrintExamination(temp); break;//1-检查 
			case 2: PrintPrescribing_medicine(temp); break;//2-开药 
			case 3: Printhospitalization(temp); break;//3-住院 
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
	if ((testfp = fopen("test.txt", "r")) == NULL)//打开测试数据 
	{
		printf("cannot open the test file\n");
		exit(0);
	}
	ch = fgetc(testfp);
	if ((systemlogfp = fopen("systemlog.txt", "a")) == NULL)//打开操作日志
	{
		printf("cannot open the systemlog file\n");
		exit(0);
	}
	/*=========================================================================*///创建四个链表表头
	Recordhead = (Record*)malloc(sizeof(Record));
	Recordhead->next = NULL;
	PatientIndexhead = (PatientIndex*)malloc(sizeof(PatientIndex));
	PatientIndexhead->next = NULL;
	DoctorIndexhead = (DoctorIndex*)malloc(sizeof(DoctorIndex));
	DoctorIndexhead->next = NULL;
	DepartmentIndexhead = (DepartmentIndex*)malloc(sizeof(DepartmentIndex));
	DepartmentIndexhead->next = NULL;
	/*=========================================================================*/
	Preload1();																	//预加载资源库
	Preload2();																	//预创建医生链表 
	Preload3();																	//预创建诊室链表
	Preload4();																	//预加载localdata中的数据
	AddRecord_Auto(5);														
	//CheckTimeFrame(7, 15, 9, 20);
	//Save();																		//无论如何都要执行一遍Save()！
	fclose(testfp);																//关闭test文件指针
	fclose(systemlogfp);														//关闭systemlog文件指针
	return 0;
}