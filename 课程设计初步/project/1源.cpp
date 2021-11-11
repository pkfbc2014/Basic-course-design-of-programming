#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#define MAXNUMI 900
/*==================================================================================*///结构体大杂烩
time_t t;//将t声明为时间变量                                         //设定时间
struct tm* p;//struct tm是一个结构体，声明一个结构体指针
float turnover;
int Departmentnumber = 0;
int Examinationnumber = 0;
int Drugnumber = 0;
int judge(int a)//判断某个月有几天
{

	if (a == 2) {
		return 29;
	}
	else if (a == 1 || a == 3 || a == 5 || a == 7 || a == 8 || a == 10 || a == 12) {
		return 31;
	}
	else return 30;
}
int count(int a, int b, int c, int d)//获取两个日期之间的天数
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
typedef struct Patient					//患者信息
{
	int ID;
	char name[30];						//患者姓名
	int age;							//年龄
	int Register;   //当天的挂号，数值不超过500


} Patient;
typedef struct Doctor					//医生信息
{
	char name[30];						//医生姓名
	int department;	//科室
	int ID;
	int level;
} Doctor;
typedef struct Situation				//诊疗情况
{
	int CheckCode;						//做了哪些检查
	int month, day, hour, minute;		//就诊时间
	int checktotal;						//检查种类数量
	int check[30];						//检查种类，最多三十种
	float checksum;						//检查总价
	int drugtotal;						//药品种类数量
	int drug[30];						//药品种类，最多三十种
	float drugsum;						//药品总价
} Situation;
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
	struct Record* next;
} Record;
typedef struct PatientIndex 			//患者检索链表节点
{
	int ID;
	char IDcard[30];
	char name[30];						//患者姓名
	int times2;							//住院之后的扣费次数
	int inmonth;						//住院开始月
	int inday;							//住院开始日
	int outmonth;						//出院月
	int outday;							//出院日
	int deposit;						//押金，初次缴纳不得少于住院天数*200
	int flag[368]; 						//该患者在这一年中每一天挂的号总数
	struct Record* index[900];
	int PatientIndexTOP;				//患者检索链表栈顶序号
	struct PatientIndex* next;
} PatientIndex;
typedef struct DoctorIndex				//医生检索链表节点
{
	char name[20];						//医生姓名
	int ID;								//医生ID
	int DoctorTime;						//出诊时间
	int DoctorLevel;					//医生等级
	int flag[366];						//该医生在这一年中每一天被挂的号总数
	struct Record* index[900];
	struct DoctorIndex* next;
	int DoctorIndexTOP;					//医生检索链表栈顶序号
} DoctorIndex;
typedef struct DepartmentIndex			//诊室检索链表节点
{
	int ID;								//诊室名称（编号）
	struct Record* index[900];
	struct DepartmentIndex* next;
	int DepartmentIndexTOP;				//诊室检索链表栈顶序号
} DepartmentIndex;
/*==================================================================================*/
int judgechinese(char* p)//利用汉字ASCII码两个字节的最高位都是1来进行判断是否输入汉字
{
	int i = 0;
	while (p[i] != '\0') {
		if ((p[i] >> 8) == 0) //将p字节进行移位运算，右移8位，这样，如果移位后是0，则说明原来的字节最高位为0，不是1那么也就不是汉字的一个字节。
		{
			return 0;
		}

		i++;
	}
	return 1;
}
char PatientName[40][20];				//所有患者的名字
char Department[33][20];				//科室名称
char Doctorlevel[15][16];				//医生级别
char Examination[30][30];				//检查项目名称
char DrugsName[40][30];					//药品名
int Flag[368] = { 0 };						//该医院在这一年中每一天被挂的号总数
float DrugsPrice[40];					//药品价格
int TotalPatientID = 0;					//患者编号中介变量
Record* Recordhead;						//诊疗记录链表头结点
PatientIndex* PatientIndexhead;			//患者检索链表头结点
DoctorIndex* DoctorIndexhead;			//医生检索链表头结点
DepartmentIndex* DepartmentIndexhead;	//诊室检索链表头结点
int PatientIndexTOP = 0;				//患者检索链表栈顶序号
int DoctorIndexTOP = 0;					//医生检索链表栈顶序号
int DepartmentIndexTOP = 0;				//诊室检索链表栈顶序号
int TotalRecord = 0;					//该医院系统总诊疗情况数量（用于统计医生繁忙程度）
FILE* nfp;//test文件
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
	return (d2 - d1);
}
void Examinationfun_Hand(Record* p)		//检查，手动添加		 
{
	int n, temp1;
	float temp2 = 0;
	printf("\n\t请输入患者进行检查的项目数:");
	while(scanf("%d", &p->situation.checktotal)!=1||p->situation.checktotal> Examinationnumber||p->situation.checktotal<0) {
		printf("\n\t您的输入有误！请重新输入：");
	}
	n = p->situation.checktotal;
	printf("\n\t检查项目:");
	int i = 0;
	printf("\n\t|____________________________________________检查项目列表_____________________________________________|\t\n\n");
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
		printf("\n\t请选择患者进行检查的项目：");
		while(scanf("%d", &temp1)!=1||temp1>Examinationnumber-1||temp1<0) {
			printf("\n\t您的输入有误！请重新输入：");
		}
		printf("\n\t请输入患者检查费用：");
		while(scanf("%f", &temp2)!=1||temp2<0) {
			printf("\n\t您的输入有误！请重新输入：");
		}//输入检查项目
		turnover += temp2;
		p->situation.check[temp1] = 1;//标记该检查项
		p->situation.checksum += temp2;//加上检查项目费用
		n--;
	}
}//检查（手动添加）
void Examinationfun_Auto(Record* p)					//检查（自动添加）
{
	int n, temp1;
	float temp2 = 0;
	fscanf(nfp, "%d", &p->situation.checktotal);
	n = p->situation.checktotal;
	while (n > 0)
	{
		fscanf(nfp, "%d %f", &temp1, &temp2);//输入检查项目
		p->situation.check[temp1] = 1;//标记该检查项
		p->situation.checksum += temp2;//加上检查项目费用
		n--;
	}
}
void Prescribing_medicinefun_Hand(Record* p)	//开药（手动添加）
{
	int n, temp1, temp2;
	printf("\n\t请输入患者所开药品的种类数:");
	while(scanf("%d", &p->situation.drugtotal)!=1||p->situation.drugtotal<0||p->situation.drugtotal>Drugnumber) {
		printf("\n\t您的输入有误！请重新输入：");
	}
	n = p->situation.drugtotal;
	int i = 0;
	printf("\n\t|_________________________________________药品列表__________________________________________________|\t\n\n");
	for (; i < Drugnumber; i++)
	{
		printf("\t%d.%s      ", i, DrugsName[i]);
		if (i % 5 == 0 && i != 0)
			printf("\n\n");
	}
	printf("\n\t|___________________________________________________________________________________________________|\t\n\n");
	while (n > 0)
	{
		printf("\n\t请选择一项患者所开的药品:");
		while(scanf("%d", &temp1)!=1||temp1<0||temp1>Drugnumber-1) {
			printf("\n\t您的输入有误！请重新输入：");
		}
		printf("\n\t请输入患者所开本项药品的数量:");
		while(scanf("%d",  &temp2)!=1||temp2<0||temp2>100) {
			printf("\n\t您的输入有误！请重新输入：");
		}
		turnover += DrugsPrice[temp1] * temp2;
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		p->situation.drug[temp1] = temp2;//标记该药品数量
		n--;
	}
}
void Prescribing_medicinefun_Auto(Record* p)		//开药（自动添加）
{
	int n, temp1, temp2;
	fscanf(nfp, "%d", &p->situation.drugtotal);
	n = p->situation.drugtotal;
	while (n > 0)
	{
		fscanf(nfp, "%d-%d", &temp1, &temp2);
		p->situation.drug[temp1] = temp2;//标记该药品数量
		p->situation.drugsum += DrugsPrice[temp1] * temp2;
		n--;
	}
}
void hospitalizationfun_Hand(PatientIndex* q)			//住院（手动添加）
{
	int k = 0;
	printf("\n\t请输入患者入院日期:");
	printf("\n\n\t月:");
	while (scanf("%d", &q->inmonth) != 1 || q->inmonth > 12 || q->inmonth < 0) {
		printf("\n\t您的输入有误！请重新输入：");
	}
	printf("\n\t日:");
	while (scanf("%d", &q->inday) != 1 || q->inday < 0||q->inday>31) {
		printf("\n\t您的输入有误！请重新输入：");
	}
	printf("\n\t请输入患者预计出院日期:");
	printf("\n\n\t月:");
	while (scanf("%d", &q->outmonth) != 1 || q->outmonth > 12 || q->outmonth < 0) {
		printf("\n\t您的输入有误！请重新输入：");
	}
	printf("\n\t日:");
	while (scanf("%d", &q->outday) != 1 || q->outday < 0 || q->outday>31) {
		printf("\n\t您的输入有误！请重新输入：");
	}
	k = count(q->inmonth, q->inday, q->outmonth, q->outday) * 200;
	printf("\n\t请输入患者缴纳的押金数目:");
	scanf("%d", &(q->deposit));
	int j = q->deposit;
	while (j - k < 0 || j < 1000) {
		printf("\n\t缴纳押金数目不足！请重新输入:");
		scanf("%d", &(q->deposit));
		j = q->deposit;
	}
}
void hospitalizationfun_Auto(PatientIndex* p)		//住院（自动添加）
{
	fscanf(nfp, "%d %d-%d %d-%d", &p->deposit, &p->inmonth, &p->inday, &p->outmonth, &p->outday);
}
void Preload1()//预加载资源库
{
	FILE* fp;
	int i = 0;
	if ((fp = fopen("Database\\Drugs.txt", "r")) == NULL)//加载药品信息
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

	if ((fp = fopen("Database\\Department.txt", "r")) == NULL)//加载科室名称
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
		Examinationnumber++;
		fscanf(fp, "%s", Examination[i]);
		i++;
	}
	fclose(fp);
}

