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
					;//输出属于该患者的诊疗信息
				}			
			}
		}
	}
}