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
					;//������ڸû��ߵ�������Ϣ
				}			
			}
		}
	}
}