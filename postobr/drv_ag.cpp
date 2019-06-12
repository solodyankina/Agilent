// ---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include "myvisa.h"
#pragma hdrstop

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
//#include <iostream>
#include <tchar.h>
#include <fstream>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Chart.hpp>
//#include "TeEngine.hpp"
//#include "TeeProcs.hpp"
//#include <ExtCtrls.hpp>
//#include "Series.hpp"
//#include "TeeFunci.hpp"
//#include <Dialogs.hpp>

#include "agilent_const.h"
#include "myvisa.h"
#include "math.h"
// -------------------------------------------------------------------------

#pragma argsused
extern "C" __declspec(dllexport)
__stdcall unsigned int Spo_Drv_CreateScriptParametrs(bool*bNeedScript,
	unsigned int*iMaxNeedScript, unsigned int*iLengthDrvName, unsigned int*iLengthMyScriptName);
extern "C" __declspec(dllexport)
__stdcall unsigned int Spo_Drv_GetScriptParametrs(wchar_t*cNeedScriptName, wchar_t*cDrvName,
	wchar_t*cMyScriptName);
extern "C" __declspec(dllexport)__stdcall unsigned int Spo_Drv_FindDevice
(unsigned int*iCount, unsigned int*iParamLevels, unsigned int*iMaxParamName);
extern "C" __declspec(dllexport)__stdcall unsigned int Spo_Drv_GetDeviceInfo (wchar_t*cParam);
extern "C" __declspec(dllexport)__stdcall unsigned int Spo_Drv_DeviceOpen(unsigned int iNum, unsigned int * iDeviceHandle);
extern "C" __declspec(dllexport)__stdcall unsigned int Spo_Drv_DeviceClose();

extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SetChanel (int NK_);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_ReadValue (unsigned int param, double*val);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_ReadStatistics (unsigned int param, double*val);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_ResetStatistics();
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerShold(unsigned int param, double param2, double param3, double param4);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerRunt(unsigned int param, unsigned int param2, double param3);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerEburst(unsigned int param, double param3, double param4);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerTransition(unsigned int param, unsigned int param2, double param3);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerGlitch(unsigned int param, unsigned int param2, double param3,double param4);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerDelay(unsigned int param, unsigned int param2, double param3, double param4);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerEdge(unsigned int param);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_TriggerM1553(unsigned int param, char *param2, char *param3);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_StartTestWithMask (int flen, char * pfilename);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SetSignalMask (double dx, double dy);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_StartTest(int flen, char * pfilename,bool bHtml);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_StopTest();
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SetScaleU (double val);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SetScaleT (double val);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_ReadTriggerStatus(bool*state);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SetMode(unsigned int param);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_WaitTrigger (double time, bool*state);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_GetWaveDataSize(unsigned int *size);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_GetWaveDataFun();
extern "C" __declspec(dllexport)__stdcall unsigned int  Drv_GetWaveDataParam(double *xreference, double *xorigin, double *xincrement,
double *yreference, double *yorigin, double *yincrement);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_GetWaveData(char *mass, unsigned int size);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SaveWaveData (char * pfilename,unsigned int fmt);
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_Reset();
extern "C" __declspec(dllexport)__stdcall unsigned int Drv_SetTypeCon(unsigned int param);
// ---------------------------------------------------------------------------
double deviation = 2; //отклонение
double timeT = 0.000001; //100mks (время развертки)
// Глобальные переменные
int NK = 0; // номер канала, по умолчанию 0
unsigned int err;
ViSession defaultRM, vi;
char strres[20];
unsigned long actual;
char buf[256] = {0};
AnsiString str, str1, imadgedir;
double  statistics,scaleu,scalet, *scale;
unsigned int points_count;
float X, Y;
unsigned long retCount = 0;
double myscaleT = 0.000005, myscaleU = 5;
double maxA,minA,maxOv,minOv,maxF,minF,maxR,minR,maxOU,minOU;
bool format = 0;

fviOpenDefaultRM viOpenDefaultRM;
fviOpen viOpen;
fviPrintf viPrintf;
fviScanf viScanf;
fviClose viClose;
fviRead viRead;
HINSTANCE visa32dll = NULL;

// настройки работы с устройством
char * DrvName_ = "AGILENT";
char * MyScriptName_ = "AGILENT";

char bStopAll;
#define DrvName_SIZE 7
#define MyScriptName_SIZE 7 //sizeof()

char * ExistAGILENTList[255];
unsigned int iCountFindDevice, // число найденных устройств
iMaxFindDeviceParamName; // нмакс имя устройства

// -------------
int myvisa_init(void) {
	if (visa32dll!=NULL)
		return 1;
	visa32dll = LoadLibrary("visa32.dll");
	if (visa32dll != NULL) {
		if (!(viOpenDefaultRM = (fviOpenDefaultRM)GetProcAddress(visa32dll,
					"viOpenDefaultRM")))
			return 0;
		if (!(viOpen = (fviOpen)GetProcAddress(visa32dll, "viOpen")))
			return 0;
		if (!(viPrintf = (fviPrintf)GetProcAddress(visa32dll, "viPrintf")))
			return 0;
		if (!(viScanf = (fviScanf)GetProcAddress(visa32dll, "viScanf")))
			return 0;
		if (!(viClose = (fviClose)GetProcAddress(visa32dll, "viClose")))
			return 0;
		if (!(viRead = (fviRead)GetProcAddress(visa32dll, "viRead")))
			return 0;
	}
	else
		return 0; // false
	return 1; // ok
}


unsigned int __stdcall  Drv_SetChanel(int NK_)
{
    NK = NK_;
	// запрашиваем канал
	str = ":STATUS? CHANEL" + IntToStr(NK) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	// выбираем канал
	str = ":VIEW CHANnel" + IntToStr(NK) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
    return Error_NoError;
}

unsigned int __stdcall Drv_ReadValue(unsigned int param, double *val) {
	str = ":MEASure:SOURce CHANnel" + IntToStr(NK) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
			return err;
	if (param == Agilent_Amplitude) {
		str = ":MEASure:VAMPlitude[" + IntToStr(NK) + "]\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		err = viPrintf(vi, ":MEASure:VAMPlitude?\n");
		if (err != Error_NoError)
			return err;
		// Read results
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
	   //	amplitude = atof(&buf[0]);
	}
	if (param == Agilent_Overshoot) {
		str = ":MEASure:OVERshoot[CHANnel" + IntToStr(NK) + "]\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		err = viPrintf(vi, ":MEASure:OVERshoot?\n");
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
		//overshoot = atof(&buf[0]);
	}
	if (param == Agilent_FallTime) {
		str = ":MEASure:FALLtime[" + IntToStr(NK) + "]\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		err = viPrintf(vi, ":MEASure:FALLtime?\n");
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
		//falltime = atof(&buf[0]);
	}
	if (param == Agilent_RiseTime) {
		str = ":MEASure:RISetime[" + IntToStr(NK) + "]\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		err = viPrintf(vi, ":MEASure:RISetime?\n");
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
	  //	ristime = atof(&buf[0]);
	}
	return Error_NoError;
}