void Preload2()//预创建医生链表
{
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
void Preload3()//预创建诊室链表
{
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
}//判断checkcode输入是否合法
Record* AddRecord_Hand(Record* Recordhead)//增加诊疗记录节点（手动添加）
{
	int MOD, checkcode;
	int ii;
	char huancun[100];
	Record* New;//将要插入的新节点
	Record* temp;//一个中介
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

	printf("\n\t请输入患者姓名:");
	while (scanf("%s", New->patient.name) != 1||judgechinese(New->patient.name)==0 ){
		printf("\t您的输入有误！请重新输入：");
		}
	printf("\n\t请输入患者年龄:");
	while (scanf("%d", &New->patient.age) != 1 || New->patient.age <= 0 || New->patient.age >= 120) {
		
		printf("\t您的输入有误！请重新输入：");
	}
	printf("\n\t请输入医生姓名:");
	gets(huancun);
	while (scanf("%s", New->doctor.name) != 1||judgechinese(New->doctor.name)==0) {
		printf("\t您的输入有误！请重新输入：");
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

	printf("\n\t请选择患者就诊的科室:");
	while (scanf("%d", &New->doctor.department) != 1 || New->doctor.department<0 || New->doctor.department>Departmentnumber-1) {
		printf("\t您的输入有误！请重新输入：");
	}
	printf("\n\t输入患者就诊的时间:\n");
	printf("\n\t月:");
	while (scanf("%d", &New->situation.month) != 1 || New->situation.month < 1 || New->situation.month>12) 
		{
			printf("\t您的输入有误！请重新输入：");
		}

	printf("\n\t日:");
	while (scanf("%d", &New->situation.day) != 1 || New->situation.day < 0 || New->situation.day>31) {
		printf("\t您的输入有误！请重新输入：");
	}
	printf("\n\t时:");
	while(scanf("%d", &New->situation.hour)!=1||New->situation.hour>23||New->situation.hour<0) {
		printf("\t您的输入有误！请重新输入：");
	}
	printf("\n\t分:");
	while(scanf("%d", &New->situation.minute)!=1||New->situation.minute>60||New->situation.minute<0) {
		printf("\t您的输入有误！请重新输入：");
	}
	printf("\n\t|_______________________________________________________________________________________________|\t\n");
	printf("\n\t1.检查         2.开药       3.住院\n");
	printf("\n\t|_______________________________________________________________________________________________|\t\n");
	printf("\n\t请选择患者的诊疗内容:");

	while(scanf("%d", &New->situation.CheckCode)!=1||judgecheckcode(New->situation.CheckCode)==0) {
		printf("\t您的输入有误！请重新输入：");
	}
	if (Flag[DayDiff(New->situation.month, New->situation.day)] == 500)
		return Recordhead;//今天的号已经被挂满了，不能增加
	while (temp->next != NULL)//找到这条记录该插在那个节点后面
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
				return Recordhead;//今天的号已经被挂满了，不能增加
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
				return Recordhead;//今天的号已经被挂满了，不能增加
			FindPatient->index[FindPatient->PatientIndexTOP] = New;
			New->Patientptr = &FindPatient->index[FindPatient->PatientIndexTOP];
			FindPatient->PatientIndexTOP++;
			FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
			break;
		}
	}
	if (FindPatient->next == NULL)//在已有患者链表中找不到此人或者是空链表，创建新节点
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
	New->next = temp->next;
	temp->next = New;
	Flag[DayDiff(New->situation.month, New->situation.day)]++;
	New->patient.ID = FindPatient->ID;
	TotalRecord++;
}


