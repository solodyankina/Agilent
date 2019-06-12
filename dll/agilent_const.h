#ifndef agilent2H
#define agilent2H
//---------------------------------------------------------------------------
#endif

// ��������� ��� ����������

//channel

//Drv_ReadValue
unsigned int Agilent_Amplitude = 0;
unsigned int Agilent_Overshoot = 1;
unsigned int Agilent_FallTime = 2;
unsigned int Agilent_RiseTime = 3;
//Drv_Statistics
unsigned int Agilent_MaskWaveform = 0;
unsigned int Agilent_MaskFWavwform = 1;
unsigned int Agilent_MaskTime = 2;
//Drv_SetTrigger
unsigned int Agilent_Edge = 0;
	unsigned int Agilent_positive = 0;
	unsigned int Agilent_negative = 1;
	unsigned int Agilent_either = 2;
	unsigned int Agilent_alternate = 3;
unsigned int Agilent_DELay= 1;
unsigned int Agilent_GLITch = 2;
	unsigned int Agilent_greaterthan = 0;
	unsigned int Agilent_lessthan = 1;
	unsigned int Agilent_range = 2;
unsigned int Agilent_TRANsition = 3;
unsigned int Agilent_EBURst = 4;
unsigned int Agilent_RUNT = 5;
unsigned int Agilent_SHOLd = 6;
unsigned int Agilent_M1553 = 7;
	unsigned int Agilent_DSTArt = 0;
	unsigned int Agilent_DSTOp = 1;
	unsigned int Agilent_CSTArt = 2;
	unsigned int Agilent_CSTop = 3;
	unsigned int Agilent_RTA = 4;
	unsigned int Agilent_RTA11 = 5;
	unsigned int Agilent_PERRor = 6;
	unsigned int Agilent_SERRor = 7;
	unsigned int Agilent_MERRor = 8;
unsigned int Agilent_Single = 0;
unsigned int Agilent_Auto = 1;
unsigned int Agilent_Normal = 2;
unsigned int Agilent_TypeCon_Transformer = 0;
unsigned int Agilent_TypeCon_Direct = 1;

//----------------------------------------
unsigned int Error_NoError = 0;
unsigned int NoErrors = 0;
unsigned int UnknowError = 0x40000000;
unsigned int ErrorLoadLybrary = 0x13130001;