unsigned int __stdcall Drv_Statistics(unsigned int param, double *val) {
	if (param == Agilent_MaskWaveform) {
		err = viPrintf(vi, ":MTESt:COUNt:WAVeforms?\n");
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
	   //	count_test = atof(&buf[0]);
	}
	if (param == Agilent_MaskFWavwform) {
		str = ":MTESt:COUNt:FWAVeforms? [CHANnel" + IntToStr(NK) + "]\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
		//failwaveform = atof(&buf[0]);
	}
	if (param == Agilent_MaskTime) {
		err = viPrintf(vi, ":MTESt:COUNt:TIME?\n");
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		*val = atof(&buf[0]);
		//timemask = atof(&buf[0]);
	}

/*	err = viPrintf(vi, ":MEASure:STATistics 1\n");
	if (err != Error_NoError)
			return err;
	err = viPrintf(vi, ":MEASure:RESults?\n");
	viScanf (vi, "%t", &buf[0]);
	if (err != Error_NoError)
			return err;
	*val = atof(&buf[0]);
	statistics = atof(&buf[0]); */
	return Error_NoError;
}

unsigned int __stdcall Drv_ResetStatistics()
{
	err = viPrintf(vi, ":MEASure:STATistics:RESet\n");
	if (err != Error_NoError)
		return err;
	return Error_NoError;
}

AnsiString MyFloatToStr(double i)
{
	AnsiString str = FloatToStr(i);
	//меняем запятую на точку
	int k = str.Pos(",");
	if (k>0)
		str[k] = '.';
	return str;
}