void AddRecord_Auto(int NUM)//增加诊疗记录节点（自动添加n条）
{
	int i;
	int ii;
	int MOD, checkcode;
	Record* New;//将要插入的新节点
	Record* temp;//一个中介
	PatientIndex* FindPatient;
	DoctorIndex* FindDoctor;
	DepartmentIndex* FindDepartment;
	temp = Recordhead;
	New = (Record*)malloc(sizeof(Record));
	for (i = 0; i < NUM; i++)//从文件中读入NUM条记录
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
			continue;//今天的号已经被挂满了，不能增加
		while (temp->next != NULL)//找到这条记录该插在那个节点后面
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
			};//今天的号已经被挂满了，不能增加
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
		};//今天的号已经被挂满了，不能增加
		FindPatient->index[FindPatient->PatientIndexTOP] = New;
		New->Patientptr = &FindPatient->index[FindPatient->PatientIndexTOP];
		FindPatient->PatientIndexTOP++;
		FindPatient->flag[DayDiff(New->situation.month, New->situation.day)]++;
		break;
	}

	if (FindPatient->next == NULL)//在已有患者链表中找不到此人或者是空链表，创建新节点
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
	New->next = temp->next;
	temp->next = New;
	Flag[DayDiff(New->situation.month, New->situation.day)]++;
	New->patient.ID = FindPatient->ID;
	TotalRecord++;
}



void ChangeRecord(char name[], int month, int day, int hour, int minute, Record* Recordhead)//更改一条就诊记录（需要信息来定位，可以用缺省参数）
{
	Record* flag;			//遍历指针（要修改的那一个节点）
	flag = Recordhead;		//初始化指向表头
	while (flag->next != NULL)
	{
		if ((strcmp(name, flag->patient.name) == 0) && (month == flag->situation.month) && (day == flag->situation.day) && (hour == flag->situation.hour) && (minute == flag->situation.minute))
		{
			//输出这条记录的所有信息，叫用户选择，选择什么改什么
		}
		flag = flag->next;
	}
}
void DeleteRecord(char name[], int month, int day, int hour, int minute, Record* Recordhead)//删除一条就诊记录（需要信息来定位，可以用缺省参数）
{
	int right = 0;			//0：查无此人
	Record* flag;			//遍历指针（要删除的那一个节点）
	Record* flagbefore;		//遍历指针的前驱节点
	flag = Recordhead->next;	//初始化指向数据域第一个
	flagbefore = Recordhead;	//初始化指向表头
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
	if (!right) printf("查无此人\n");
}
void CheckTimeFrame(int startmonth, int startday, int endmonth, int endday, Record* Recordhead)//查询某段时间范围内的就诊记录
{
	int checkcode, MOD;
	Record* flag;//遍历指针
	flag = Recordhead->next;//初始化指向表头
	while (flag != NULL)
	{
		checkcode = flag->situation.CheckCode;
		if ((startmonth <= flag->situation.month) && (startday <= flag->situation.day) && (flag->situation.month <= endmonth) && (flag->situation.day <= endday))//在时间范围内
		{
			printf("患者姓名：%s 患者年龄：%d 医生姓名：%s 就诊科室：%s 医生级别：%s 就诊日期：%d月%d日%d时%d分\n", flag->patient.name, flag->patient.age, flag->doctor.name, Department[flag->doctor.department], Doctorlevel[flag->doctor.level], flag->situation.month, flag->situation.day, flag->situation.hour, flag->situation.minute);
			checkcode = reverse(checkcode);//将check逆序
			while (checkcode > 0)
			{
				MOD = checkcode % 10;
				switch (MOD)
				{
				case 1: printf("检查总价：%.2f\n", flag->situation.checksum); break;				//1-检查
				case 2: printf("药品总价：%.2f\n", flag->situation.drugsum); break;		//2-开药
////////////////////case 3: printf("缴纳押金：%d 入院日期：2019-%d-%d 出院日期：2019-%d-%d",); break;	//3-住院
						//如何查找这个人的住院信息啊？？？！！！
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
					;//输出属于该医生的诊疗记录
			}
			break;//该医生的找完了不必继续往下找
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
					;//输出属于该诊室的诊疗记录
			}
			break; //该诊室的找完了不必继续往下找
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
//					;//输出属于该患者的诊疗记录
//			}
//			break;//该患者的找完了不必继续往下找
//		}
//
//	}
//}
void StatisticsBusy()
{
	float max = -1, min = 10;//初始化：最大值最小，最小值最大
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
		printf("%s医生的繁忙率为：%.2f\n", flag->name, ratio);
	}
	printf("最忙的医生是：%s，其繁忙率为：%.2f\n", maxname, max);
	printf("最闲的医生是：%s，其繁忙率为：%.2f\n", minname, min);
}
int gettime()//时间获得函数
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
int checkdate(struct PatientIndex* i)//检测患者是否需要缴费的函数
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



float getturnover(PatientIndex* i)//(Float)遍历扣除押金的营业额查看函数
{


	if ((gettime() % 10000) / 100 >= 8) {

		while (i != NULL)
		{
			turnover += 200 * checkdate(i);
			i->deposit -= 200 * checkdate(i);
			if (i->deposit < 1000)
			{
				printf("\n\t%d号患者%s住院押金已不足1000元，请通知缴纳住院押金!", i->ID, i->name);
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
	printf("\n\t医院的营业额为:%d元%d角%d分", a, b, c);
	return 0;
}
int PrintInPatient(PatientIndex* i)//打印住院患者表
{

	printf("\n\t|_________________________________________住院患者表__________________________________________________|\t\n\n");
	int k = 0;
	while (i != NULL) {
		if (i->inmonth != 0 && i->outmonth != 0) {
			printf("\t%d号患者:%s        ", i->ID, i->name);
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
void Save()//把当前系统所有信息保存
{
	;//保存诊疗记录
	;//保存日志
}

int ChangeDate(int y, int m, int d)//把日期转换成星期，用于判断该医生是否出诊（诊疗记录是否有误）
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
	p = localtime(&t);//获得当地的时间


	if ((nfp = fopen("test.txt", "r")) == NULL)//打开测试数据
	{
		printf("cannot open the test file\n");
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
	Preload3();
	/*=========================================================================*/
	if (fopen("Database\\turnover.txt", "r+") != NULL)                                    //录入营业额
	{
		turnoverfp = fopen("Database\\turnover.txt", "r+");
		fscanf(turnoverfp, "%f", &turnover);
	}
	else
	{
		printf("营业额文件无法正常打开");
		exit(0);
	}


	/*=========================================================================*/
	system("color F4");                                             //设定交互界面
//0 = 黑色 8 = 灰色（“亮黑色”）
//1 = 蓝色 9 = 亮蓝色
//2 = 绿色 A = 亮绿色
//3 = 湖蓝色 B = 亮湖蓝色
//4 = 红色 C = 亮红色
//5 = 紫色 D = 亮紫色
//6 = 黄色 E = 亮黄色
//7 = 白色 F = 亮白色 第一位为背景色 第二位为字体色
	/*=========================================================================*/
	while (1)
	{



		printf(

			"\n\t|_________________________诊疗记录管理系统________________________________________________|\t\n"
			"\t|1:录入诊疗记录                                                                           |\t\n"
			"\t|2:从文件中导入诊疗记录                                                                   |\t\n"
			"\t|3:从系统中删除诊疗记录                                                                   |\t\n"
			"\t|4:查看科室诊疗信息                                                                       |\t\n"
			"\t|5:查询医生诊疗信息                                                                       |\t\n"
			"\t|6:查看营业额                                                                             |\t\n"
			"\t|7:查看住院患者                                                                           |\t\n"
			"\t|8:查看医生出诊情况                                                                       |\t\n"
			"\t|9:退出程序                                                                               |\t\n"
			"\t|____________________________________%d-%d-%d %d:%d:%d___________________________________|\t\n\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec
		);
		int func;
		printf("\t请输入您要进行的操作：");
		while (scanf("%d", &func) != 1 || func < 1 || func>9)
		{
			printf("\n\t您的输入有误！请重新输入：");

		}
		gets(huancun);
		switch (func)
		{
		case 1:
			AddRecord_Hand(Recordhead);
		case 2:
			int m;
			printf("\n\t请输入您想录入诊疗记录的数量:");
			while(scanf("%d", &m)!=1||m<0) {
				printf("\n\t您的输入有误！请重新输入：");

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
			//保存文件并更新日期

			//
			//关闭文件

						//此处应该再复制case 8的代码

			fclose(turnoverfp);

		}

	}




	Save();//无论如何都要执行一遍Save()
	fclose(nfp);
	return 0;
}