// по фронту
unsigned int __stdcall Drv_TriggerEdge(unsigned int param)
{
	err = viPrintf(vi, ":TRIGger:MODE EDGE\n");
	if (err != Error_NoError)
		return err;
	if (param == Agilent_positive)
	{
		err = viPrintf(vi, ":TRIGger[:EDGE]:SLOPe POSitive\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_negative)
	{
		err = viPrintf(vi, ":TRIGger[:EDGE]:SLOPe NEGative\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_either)
	{
		err = viPrintf(vi, ":TRIGger[:EDGE]:SLOPe EITHer\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_alternate)
	{
		err = viPrintf(vi, ":TRIGger[:EDGE]:SLOPe ALTernate\n");
		if (err != Error_NoError)
			return err;
	}
	return Error_NoError;
}
	// по фронту за фронтом
unsigned int __stdcall Drv_TriggerDelay(unsigned int  param, unsigned int param2, double param3,double param4)
{
	err = viPrintf(vi, ":TRIGger:MODE DELay\n");
	if (err != Error_NoError)
		return err;
	if (param == Agilent_positive)
	{
		err = viPrintf(vi, ":TRIGger:DELay:ARM:SLOPe POSitive\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_negative)
	{
		err = viPrintf(vi, ":TRIGger:DELay:ARM:SLOPe NEGative\n");
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_positive)
	{
		err = viPrintf(vi, ":TRIGger:DELay:TRIGger:SLOPe POSitive\n");
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_negative)
	{
		err = viPrintf(vi, ":TRIGger:DELay:TRIGger:SLOPe NEGative\n");
		if (err != Error_NoError)
			return err;
	}
	str = ":TRIGger:DELay:TDELay:TIME " + MyFloatToStr(param3) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
			return err;
	str =  ":TRIGger:DELay:TRIGger:COUNt " + MyFloatToStr(param4) + "\n" ;
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
			return err;
	return Error_NoError;
}
//по длительности импульса
unsigned int __stdcall Drv_TriggerGlitch(unsigned int param, unsigned int param2, double param3,double param4)
{
	err = viPrintf(vi, ":TRIGger:MODE GLITch\n");
	if (err != Error_NoError)
		return err;
	if (param == Agilent_positive)
	{
	err = viPrintf(vi, ":TRIGger:GLITch:POLarity POSitive\n");
	if (err != Error_NoError)
		return err;
	}
	if (param == Agilent_negative)
	{
		err = viPrintf(vi, ":TRIGger:GLITch:POLarity NEGative\n");
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_greaterthan)
	{
		// больше заданного времеи
		err = viPrintf(vi, ":TRIGger:GLITch:QUALifier GREaterthan\n");
		if (err != Error_NoError)
			return err;
		str = ":TRIGger:GLITch:GREaterthan " + MyFloatToStr(param3) + "\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_lessthan)
	{
		// меньше заданного времеи
		err = viPrintf(vi, ":TRIGger:GLITch:QUALifier LESSthan\n");
		if (err != Error_NoError)
			return err;
		str = ":TRIGger:GLITch:LESSthan " + MyFloatToStr(param4) + "\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_range)
	{
		// в пределах диапозона
		err = viPrintf(vi, ":TRIGger:GLITch:QUALifier RANGe\n");
		if (err != Error_NoError)
			return err;
		str = ":TRIGger:GLITch:RANGe " + MyFloatToStr(param3) + "," + MyFloatToStr(param4) + "\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}

	return Error_NoError;
}
//по длительности нарастания/спада
unsigned int __stdcall Drv_TriggerTransition(unsigned int param, unsigned int param2, double param3)
{
	err = viPrintf(vi, ":TRIGger:MODE TRANsition\n");
	if (err != Error_NoError)
		return err;
	if (param == Agilent_positive)
	{
		err = viPrintf(vi, ":TRIGger:TRANsition:SLOPe POSitive\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_negative)
	{
		err = viPrintf(vi, ":TRIGger:TRANsition:SLOPe NEGative\n");
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_greaterthan)
	{
		err = viPrintf(vi, ":TRIGger:TRANsition:QUALifier GREaterthan\n");
		if (err != Error_NoError)
		return err;
	}
	if (param2 == Agilent_lessthan)
	{
		err = viPrintf(vi, ":TRIGger:TRANsition:QUALifier LESSthan\n");
		if (err != Error_NoError)
			return err;
	}
	str = ":TRIGger:TRANsition:TIME " + MyFloatToStr(param3) + "us\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	return Error_NoError;
}
// N front
unsigned int __stdcall Drv_TriggerEburst(unsigned int param, double param3, double param4)
{
	err = viPrintf(vi, ":TRIGger:MODE EBURst\n");
	if (err != Error_NoError)
		return err;
	if (param == Agilent_positive)
	{
	err = viPrintf(vi, ":TRIGger:EBURst:SLOPe POSitive\n");
	if (err != Error_NoError)
	return err;
	}
	if (param == Agilent_negative)
	{
	err = viPrintf(vi, ":TRIGger:EBURst:SLOPe NEGative\n");
	if (err != Error_NoError)
		return err;
	}
	str = ":TRIGger:EBURst:COUNt " + MyFloatToStr(param3) + "\n"; // какой фронтт
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	str = ":TRIGger:EBURst:IDLE " + MyFloatToStr(param4) + "\n"; // время бездействия
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	return Error_NoError;
}
// запуск по короткому пакету
unsigned int __stdcall Drv_TriggerRunt(unsigned int param, unsigned int param2, double param3)
{
	err = viPrintf(vi, ":TRIGger:MODE RUNT\n");
	if (err != Error_NoError)
		return err;
	if (param == Agilent_positive)
	{
		err = viPrintf(vi, ":TRIGger:RUNT:POLarity POSitive\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_negative)
	{
		err = viPrintf(vi, ":TRIGger:RUNT:POLarity NEGative\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_either)
	{
		err = viPrintf(vi, ":TRIGger:RUNT:POLarity EITHer\n");
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_greaterthan)
	{
		err = viPrintf(vi, ":TRIGger:RUNT:QUALifier GREaterthan\n");
		if (err != Error_NoError)
			return err;
	}
	if (param2 == Agilent_lessthan)
	{
		err = viPrintf(vi, ":TRIGger:RUNT:QUALifier LESSthan\n");
		if (err != Error_NoError)
			return err;
	}
	str = ":TRIGger:RUNT:TIME " + MyFloatToStr(param3) + "us\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	return Error_NoError;
}

unsigned int __stdcall Drv_TriggerM1553(unsigned int param, char *param2, char *param3)
{
	str = ":TRIGger:MODE SBUS" + IntToStr(NK) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	str = ":SBUS" + IntToStr(NK) + ":MODE M1553\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	str = ":SBUS" + IntToStr(NK) + ":M1553:SOURce CHANnel" + NK + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:SOURce?\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:AUTosetup\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	if (param == Agilent_DSTArt)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE DSTArt\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_DSTOp)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE DSTOp\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_CSTArt)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE CSTArt\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_CSTop)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE CSTop\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_RTA)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE RTA\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		AnsiString type = param2;
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:RTA " + type + "\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_RTA11)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE RTA11\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		AnsiString type = param2;
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:RTA " +
			type  + "\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
		AnsiString time = param3;
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:PATTern:DATA "  + char(34)
			+ time +char(34) +"\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_PERRor)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE PERRor\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_SERRor)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE SERRor\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_MERRor)
	{
		str = ":SBUS" + IntToStr(NK) + ":M1553:TRIGger:TYPE MERRor\n";
		err = viPrintf(vi, str.c_str());
		if (err != Error_NoError)
			return err;
	}
	return Error_NoError;
}

unsigned int __stdcall Drv_SetSignalMask(double dx, double dy)
{
   	str = ":MTESt:AMASk:UNITs CURRent\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	// dX
	str = ":MTESt:AMASk:XDELta " + MyFloatToStr(dx) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	// dY
	str = ":MTESt:AMASk:YDELta " + MyFloatToStr(dy) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
    Sleep(1000);
	// подключиться к каналу
	str = ":MTESt:AMASk:SOURce CHANnel" + MyFloatToStr(NK) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	// create mask
	err = viPrintf(vi, ":MTESt:AMASk:CREate\n");
	if (err != Error_NoError)
		return err;
	// остановки при ошибке
	err = viPrintf(vi, ":MTESt:RMODe:FACTion:STOP 1\n");
	if (err != Error_NoError)
		return err;
    return Error_NoError;
}

char Check_Range (double val,double min,double max)
{
  // nenorma
  if ((val<min)||(val>max))
	return 0;
  //norma
  else
	return 1;
}

void MyCreateBin(char* filename,double xreference,double xorigin,double xincrement,
	unsigned long size, double *mydata)
{
	HANDLE binFile;
	bool bResult;
	unsigned long n;
	unsigned long mysize=size*sizeof(double);
	int k;
	// открытие файла для добавления данных
	binFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (binFile == INVALID_HANDLE_VALUE)
		return;
	SetFilePointer(binFile, 0, 0, FILE_END);

	AnsiString str_date = DateToStr(Now());
	AnsiString str_time = TimeToStr(Now());
	//заполняем файл
	WriteFile(binFile,str_date.c_str(),10,&n,NULL);
	WriteFile(binFile,str_time.c_str(),8,&n,NULL);
	WriteFile(binFile,&xreference,2,&n,NULL);
	WriteFile(binFile,&xorigin,8,&n,NULL);
	WriteFile(binFile,&xincrement,10,&n,NULL);
	WriteFile(binFile,&size,4,&n,NULL);
	int i = 0;
	while(i<size)
	{
		if (mysize<=(mysize-i)) {
		  for (i = 0; i < size; i++)
		  {
			bResult=WriteFile(binFile,&mydata[i],mysize,&n,NULL); // size * sizeof(double) т.к. считываем количество всех точек, а он делит на количества байт
			*((char *) ((unsigned int)mydata+k)) ;
			if ((bResult == false) || (n==0))
				break;
		  }
		}
		else
			bResult=WriteFile(binFile,&mydata[i],size-i,&n,NULL);
			if ((bResult == false) || (n==0))
				break;
	}
	CloseHandle(binFile);
}

void MyCreateBitMap(Graphics::TBitmap *MyBitmap, AnsiString file_name, double *mydata, unsigned int size, double max, double min)
{
	double dx,dy,X,Y;
	dx=20;
	dy=20;

	MyBitmap->Canvas->Pen->Color = clBlack;
	MyBitmap->Canvas->Pen->Width = 1;
	MyBitmap->Canvas->Brush->Color = clWhite;
	MyBitmap->Canvas->Rectangle(dx-1,dy-1,MyBitmap->Width - dx+1, MyBitmap->Height - dy+1);

	//координатные оси
	MyBitmap->Canvas->Pen->Color = clBlack;
	MyBitmap->Canvas->Pen->Width = 2;
	MyBitmap->Canvas->MoveTo(dx, MyBitmap->Height/2);
	MyBitmap->Canvas->LineTo(MyBitmap->Width-dx, MyBitmap->Height/2);
	MyBitmap->Canvas->MoveTo(MyBitmap->Width/2,dy);
	MyBitmap->Canvas->LineTo(MyBitmap->Width/2,MyBitmap->Height - dy);
	MyBitmap->Canvas->TextOutA(5, MyBitmap->Height/2,"0V");
	MyBitmap->Canvas->TextOutA(5,5,MyFloatToStr(scaleu) + "V");
	MyBitmap->Canvas->TextOutA(MyBitmap->Width/2+2,5,MyFloatToStr(scalet*1000000) + " ms");
	//сетка
	MyBitmap->Canvas->Pen->Color = clGray;
	MyBitmap->Canvas->Pen->Width = 1;
	for (int i = 0; i < 10; ++i)
	{
		MyBitmap->Canvas->MoveTo(dx+int(i*(MyBitmap->Width-2*dx)/10),dy);
		MyBitmap->Canvas->LineTo(dx+int(i*(MyBitmap->Width-2*dx)/10),MyBitmap->Height-dy);
		MyBitmap->Canvas->MoveTo(dx,dy+int(i*(MyBitmap->Height-2*dy)/8));
		MyBitmap->Canvas->LineTo(MyBitmap->Width - dx,dy+int(i*(MyBitmap->Height-2*dy)/8));
	}
	MyBitmap->Canvas->Pen->Color = clRed;
	X = dx;
	Y = (MyBitmap->Height - 2*dy)*(max - mydata[0])/(max - min) + dy;
	MyBitmap->Canvas->MoveTo(int(X),int(Y));
	for (int i = 1; i < size; i++)
	{
		X = (MyBitmap->Width - 2*dx)*i/size + dx;
		Y = (MyBitmap->Height - 2*dy)*(max - mydata[i])/(max - min) + dy;
		MyBitmap->Canvas->LineTo(int(X),int(Y));
	}
//	str = file_name + "im.bmp";
	MyBitmap->SaveToFile(file_name.c_str());
}

unsigned int __stdcall Drv_SetTypeCon(unsigned int param)
{
	if (param == Agilent_TypeCon_Transformer)
	{
	   minA = 18; maxA = 27;
	   minOv = -0.9; maxOv = 0.9;
	   minR = 100E-09; maxR = 300E-09;
	   minF = 100E-09; maxF = 300E-09;
	   minOU = -0.25; maxOU = 0.25;
	}
	if (param == Agilent_TypeCon_Direct)
	{
	   minA = 6; maxA = 9;
	   minOv = -0.3; maxOv = 0.3;
	   minR = 100E-09; maxR = 300E-09;
	   minF = 100E-09; maxF = 300E-09;
	   minOU = -0.09; maxOU = 0.09;
	}
}

unsigned int __stdcall Drv_StartTestWithMask(int flen, char* pfilename)
{
	double val;
	ofstream f;
	bStopAll = false;

	double dx,dy,X,Y, xreference, xorigin, xincrement, yreference, yorigin, yincrement;
	double amplitude,overshoot,ristime,falltime,ostU,st,ct,spad,a,b,Umax,Umin,minFall,maxFall,
		minRis,maxRis,minAmplit,maxAmplit,maxOvershoot,minOvershoot,minOstU,maxOstU;
	unsigned int size;
	char *filename = new char[flen+1];
	char *filename2 = new char[flen+1];
	//полное имя файла
	int kkk;
	for (kkk=0; kkk<=flen-1; kkk++)
	{
		filename[kkk]=pfilename[kkk];

	}
	filename[flen]='\0';
	//имя папки для картинок
	for (kkk=0; kkk<=flen-1; kkk++)
	{
		if (pfilename[kkk]=='.')
			break;
		filename2[kkk]=pfilename[kkk];
	}
	filename2[kkk] ='\0';
	AnsiString file_name = filename2;
	//ShowMessage(filename2);
	//throw(Exception(file_name));
	CreateDir(file_name);
	f.open(filename);
	int k=0,j=0;
	dx = 20;
	dy = 20;
	unsigned int *mylocal;
	unsigned int mylocal_cnt;

	err = Drv_SetMode(Agilent_Single);
	if (err != Error_NoError)
		return err;
	str = ":MEASure:SOURce CHANnel" + IntToStr(NK) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;

	Graphics::TBitmap *MyBitmap = new Graphics::TBitmap();
		MyBitmap->Width = 800;
		MyBitmap->Height = 400;
	while(true)
	{
		Application->ProcessMessages();
		if (bStopAll)
		  break;
		err = viPrintf(vi, ":OPERegister:CONDition?\n");
		if (err != Error_NoError)
			return err;
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		if ((str == "+32\n") || (str == "+544\n"))
		{
			AnsiString str_date = DateToStr(Now());
			AnsiString str_time = TimeToStr(Now());
			f << "<head>" << "\n" << "<meta http-equiv=Content-Type content=" <<
				"text/html; charset=windows-1251" << "></head>";
			f << "<p>" << str_date.c_str() <<" / "<< str_time.c_str() << "</p>";
			//st = myGetMSTicks(); //для подсчета затраченного временни
			k=k+1;  // считает количество циклов
			BOOL bFindWaveEnd = false;
			do
			{
				err = Drv_GetWaveDataParam(&xreference, &xorigin, &xincrement,
					&yreference, &yorigin, &yincrement);
				if (err != Error_NoError)
					return err;
				err = Drv_GetWaveDataFun();
				err = Drv_GetWaveDataSize(&size);
				if (err != Error_NoError)
					return err;
				if (size<10)
					break;
				mylocal = new unsigned int[size];
				char *mass = new char [size];
				err = Drv_GetWaveData(mass, size);
				if (err != Error_NoError)
					return err;
				//	size = 1000;
				size = size/2;

				double *mydata = new double[size];
				for (int i = 0; i < size; i++)
				{
					WORD w = mass[i*2]*0x100 + mass[i*2+1];
					short dt = * ((short * )(&w));
					if (dt > 0 )
					{
						mydata[i] = -(abs(dt - yreference)*yincrement)+yorigin;
					}
					else
					{
						mydata[i] = (abs(-dt - yreference)*yincrement)+yorigin;
					}
					//для отладки:
					//	mydata[i] = i % 200;
				}
				//смотрим точеи графика
				double max =mydata[0];
				double min =mydata[0];
				mylocal_cnt=1;
				mydata[0] = 0;
				char bMeasured = false;
				for (int i = 2; i < size-2; i++)
				{
					if ((mydata[i]-mydata[i-1])*(mydata[i+1]-mydata[i])<=0)
					{
						// if (fabsl(mydata[i]-mydata[mylocal[mylocal_cnt - 1]])<1E-5)
						//	continue;
						mylocal_cnt = mylocal_cnt+1;
						mylocal[mylocal_cnt - 1] = i;
						//f<<"Y: " << mydata[i]<<"\n";
					}
					if (max<mydata[i])
						max = mydata[i];
					if (min>mydata[i])
						min = mydata[i];
				}
				if (size>1)
				{
					mylocal[mylocal_cnt] = size-1;
					mylocal_cnt++;
					if (max<mydata[size-1])
						max = mydata[size-1];
					if (min>mydata[size-1])
						min = mydata[size-1];
				}
				f<<"\r\n\r\n";

				unsigned int iLastPoint = mylocal_cnt-1;
				unsigned int i;
				for ( i = mylocal_cnt-1; i > 0 ; i--)
				{
					if (fabsl(mydata[mylocal[i]] - mydata[mylocal[mylocal_cnt-1]]) > deviation)
						break;
				}
				if ((mylocal[mylocal_cnt-1]-mylocal[i+1])*xincrement > timeT)
				{
					bFindWaveEnd = true;
					iLastPoint = i+1;
				}

				Umax=0; Umin=0;
				minRis=300; maxRis=0;
				minFall=300; maxFall=0;
				minOstU=300; maxOstU=0;
				minAmplit=300; maxAmplit=0;
				maxOvershoot=0; minOvershoot=300;
				overshoot = 0;
				// если не конец сигнала
				for (i = 1; i < iLastPoint; i++)
				{
				//f<<"Y2: " << mydata[mylocal[i]]<<"\n";
				// если перепад разного знака
					if (((mydata[mylocal[i]]*mydata[mylocal[i+1]])<=0)&&(fabsl(mydata[mylocal[i+1]])>=0.3))
					{
							if (i>mylocal_cnt-3)
								continue;
							//среднее значение напряжения пика
							X =( mydata[mylocal[i+1]]+ mydata[mylocal[i+2]])/2;
							if (X>=0)
								Umax = X;
							else
								Umin = X;
							if (fabsl(mydata[mylocal[i]])<0.3)
								continue;
							//амплитуда 18-27В
							amplitude = Umax-Umin;
							if (minAmplit>amplitude)
								minAmplit=fabsl(amplitude);
							if (maxAmplit<amplitude)
								maxAmplit=amplitude;
							bMeasured = 1;
					//определяем нарастание и спад  100-300нс
							a = (mydata[mylocal[i]]-mydata[mylocal[i+1]])/((double)mylocal[i]-(double)mylocal[i+1]);
							b = mydata[mylocal[i]] - a*mylocal[i];
							spad = ((Umin + 0.1*(Umax - Umin)-b)/a-(Umin + 0.9*(Umax - Umin)-b)/a)*xincrement;
							if (mydata[mylocal[i]]>0)
							{
								falltime=spad; //спад
								if (minFall>spad)
									minFall=spad;
								if (maxFall<spad)
									maxFall=spad;
							}
							if (mydata[mylocal[i]]<0)
							{
								ristime=spad; //нарастание
								if (minRis>fabsl(spad))
									minRis=fabsl(spad);
								if (maxRis<fabsl(spad))
									maxRis=fabsl(spad);
							}
					}//if
					//выбросы на вершине +-900мВ(перепад одного знака)
					if (((mydata[mylocal[i]]*mydata[mylocal[i+1]])>=0)&&
						(fabsl(mydata[mylocal[i+1]])>=0.3))
					{
						if ((mydata[mylocal[i]]>=Umax)&& (Umax!=0))
						{
							if (overshoot < mydata[mylocal[i]] - Umax)
								overshoot = mydata[mylocal[i]] - Umax;
						}
						else
						if ((mydata[mylocal[i]]<=Umin)&&(Umin!=0))
						{
							if (overshoot < fabsl(mydata[mylocal[i]] - Umin))
								overshoot = fabsl(mydata[mylocal[i]] - Umin);
						}
					}
					else
					if (fabsl(overshoot)>1E-200)
					{
						if (minOvershoot>overshoot)
							minOvershoot=overshoot;
						if (maxOvershoot<overshoot)
							maxOvershoot=overshoot;
						overshoot  = 0;
					}
				}//for
				max = max + 0.05*(max - min);
				min = min - 0.05*(max - min);
				//если конец сигнала,считаем остаточное напряжение
				ostU = 0;
				if (bFindWaveEnd == true)
				{
					for (int i = mylocal_cnt-1; i >iLastPoint; i--)
					{
						if ((ostU<mydata[mylocal[i]])&& (fabsl(mydata[mylocal[i+1]])>=0.3))
							ostU = mydata[mylocal[i]];
						if (minOstU>ostU)
						{
							minOstU=ostU;
							bMeasured = 1;
						}
						if (maxOstU<ostU)
						{
							maxOstU=ostU;
							bMeasured = 1;
						}
					}
				}

				//проверяем измеренения на отклонения
				if (bMeasured == 1)
				if ((Check_Range(maxAmplit,minA,maxA)==0)||(Check_Range(minAmplit,minA,maxA)==0)
					||(Check_Range(maxOvershoot,0,maxOv)==0)||(Check_Range(minOvershoot,0,minOv)==0)
					||(Check_Range(maxRis,minR,maxR)==0)||(Check_Range(minRis,minR,maxR)==0)
					||(Check_Range(maxFall,minF,maxF)==0)||(Check_Range(minFall,minF,maxF)==0)
					||(Check_Range(maxOstU,0,maxOU)==0)||(Check_Range(minOstU,0,maxOU)==0))
				{
					str = file_name + "\\" + "im" + IntToStr(j) + ".bmp";
					MyCreateBitMap(MyBitmap,str, mydata, size,max,min);
					j++;
					//заполняем документ
					f<<"<p align=center><img src="<< str.c_str()<<"></p>"<<"\n"<<"</body></html>";
					f<< "<table border="<<"1"<<"><tr><td>"<<"Измерение"<<"</td><td>"<<"MIN"<<"</td><td>"<<"MAX"<<"</td><td>"<<"Норма"<<"</td></tr>";
					if ((Check_Range(maxAmplit,minA,maxA)==0)||(Check_Range(minAmplit,minA,maxA)))
					{
						f<< "<tr><td> Амплитуда (В)</td><td> "<<minAmplit<<" </td><td> "<<maxAmplit<<" </td><td> 6 - 9 B</td></tr> ";
					}
					if ((Check_Range(maxOvershoot,minOv,maxOv)==0)||(Check_Range(minOvershoot,minOv,maxOv)==0)) {
						f<< "<tr><td> Колебания (В) </td><td> "<<minOvershoot<<" </td><td>"<<maxOvershoot<<"</td><td> 0..0.3 B</td></tr>";
					}
					if ((Check_Range(maxRis,minR,maxR)==0)||(Check_Range(minRis,minR,maxR)==0)) {
						f<< "<tr><td> Нарастание фронта (нс) </td><td> "<<minRis*1000000000<<" </td><td>"<<maxRis*1000000000<<"</td><td> 100 - 300 нс </td></tr>";
					}
					if ((Check_Range(maxFall,minF,maxF)==0)||(Check_Range(minFall,minF,maxF)==0)) {
						f<< "<tr><td> Спад фронта (нс) </td><td> "<<minFall*1000000000<<" </td><td>"<<maxFall*1000000000<<"</td><td> 100 - 300 нс </td></tr>";
					}
					if (bFindWaveEnd == true){

						if ((Check_Range(maxOstU,0,maxOU)==0)||(Check_Range(minOstU,0,maxOU)==0)) {
							f<< "<tr><td> Остаточное напряжение (В) </td><td> "<<minOstU<<" </td><td>"<<maxOstU<<"</td><td>  0..0.9 B</td></tr>";
						}
					}
					f<<"</table>";

				} // if от условия с ошибками
				if (bFindWaveEnd == false)
				{
					str = ":TIMebase:POSition " + MyFloatToStr((mylocal[iLastPoint] -xreference)*xincrement + xorigin + 5*myscaleT) + "\n";
					viPrintf(vi, str.c_str());
				}

			} while(bFindWaveEnd == false); //к do
			//пеервод тригера в режим ожидания сигала
			str = ":TIMebase:POSition 0.0000\n";
			viPrintf(vi, str.c_str());
			Drv_SetScaleT(scalet);
			err = Drv_SetMode(Agilent_Single);
			if (err != Error_NoError)
				return err;
			//считаем время
			//ct = myGetMSTicks();
			//f << "<p>" << "Всего затрачено времени: "<< ct-st << "</p>";
		}//if тригер сработал   ;
	}//wile not bStop
	MyBitmap->Free();
	f << "<p>" << "Всего тестов: "<< k << "</p>";
	f.close();
	return Error_NoError;
}

unsigned int __stdcall Drv_StartTest(int flen, char * pfilename,bool bHtml)
{
	double val;
	ofstream f;
	bStopAll = false;

	double dx,dy,X,Y, xreference, xorigin, xincrement, yreference, yorigin, yincrement;
	double amplitude,overshoot,ristime,falltime,ostU,st,ct,spad,a,b,Umax,Umin,minFall,maxFall,
		minRis,maxRis,minAmplit,maxAmplit,maxOvershoot,minOvershoot,minOstU,maxOstU;
	unsigned int size;
	char *filename = new char[flen+1];
	char *filename2 = new char[flen+1];
	//полное имя файла
	int kkk;
	for (kkk=0; kkk<=flen-1; kkk++)
	{
		filename[kkk]=pfilename[kkk];

	}
	filename[flen]='\0';
	//имя папки для картинок
	for (kkk=0; kkk<=flen-1; kkk++)
	{
		if (pfilename[kkk]=='.')
			break;
		filename2[kkk]=pfilename[kkk];
	}
	filename2[kkk] ='\0';
	AnsiString file_name = filename2;
	//ShowMessage(filename2);
	//throw(Exception(file_name));
	CreateDir(file_name);
	f.open(filename);
	int k=0,j=0;
	dx = 20;
	dy = 20;
	unsigned int *mylocal;
	unsigned int mylocal_cnt;
	unsigned int iLastPoint;

	if (ExtractFileExt(filename)==".html")
	{
		bHtml=true;
	}
	err = Drv_SetMode(Agilent_Single);  // устанавливаем режим единичного измерения
	if (err != Error_NoError)
		return err;
	str = ":MEASure:SOURce CHANnel" + IntToStr(NK) + "\n";    // подключаемся к каналу
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	Graphics::TBitmap *MyBitmap = new Graphics::TBitmap(); // для создания картинки
   if (bHtml==true) {
		f.open(filename);

		MyBitmap->Width = 800;  // ширина и высота
		MyBitmap->Height = 400;
   }
	while(true)  // пока не прервемся в цикле
	{
		Application->ProcessMessages();
		if (bStopAll)   // прерывемся если стоп
		  break;
		err = viPrintf(vi, ":OPERegister:CONDition?\n"); // сработал ли тригер
		if (err != Error_NoError)
			return err;
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		if ((str == "+32\n") || (str == "+544\n"))     // если триггер сработал
		{

			//st = myGetMSTicks(); //для подсчета затраченного временни
			k=k+1;  // считает количество циклов
			BOOL bFindWaveEnd = false;
			do
			{
				err = Drv_GetWaveDataParam(&xreference, &xorigin, &xincrement,
					&yreference, &yorigin, &yincrement);
				if (err != Error_NoError)
					return err;
				err = Drv_GetWaveDataFun();
				err = Drv_GetWaveDataSize(&size);
				if (err != Error_NoError)
					return err;
				if (size<10)
					break;

				mylocal = new unsigned int[size];
				char *mass = new char [size];
				err = Drv_GetWaveData(mass, size);
				if (err != Error_NoError)
					return err;
                //	size = 1000;
				size = size/2;   // делим на 2 т.к. формат задан word и выделяется по 2 байта

				double *mydata = new double[size];  // массив содержащий точки сигнала, в вольтах
				for (int i = 0; i < size; i++)
				{
					WORD w = mass[i*2]*0x100 + mass[i*2+1];
					short dt = * ((short * )(&w));
					if (dt > 0 )
					{
						mydata[i] = -(abs(dt - yreference)*yincrement)+yorigin;
					}
					else
					{
						mydata[i] = (abs(-dt - yreference)*yincrement)+yorigin;
					}
					//для отладки:
					//	mydata[i] = i % 200;
				}
				char bMeasured = false;
				double max =mydata[0];
				double min =mydata[0];
				//смотрим точеи графика

				mylocal_cnt=1;
				mydata[0] = 0;

				for (int i = 2; i < size-2; i++)
				{
					if ((mydata[i]-mydata[i-1])*(mydata[i+1]-mydata[i])<=0)
					{
						// if (fabsl(mydata[i]-mydata[mylocal[mylocal_cnt - 1]])<1E-5)
						//	continue;
						mylocal_cnt = mylocal_cnt+1;
						mylocal[mylocal_cnt - 1] = i;
						//f<<"Y: " << mydata[i]<<"\n";
					}
					if (max<mydata[i])
						max = mydata[i];
					if (min>mydata[i])
						min = mydata[i];
				}
				if (size>1)
				{
					mylocal[mylocal_cnt] = size-1;
					mylocal_cnt++;
					if (max<mydata[size-1])
						max = mydata[size-1];
					if (min>mydata[size-1])
						min = mydata[size-1];
				}
				f<<"\r\n\r\n";
				if (bHtml==false)
				{
					MyCreateBin(filename,xreference,xorigin,xincrement,size,mydata);
				}
				unsigned int i;
				iLastPoint = mylocal_cnt-1;
				for ( i = mylocal_cnt-1; i > 0 ; i--)
				{
					if (fabsl(mydata[mylocal[i]] - mydata[mylocal[mylocal_cnt-1]]) > deviation)
						break;
				}
				if ((mylocal[mylocal_cnt-1]-mylocal[i+1])*xincrement > timeT)
				{
					bFindWaveEnd = true;
					iLastPoint = i+1;
				}
			if (bHtml==true) {


				Umax=0; Umin=0;
				minRis=300; maxRis=0;
				minFall=300; maxFall=0;
				minOstU=300; maxOstU=0;
				minAmplit=300; maxAmplit=0;
				maxOvershoot=0; minOvershoot=300;
				overshoot = 0;
				// если не конец сигнала
				for (i = 1; i < iLastPoint; i++)
				{
				//f<<"Y2: " << mydata[mylocal[i]]<<"\n";
				// если перепад разного знака
					if (((mydata[mylocal[i]]*mydata[mylocal[i+1]])<=0)&&(fabsl(mydata[mylocal[i+1]])>=0.3))
					{
							if (i>mylocal_cnt-3)
								continue;
							//среднее значение напряжения пика
							X =( mydata[mylocal[i+1]]+ mydata[mylocal[i+2]])/2;
							if (X>=0)
								Umax = X;
							else
								Umin = X;
							if (fabsl(mydata[mylocal[i]])<0.3)
								continue;
							//амплитуда 18-27В
							amplitude = Umax-Umin;
							if (minAmplit>amplitude)
								minAmplit=fabsl(amplitude);
							if (maxAmplit<amplitude)
								maxAmplit=amplitude;
							bMeasured = 1;
					//определяем нарастание и спад  100-300нс
							a = (mydata[mylocal[i]]-mydata[mylocal[i+1]])/((double)mylocal[i]-(double)mylocal[i+1]);
							b = mydata[mylocal[i]] - a*mylocal[i];
							spad = ((Umin + 0.1*(Umax - Umin)-b)/a-(Umin + 0.9*(Umax - Umin)-b)/a)*xincrement;
							if (mydata[mylocal[i]]>0)
							{
								falltime=spad; //спад
								if (minFall>spad)
									minFall=spad;
								if (maxFall<spad)
									maxFall=spad;
							}
							if (mydata[mylocal[i]]<0)
							{
								ristime=spad; //нарастание
								if (minRis>fabsl(spad))
									minRis=fabsl(spad);
								if (maxRis<fabsl(spad))
									maxRis=fabsl(spad);
							}
					}//if
					//выбросы на вершине +-900мВ(перепад одного знака)
					if (((mydata[mylocal[i]]*mydata[mylocal[i+1]])>=0)&&
						(fabsl(mydata[mylocal[i+1]])>=0.3))
					{
						if ((mydata[mylocal[i]]>=Umax)&& (Umax!=0))
						{
							if (overshoot < mydata[mylocal[i]] - Umax)
								overshoot = mydata[mylocal[i]] - Umax;
						}
						else
						if ((mydata[mylocal[i]]<=Umin)&&(Umin!=0))
						{
							if (overshoot < fabsl(mydata[mylocal[i]] - Umin))
								overshoot = fabsl(mydata[mylocal[i]] - Umin);
						}
					}
					else
					if (fabsl(overshoot)>1E-200)
					{
						if (minOvershoot>overshoot)
							minOvershoot=overshoot;
						if (maxOvershoot<overshoot)
							maxOvershoot=overshoot;
						overshoot  = 0;
					}
				}//for
				max = max + 0.05*(max - min);
				min = min - 0.05*(max - min);
				//если конец сигнала,считаем остаточное напряжение
				ostU = 0;
				if (bFindWaveEnd == true)
				{
					for (int i = mylocal_cnt-1; i >iLastPoint; i--)
					{
						if ((ostU<mydata[mylocal[i]])&& (fabsl(mydata[mylocal[i+1]])>=0.3))
							ostU = mydata[mylocal[i]];
						if (minOstU>ostU)
						{
							minOstU=ostU;
							bMeasured = 1;
						}
						if (maxOstU<ostU)
						{
							maxOstU=ostU;
							bMeasured = 1;
						}
					}
				}

				if (bMeasured == 1)
				if ((Check_Range(maxAmplit,minA,maxA)==0)||(Check_Range(minAmplit,minA,maxA)==0)
					||(Check_Range(maxOvershoot,0,maxOv)==0)||(Check_Range(minOvershoot,0,minOv)==0)
					||(Check_Range(maxRis,minR,maxR)==0)||(Check_Range(minRis,minR,maxR)==0)
					||(Check_Range(maxFall,minF,maxF)==0)||(Check_Range(minFall,minF,maxF)==0)
					||(Check_Range(maxOstU,0,maxOU)==0)||(Check_Range(minOstU,0,maxOU)==0))
				{
					AnsiString str_date = DateToStr(Now());
					AnsiString str_time = TimeToStr(Now());
					f << "<head>" << "\n" << "<meta http-equiv=Content-Type content=" <<
						"text/html; charset=windows-1251" << "></head>";
					f << "<p>" << str_date.c_str() <<" / "<< str_time.c_str() << "</p>";
					str = file_name + "\\" + "im" + IntToStr(j) + ".bmp";
					MyCreateBitMap(MyBitmap,str, mydata, size,max,min);
					j++;
					//заполняем документ
					f<<"<p align=center><img src="<< str.c_str()<<"></p>"<<"\n"<<"</body></html>";
					f<< "<table border="<<"1"<<"><tr><td>"<<"Измерение"<<"</td><td>"<<"MIN"<<"</td><td>"<<"MAX"<<"</td><td>"<<"Норма"<<"</td></tr>";
					if ((Check_Range(maxAmplit,minA,maxA)==0)||(Check_Range(minAmplit,minA,maxA)))
					{
						f<< "<tr><td> Амплитуда (В)</td><td> "<<minAmplit<<" </td><td> "<<maxAmplit<<" </td><td> 6 - 9 B</td></tr> ";
					}
					if ((Check_Range(maxOvershoot,minOv,maxOv)==0)||(Check_Range(minOvershoot,minOv,maxOv)==0)) {
						f<< "<tr><td> Колебания (В) </td><td> "<<minOvershoot<<" </td><td>"<<maxOvershoot<<"</td><td> 0..0.3 B</td></tr>";
					}
					if ((Check_Range(maxRis,minR,maxR)==0)||(Check_Range(minRis,minR,maxR)==0)) {
						f<< "<tr><td> Нарастание фронта (нс) </td><td> "<<minRis*1000000000<<" </td><td>"<<maxRis*1000000000<<"</td><td> 100 - 300 нс </td></tr>";
					}
					if ((Check_Range(maxFall,minF,maxF)==0)||(Check_Range(minFall,minF,maxF)==0)) {
						f<< "<tr><td> Спад фронта (нс) </td><td> "<<minFall*1000000000<<" </td><td>"<<maxFall*1000000000<<"</td><td> 100 - 300 нс </td></tr>";
					}
					if (bFindWaveEnd == true){
						if ((Check_Range(maxOstU,0,maxOU)==0)||(Check_Range(minOstU,0,maxOU)==0)) {
						f<< "<tr><td> Остаточное напряжение (В) </td><td> "<<minOstU<<" </td><td>"<<maxOstU<<"</td><td>  0..0.9 B</td></tr>";
						}
					}
					f<<"</table>";
				} // if от условия с ошибками
				} // if от bhtml=true

				if (bFindWaveEnd == false)
				{
					str = ":TIMebase:POSition " + MyFloatToStr((mylocal[iLastPoint] -xreference)*xincrement + xorigin + 5*myscaleT) + "\n";
					viPrintf(vi, str.c_str());
				}

			} while(bFindWaveEnd == false); //к do
			//пеервод тригера в режим ожидания сигала
			str = ":TIMebase:POSition 0.0000\n";
			viPrintf(vi, str.c_str());
			Drv_SetScaleT(scalet);
			err = Drv_SetMode(Agilent_Single);
			if (err != Error_NoError)
				return err;
			//считаем время
			//ct = myGetMSTicks();
			//f << "<p>" << "Всего затрачено времени: "<< ct-st << "</p>";
		}//if тригер сработал   ;
	}//wile not bStop
	MyBitmap->Free();
	if (bHtml==true) {
	   f << "<p>" << "Всего тестов: "<< k << "</p>";
	   f.close();
	}
	return Error_NoError;
}

unsigned int __stdcall Drv_SetScaleU(double val)
{
	str = ":CHANnel"+IntToStr(NK)+":SCALe " + MyFloatToStr(val) + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	myscaleU = val;
	err = viPrintf(vi, ":CHANnel1:SCALe?\n");
	if (err != Error_NoError)
		return err;
	err = viScanf(vi, "%t", &buf[0]);
	if (err != Error_NoError)
		return err;
//	*scale = atof(&buf[0]);
	scaleu = atof(&buf[0]);
	err = viPrintf(vi, ":CHANnel1:OFFSet 0\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":CHANnel1:OFFSet?\n");
	if (err != Error_NoError)
		return err;
	return Error_NoError;
}

unsigned int __stdcall Drv_SetScaleT(double val) {
	// Горизонтальная настройка
	str = ":TIMebase:SCALe " + MyFloatToStr(val) + "\n"; // norm 0.000002
	err = viPrintf(vi, str.c_str());
	if (err != Error_NoError)
		return err;
	myscaleT = val;
	err = viPrintf(vi, ":TIMebase:SCALe?\n");
	if (err != Error_NoError)
		return err;
	err = viScanf(vi, "%t", &buf[0]);
	if (err != Error_NoError)
		return err;
	//*scale = atof(&buf[0]);
	scalet = atof(&buf[0]);
	err = viPrintf(vi, ":TIMebase:POSition 0.00\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":TIMebase:POSition?\n");
	if (err != Error_NoError)
		return err;
	return Error_NoError;
}

unsigned int __stdcall Drv_ReadTriggerStatus(bool *state)
{
	err = viPrintf(vi, ":OPERegister:CONDition?\n");
	if (err != Error_NoError)
		return err;
	err = viScanf(vi, "%t", &buf[0]);
	if (err != Error_NoError)
		return err;
	str = atof(&buf[0]);
	if ((str == 544) || (str == 32)) // стоп
	{
		*state = 1;
	}
	if ((str == 442)|| (str == 40)) // не стоп
	{
		*state = 0;
	}
	return Error_NoError;
}

unsigned int __stdcall Drv_SetMode(unsigned int param)
{
	if (param == Agilent_Single)
	{
		err = viPrintf(vi, ":SINGle\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_Auto)
	{
		err = viPrintf(vi, ":TRIGger:SWEep AUTO\n");
		if (err != Error_NoError)
			return err;
	}
	if (param == Agilent_Normal)
	{
		err = viPrintf(vi, ":TRIGger:SWEep NORMal\n");
		if (err != Error_NoError)
			return err;
	}
	return Error_NoError;

}

unsigned int __stdcall Drv_WaitTrigger(double time, bool *state) {
	err = viPrintf(vi, ":MTESt:COUNt:TIME?\n");
	if (err != Error_NoError)
		return err;
	str = atof(&buf[0]);
	if (str >= time)
	{
		err = viPrintf(vi, ":OPERegister:CONDition?\n");
		if (err != Error_NoError)
			return err;
		err = viScanf(vi, "%t", &buf[0]);
		if (err != Error_NoError)
			return err;
		str = atof(&buf[0]);
		if ((str == 552) || (str == 32)) // стоп
		{
			*state = true;
		}
		if ((str == 442)|| (str == 40)) // не стоп
		{
			*state = false;
		}
	}
	return Error_NoError;
}

unsigned int __stdcall Drv_GetWaveDataFun()
{
	err = viPrintf(vi, ":WAVeform:FORMat WORD\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":WAVeform:BYTeorder MSBFirst\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":WAVeform:UNSigned\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":ACQuire:TYPE NORMal\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":WAVeform:POINts:MODE Normal\n");
	if (err != Error_NoError)
		return err;
	err = viPrintf(vi, ":WAVeform:POINts 1000\n");
	if (err != Error_NoError)
		return err;
}

unsigned int __stdcall Drv_GetWaveDataSize(unsigned int  *size)
{
	double *val, mydata;
	err = viPrintf(vi, ":WAVeform:DATA?\n");
	if (err != Error_NoError)
			return err;
	memset(buf,0,10);
	unsigned long retCount = 0;
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 2, &retCount);
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 8, &retCount);
	*size  = atoi(&buf[0]);
	return Error_NoError;
}

unsigned int __stdcall Drv_GetWaveDataParam(double *xreference, double *xorigin, double *xincrement,
double *yreference, double *yorigin, double *yincrement)
{
	//XREFerence
	err = viPrintf(vi, ":WAVeform:XREFerence?\n");
	if (err != Error_NoError)
			return err;
	err = viScanf (vi, "%t", &buf[0]);
	if (err != Error_NoError)
			return err;
	str = buf;
	*xreference = atof(&buf[0]);
	// XORigin
	err = viPrintf(vi, ":WAVeform:XORigin?\n");
	if (err != Error_NoError)
			return err;
	err = viScanf (vi, "%t", &buf[0]);
	if (err != Error_NoError)
			return err;
	str = buf;
	*xorigin = atof(&buf[0]);
	// XINCrement?
	err = viPrintf(vi, ":WAVeform:XINCrement?\n");
	if (err != Error_NoError)
			return err;
	err = viScanf (vi, "%t", &buf[0]);
	if (err != Error_NoError)
			return err;
	str = buf;
	*xincrement = atof(&buf[0]);
	//YREFerence
	err = viPrintf(vi, ":WAVeform:YREFerence?\n");
	if (err != Error_NoError)
			return err;
	err = viScanf (vi, "%t", &buf[0]);
	if (err != Error_NoError)
			return err;
	str = buf;
	*yreference = atof(&buf[0]);
	//YORigin
	err = viPrintf(vi, ":WAVeform:YORigin?\n");
	if (err != Error_NoError)
			return err;
	err = viScanf (vi, "%t", &buf[0]);
	str = buf;
	*yorigin = atof(&buf[0]);
	//YINCrement
	err = viPrintf(vi, ":WAVeform:YINCrement?\n");
	if (err != Error_NoError)
			return err;
	err = viScanf (vi, "%t", &buf[0]);
	if (err != Error_NoError)
			return err;
	str = buf;
	*yincrement = atof(&buf[0]);

	return Error_NoError;
}

unsigned int __stdcall Drv_GetWaveData(char *mass, unsigned int size)
{
    unsigned long retCount;
	memset(mass,0,size);
	err = viRead(vi, (unsigned char *)( &mass[0]), size, &retCount);
	if ((err != Error_NoError)&&(err!=0x3FFF0006))
		return err;
	return Error_NoError;
}

unsigned int __stdcall Drv_SaveWaveData(unsigned int fmt, char * pfilename)
{

	if (fmt == Agilent_SaveBmp)
	{
	   format = 0;
	}
	else
	if (fmt == Agilent_SaveBin)
	{
	   format = 1;
	}

	return Error_NoError;
}

unsigned int __stdcall Drv_Reset()
{
   err = viPrintf(vi, "*RST\n");
   if (err != Error_NoError)
			return err;
   return Error_NoError;
}
////////////////////////////////////////////////////////////////////////////////
// --- прокладывание связей с dll уровня скриптов
unsigned int length(char *str)
{
	unsigned int i = 0;
	while ((str[i]!='\0')&&(i<=255))
		i++;
	return i;
}

unsigned int Max(unsigned int a, unsigned int b)
{
	if (a>b)
		return a;
	else
		return b;
}

unsigned int __stdcall  Spo_Drv_CreateScriptParametrs
	(bool * bNeedScript, unsigned int * iMaxNeedScript, unsigned int * iLengthDrvName,
	unsigned int * iLengthMyScriptName) {
	try {
		*bNeedScript = false; // функции скриптов не нужны
		*iMaxNeedScript = 0;
		*iLengthDrvName = DrvName_SIZE + 1;
		*iLengthMyScriptName = MyScriptName_SIZE + 1;
	}
	catch(...) {
		return UnknowError;
	}
	return NoErrors;
}

unsigned int __stdcall  Spo_Drv_GetScriptParametrs
	(wchar_t * cNeedScriptName, wchar_t * cDrvName, wchar_t * cMyScriptName) {
	unsigned int i;

	try {
		if (MyScriptName_SIZE > 0)
			for (unsigned int i = 0; i < MyScriptName_SIZE; ++i)
				cMyScriptName[i] = MyScriptName_[i];
		cMyScriptName[MyScriptName_SIZE] = '\0';
		if (DrvName_SIZE > 0)
			for (unsigned int i = 0; i < DrvName_SIZE; ++i)
				cDrvName[i] = DrvName_[i];
		cDrvName[DrvName_SIZE] = '\0';
		return NoErrors;
	}
	catch(...) {
		return UnknowError;
	}
}

// --- поиск устройства
unsigned int __stdcall  Spo_Drv_FindDevice(unsigned int * iCount,
	unsigned int  *iParamLevels, unsigned int * iMaxParamName)
{
	try
	{
		if (!(myvisa_init()))
			return ErrorLoadLybrary;

		err = viOpenDefaultRM(&defaultRM);
		if (err != Error_NoError)
			return err;
		err = viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR",
			VI_NULL, VI_NULL, &vi);
		if (err != NoErrors)
		{
			iCountFindDevice = 0;
			viClose(vi);
			viClose(defaultRM);
		}
		else
			iCountFindDevice = 1;
		* iCount = iCountFindDevice;  //число устройств
		* iParamLevels = 1; //уровень параметров
		iMaxFindDeviceParamName = 0;
		if (*iCount>0)
		{
			//выделение памяти, каждая строка с названием не больше 255 символов
			for (unsigned int __stdcall i = 0; i < 255; i++) {
				if (ExistAGILENTList[i] == NULL)
					ExistAGILENTList[i] = new char[255];
			}
			//информация об устройствах
			for(unsigned int __stdcall i = 0;i<*iCount;++i)
			{
				//собираю информацию об устройстве
//				ExistAGILENTList[i] = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
				ExistAGILENTList[i] = "USB0";
				iMaxFindDeviceParamName = Max(iMaxFindDeviceParamName, length(ExistAGILENTList[i]));
			}
		}
		* iMaxParamName = ++iMaxFindDeviceParamName;
	}
	catch(...) {
		return UnknowError;
	}

	return Error_NoError;
}

// --- выдача информации об устройствах
unsigned int __stdcall  Spo_Drv_GetDeviceInfo(wchar_t * cParam) {
	unsigned int __stdcall i, j;
	unsigned int __stdcall l, p;
	try {
		if (iCountFindDevice <= 0)
			return NoErrors;
		l = 0;
		p = 0;
		for (unsigned int __stdcall i = 0; i < iCountFindDevice; ++i) {
			unsigned int __stdcall lengthExistAGILENTListCur = length(ExistAGILENTList[i]);
			if (lengthExistAGILENTListCur > 0)
				for (unsigned int __stdcall j = 0; j < lengthExistAGILENTListCur; ++j)
					cParam[j + p] = ExistAGILENTList[i][j];
			cParam[lengthExistAGILENTListCur + p] = '\0';
			l++;
			p = p + iMaxFindDeviceParamName;
		}
		return NoErrors;
	}
	catch(...) {
		return UnknowError;
	}
}

// --- инициализация устройства тут менять
unsigned int __stdcall  Spo_Drv_DeviceOpen(unsigned int  iNum, unsigned int * iDeviceHandle)
{
	try {
		if (!(myvisa_init()))
			return ErrorLoadLybrary;
        *iDeviceHandle = iNum;
		err = viOpenDefaultRM(&defaultRM);
		if (err != Error_NoError)
			return err;
//		err = viOpen(defaultRM, ExistAGILENTList[iNum],
//			VI_NULL, VI_NULL, &vi);
		err = viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR",
			VI_NULL, VI_NULL, &vi);
		if (err != Error_NoError)
			return err;
	}
	catch(...) {
		return UnknowError;
	}
	return Error_NoError;
}

// --- завершение работы тут менять
unsigned int __stdcall  Spo_Drv_DeviceClose() {
	try {
		viClose(vi);
		viClose(defaultRM);
	}
	catch(...) {
	}
	return Error_NoError;
}

