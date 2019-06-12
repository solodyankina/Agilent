//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#pragma hdrstop

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include "myvisa.h"
#include "mytime.h"


#include "test_oscil.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "Chart.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include "Series.hpp"
#include "TeeFunci.hpp"
#include <Dialogs.hpp>
#pragma resource "*.dfm"
TForm1 *Form1;

fviOpenDefaultRM viOpenDefaultRM;
fviOpen viOpen;
fviPrintf viPrintf;
fviScanf viScanf;
fviClose viClose;
fviWrite viWrite;
fviRead viRead;

int myvisa_init(void) {
	HINSTANCE visa32dll;

	visa32dll = NULL;
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
		if (!(viWrite = (fviWrite)GetProcAddress(visa32dll, "viWrite")))
			return 0;
		if (!(viRead = (fviRead)GetProcAddress(visa32dll, "viRead")))
			return 0;
	}
	else
		return 0; // false
	return 1; // ok
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
//  ��� ���������� �� ����� ������
	unsigned int err;
	ViRsrc name = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
	ViSession defaultRM, vi;
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;


	myvisa_init();

	// Open session to USB device at address
	err = viOpenDefaultRM(&defaultRM);
	if (err != 0)
	{
		RichEdit1->Lines->Add("������ ������ ������� viOpenDefaultRM. �� ������� ������� ������");
		return ;
	}
	else
	RichEdit1->Lines->Add("������ ������� �������");
	Application->ProcessMessages();

	err = viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	if (err!=0)
	{
		RichEdit1->Lines->Add("������ ������ ������� viOpen. �� ������� ���������� ����� � �����������. �������� �����������");
		return ;
	}
	else
	RichEdit1->Lines->Add("����� � USB ������� ������������");
	Application->ProcessMessages();

	//����������� �����
	err = viPrintf(vi, ":STATUS? CHANEL1\n");
	if (err!=0)
	{
		RichEdit1->Lines->Add("�� ������� ���������� ����� � ��������� �������. �������� �����������");
		return ;
	}
	else
	RichEdit1->Lines->Add("����� � ������� ������������");
	Application->ProcessMessages();

	//����� ������
	err = viPrintf(vi, ":VIEW CHANnel1\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������������ � ��������� ������. �������� �����������");
		return ;
	}
	else
	RichEdit1->Lines->Add("����������� � ������ ������������");
	Application->ProcessMessages();

	// �������� ����� ���������
	err = viPrintf(vi, ":TRIGger:SWEep NORMal\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ��������� ���������� ����� ���������");
		return ;
	}
	else
	RichEdit1->Lines->Add("������ ���������� ����� ���������");
	Application->ProcessMessages();

	// ����� �������
/*	str = ":SBUS1:M1553:TRIGger:TYPE " + ComboBox1->Text + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������� :SBUS<n>:M1553:TRIGger:TYPE");
		return;
	}
	viPrintf(vi, ":SBUS<n>:M1553:TRIGger:TYPE?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "����� �������: " + str;
	RichEdit1->Lines->Add(str);
	Application->ProcessMessages();    */

	// ������������� ���������
	err = viPrintf(vi, ":MEASure:SOURce CHANnel1\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������������ � ��������� ������. �������� �����������");
		return ;
	}

	// ���������
	err = viPrintf(vi, ":MEASure:VAMPlitude[1]\n");
	if(err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������� ���������");
		return;
	}
	viPrintf(vi, ":MEASure:VAMPlitude?\n");
	// Read results
	viScanf (vi, "%t", &buf[0]);
	// Print results
		/*DWORDLONG ct[201];
		ct[0] = myGetMKSTicks();
		for(int k=1; k<201; ++k)
			ct[k] = 0;
		for(int k=1; k<201; k++)
		{
			viPrintf(vi, ":MEASure:RESults?\n" );
			viScanf (vi, "%t", &buf[0]) ;
			ct[k] =  myGetMKSTicks();
		}
		AnsiString amp;
		for (int k=1; k<201; ++k)
		{
		   amp = (IntToStr(int(ct[k] - ct[k-1]))) ;
		   Edit5->Text = amp;
		   Application->ProcessMessages();
		} */
	str = buf;
	Edit1->Text = str;
	Application->ProcessMessages();

	// ������������ ����� ������
	err = viPrintf(vi, ":MEASure:FALLtime[1]\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������� ���� ������");
		return;
	}
	viPrintf(vi, ":MEASure:FALLtime?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	Edit4->Text = str;
	Application->ProcessMessages();
	// ������������ ���������� ������
	err = viPrintf(vi, ":MEASure:RISetime[1]\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������� ���������� ������");
		return ;
	}
	viPrintf(vi, ":MEASure:RISetime?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	Edit3->Text = str;
	Application->ProcessMessages();

	// ����������
	err = viPrintf(vi, ":MEASure:OVERshoot[CHANnel1]\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������� ����������");
		return ;
	}
	viPrintf(vi, ":MEASure:OVERshoot?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	Edit2->Text = str;
	Application->ProcessMessages();

  /*	err = viPrintf(vi, ":MEASure:RESults?\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("err :MEASure:RESults");
		return ;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "res: " + str;
	RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	err = viPrintf(vi, ":MEASure:STATistics 1\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� ������ ����������");
		return ;
	}
	viPrintf(vi, ":MEASure:STATistics?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "stat: " + str;
	RichEdit1->Lines->Add(str);
	Application->ProcessMessages();     */

   	// ������������ ���������
	err = viPrintf(vi, ":CHANnel1:SCALe 5[V]\n");
	if(err != 0)
	{
		RichEdit1->Lines->Add("err :CHANnel1:SCALe");
		return;
	}
	viPrintf(vi, ":CHANnel1:SCALe?\n");
	err = viPrintf(vi, ":CHANnel1:OFFSet 0\n");
	if(err != 0)
	{
		RichEdit1->Lines->Add("err :CHANnel1:OFFSet");
		return;
	}
	viPrintf(vi, ":CHANnel1:OFFSet?\n");
	//�������������� ���������
	err = viPrintf(vi, ":TIMebase:SCALe 0.000002\n");
	if(err != 0)
	{
		RichEdit1->Lines->Add("err :TIMebase:SCALe");
		return;
	}
	viPrintf(vi, ":TIMebase:SCALe?\n");
	str = ":TIMebase:POSition 0.00\n";
	err = viPrintf(vi, str.c_str());
	if(err != 0)
	{
		RichEdit1->Lines->Add("err :TIMebase:POSition?");
		return;
	}
	viPrintf(vi, ":TIMebase:POSition?\n");

	err = viPrintf(vi, ":ACQuire:TYPE NORMal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts:MODE Normal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts mode");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts 1000\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}
	viPrintf(vi, ":WAVeform:POINts?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "POINts " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//XREFerence
	err = viPrintf(vi, ":WAVeform:XREFerence?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xreference");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xreference = atof(&buf[0]);
	str = "xreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	// XORigin
	err = viPrintf(vi, ":WAVeform:XORigin?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xorigin = atof(&buf[0]);
	str = "xorigin: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	// XINCrement?
	err = viPrintf(vi, ":WAVeform:XINCrement?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xincrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xincrement = atof(&buf[0]);
	str = "xincrement: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YREFerence
	err = viPrintf(vi, ":WAVeform:YREFerence?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yrefenence");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yreference = atof(&buf[0]);
	str = "yreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YORigin
	err = viPrintf(vi, ":WAVeform:YORigin?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yorigin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yorigin = atof(&buf[0]);
	str  = "yorigin : " + str ;
	Form1->RichEdit1->Lines->Add(str );
	Application->ProcessMessages();
	//YINCrement
	err = viPrintf(vi, ":WAVeform:YINCrement?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� YINCrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yincrement = atof(&buf[0]);
	str  = "yincrement : " + str ;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();


	//Format
	err = viPrintf(vi, ":WAVeform:FORMat WORD\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:FORMat word");
		return;
	}
	viPrintf(vi, ":WAVeform:FORMat?\n");
	//BYTeorder
	err = viPrintf(vi, ":WAVeform:BYTeorder MSBFirst\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:BYTeorder");
		return;
	}
	viPrintf(vi, ":WAVeform:BYTeorder?\n");
	 //UNSigned
	err = viPrintf(vi, ":WAVeform:UNSigned 1\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:UNSigned ");
		return;
	}
	viPrintf(vi, ":WAVeform:UNSigned?\n");

	viPrintf(vi, ":WAVeform?\n");  // data
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "nastr: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	//DATA
	err = viPrintf(vi, ":WAVeform:DATA?\n");  // data
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:DATA?");
		return;
	}
	unsigned long retCount = 0;
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 2, &retCount);
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 8, &retCount);
	points_count = atoi(&buf[0]);
	char *buf2;
	buf2 = new char [points_count];
	memset(buf2,0,points_count);
	viRead(vi, (unsigned char *)( &buf2[0]), points_count, &retCount);
	Form1->Series1->Clear();
	for (int i = 0; i < retCount/2; i++)
	{
		WORD w = buf2[i*2]*0x100 + buf2[i*2+1];
		short dt = * ((short * )(&w));
		if (dt> 0 )
		{
			Y = -(abs(dt - yreference)*yincrement)+yorigin;
		}
		else
		{
			Y = (abs(-dt - yreference)*yincrement)+yorigin;
		}
		X = ((i - xreference)*xincrement)+xorigin;
		Form1->Series1->AddXY(X,Y,FloatToStr(X),clRed);
	}
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 10, &retCount);

 /*	// ����� ����������
	err = viPrintf(vi, ":MEASure:STATistics:RESet\n");
	if (err != 0)
	{
		RichEdit1->Lines->Add("�� ������� �������� ����������");
		return ;
	}
	// ������� ���� ���������� ����������
	AnsiString ResultsTypeArray[7];
	ResultsTypeArray[0] = "CURRent";
	ResultsTypeArray[1] = "MINimum";
	ResultsTypeArray[2] = "MAXimum";
	ResultsTypeArray[3] = "MEAN";
	ResultsTypeArray[4] = "STDDev";
	ResultsTypeArray[5] = "COUNt";
	ResultsTypeArray[6] = "ON"; // All results.

	AnsiString ValueColumnArray[7];
	ValueColumnArray[0] = "Meas_Lbl";
	ValueColumnArray[1] = "Current";
	ValueColumnArray[2] = "Min";
	ValueColumnArray[3] = "Max";
	ValueColumnArray[4] = "Mean";
	ValueColumnArray[5] = "Std_Dev";
	ValueColumnArray[6] = "Count";


	for (int i=6; i<7; i++)
	{
		str=":MEASure:STATistics " + ResultsTypeArray[i]+"\n";
		err = viPrintf(vi, str.c_str());
		// �������� ���������� ����������
		int intCounter;
		intCounter = 0;
		memset(&buf[0],0,256);

		DWORDLONG ct[201];
		ct[0] = myGetMKSTicks();
		for(int k=1; k<201; ++k)
        	ct[k] = 0;
		for(int k=1; k<201; k++)
		{
			viPrintf(vi, ":MEASure:RESults?\n" );
			viScanf (vi, "%t", &buf[0]) ;
			ct[k] =  myGetMKSTicks();
		}
		for (int k=1; k<201; ++k)
		{

		   RichEdit1->Lines->Add(IntToStr(int(ct[k] - ct[k-1])));
		   Application->ProcessMessages();
		}



		str = buf;
		str = ResultsTypeArray[i]+": " + str;
		RichEdit1->Lines->Add(str);
		Application->ProcessMessages();

*/
	   /*
		//For Measurement In MeasurementArray
			if (CheckBox1->Checked)
			{
				for (int i=0; i<7; i++)
				{
					//if (intCounter <> vbString)
					{
						err = viPrintf(vi, ":MEASure:RESults?\n" );
						memset(&buf[0],0,256);
						viScanf (vi, "%t", &buf[0]) ;
						str = buf;
						str = ResultsTypeArray[i]+": " + str;
						RichEdit1->Lines->Add(str);
						Application->ProcessMessages();
					}
					else // Result is a string (e.g., measurement label).
					{
						err=viPrintf(vi, ":MEASure:RESults?\n" );
						//RichEdit1->Lines->Add(intCounter);
					}
				}
			}
			else //Specific statistic (e.g., Current, Max, Min, etc.).
			{
				err = viPrintf(vi, ":MEASure:RESults?\n" );
				memset(&buf[0],0,256);
				viScanf (vi, "%t", &buf[0]) ;
				str = buf;
				str = ResultsTypeArray[i]+": " + str;
				RichEdit1->Lines->Add(str);
				Application->ProcessMessages();
			}


	}
	*/
	// Close session
	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------

AnsiString MyFloatToStr(double i)
{
	AnsiString str = FloatToStr(i);
	//������ ������� �� �����
	int k = str.Pos(",");
	if (k>0)
		str[k] = '.';
	return str;
}

void __fastcall TForm1::Button2Click(TObject *Sender)
{
	//  ��� ���������� �� ����� ������
	unsigned int err;
	ViRsrc name = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
	ViSession defaultRM, vi;
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;

	myvisa_init();

	// Open session to USB device at address
	viOpenDefaultRM(&defaultRM);
	viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	//����������� �����
	viPrintf(vi, ":STATUS? CHANEL1\n");
	//����� ������
	viPrintf(vi, ":VIEW CHANnel1\n");

	//�������������� ���������
	for (float i = 0; i < 0.0000192; i = i + 0.0000001)
	{
	   str = MyFloatToStr(i);
	   str = ":TIMebase:POSition " + FloatToStr(i) + "\n";
	   int k = str.Pos(",");
	   if (k>0)
	   str[k] = '.';
	   //return str;
	   err = viPrintf(vi, str.c_str());
	   if(err != 0)
	   {
			RichEdit1->Lines->Add("err :TIMebase:POSition?");
			return;
	   }
	   viPrintf(vi, ":TIMebase:POSition?\n");
	}

	err = viPrintf(vi, ":ACQuire:TYPE NORMal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts:MODE Normal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts mode");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts 1000\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}
	viPrintf(vi, ":WAVeform:POINts?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "POINts " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//XREFerence
	err = viPrintf(vi, ":WAVeform:XREFerence?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xreference");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xreference = atof(&buf[0]);
	str = "xreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	// XORigin
	err = viPrintf(vi, ":WAVeform:XORigin?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xorigin = atof(&buf[0]);
	str = "xorigin: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	// XINCrement?
	err = viPrintf(vi, ":WAVeform:XINCrement?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xincrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xincrement = atof(&buf[0]);
	str = "xincrement: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YREFerence
	err = viPrintf(vi, ":WAVeform:YREFerence?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yrefenence");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yreference = atof(&buf[0]);
	str = "yreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YORigin
	err = viPrintf(vi, ":WAVeform:YORigin?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yorigin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yorigin = atof(&buf[0]);
	str  = "yorigin : " + str ;
	Form1->RichEdit1->Lines->Add(str );
	Application->ProcessMessages();
	//YINCrement
	err = viPrintf(vi, ":WAVeform:YINCrement?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� YINCrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yincrement = atof(&buf[0]);
	str  = "yincrement : " + str ;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();


	//Format
	err = viPrintf(vi, ":WAVeform:FORMat WORD\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:FORMat word");
		return;
	}
	viPrintf(vi, ":WAVeform:FORMat?\n");
	//BYTeorder
	err = viPrintf(vi, ":WAVeform:BYTeorder MSBFirst\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:BYTeorder");
		return;
	}
	viPrintf(vi, ":WAVeform:BYTeorder?\n");
	 //UNSigned
	err = viPrintf(vi, ":WAVeform:UNSigned 1\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:UNSigned ");
		return;
	}
	viPrintf(vi, ":WAVeform:UNSigned?\n");

	viPrintf(vi, ":WAVeform?\n");  // data
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "nastr: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	//DATA
	err = viPrintf(vi, ":WAVeform:DATA?\n");  // data
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:DATA?");
		return;
	}
	unsigned long retCount = 0;
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 2, &retCount);
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 8, &retCount);
	points_count = atoi(&buf[0]);
	char *buf2;
	buf2 = new char [points_count];
	memset(buf2,0,points_count);
	viRead(vi, (unsigned char *)( &buf2[0]), points_count, &retCount);
	Form1->Series1->Clear();
	for (int i = 0; i < retCount/2; i++)
	{
		WORD w = buf2[i*2]*0x100 + buf2[i*2+1];
		short dt = * ((short * )(&w));
		if (dt> 0 )
		{
			Y = -(abs(dt - yreference)*yincrement)+yorigin;
		}
		else
		{
			Y = (abs(-dt - yreference)*yincrement)+yorigin;
		}
		X = ((i - xreference)*xincrement)+xorigin;
		Form1->Series1->AddXY(X,Y,FloatToStr(X),clRed);
	}
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 10, &retCount);

	viClose (vi);
	viClose (defaultRM);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
//  ��� ���������� �� ����� ������
	unsigned int err;
	ViRsrc name = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
	ViSession defaultRM, vi;
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;

	myvisa_init();

	// Open session to USB device at address
	viOpenDefaultRM(&defaultRM);
	viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	//����������� �����
	viPrintf(vi, ":STATUS? CHANEL1\n");
	//����� ������
	viPrintf(vi, ":VIEW CHANnel1\n");

	//�������������� ���������
	for (float i = 0.0000192; i < 0.00003; i = i + 0.0000001)
	{
	   str = MyFloatToStr(i);
	   str = ":TIMebase:POSition " + FloatToStr(i) + "\n";
	   int k = str.Pos(",");
	   if (k>0)
	   str[k] = '.';
	   //return str;
	   err = viPrintf(vi, str.c_str());
	   if(err != 0)
	   {
			RichEdit1->Lines->Add("err :TIMebase:POSition?");
			return;
	   }
	   viPrintf(vi, ":TIMebase:POSition?\n");
	}

	err = viPrintf(vi, ":ACQuire:TYPE NORMal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts:MODE Normal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts mode");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts 1000\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}
	viPrintf(vi, ":WAVeform:POINts?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "POINts " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//XREFerence
	err = viPrintf(vi, ":WAVeform:XREFerence?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xreference");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xreference = atof(&buf[0]);
	str = "xreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	// XORigin
	err = viPrintf(vi, ":WAVeform:XORigin?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xorigin = atof(&buf[0]);
	str = "xorigin: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	// XINCrement?
	err = viPrintf(vi, ":WAVeform:XINCrement?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xincrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xincrement = atof(&buf[0]);
	str = "xincrement: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YREFerence
	err = viPrintf(vi, ":WAVeform:YREFerence?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yrefenence");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yreference = atof(&buf[0]);
	str = "yreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YORigin
	err = viPrintf(vi, ":WAVeform:YORigin?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yorigin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yorigin = atof(&buf[0]);
	str  = "yorigin : " + str ;
	Form1->RichEdit1->Lines->Add(str );
	Application->ProcessMessages();
	//YINCrement
	err = viPrintf(vi, ":WAVeform:YINCrement?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� YINCrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yincrement = atof(&buf[0]);
	str  = "yincrement : " + str ;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();


	//Format
	err = viPrintf(vi, ":WAVeform:FORMat WORD\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:FORMat word");
		return;
	}
	viPrintf(vi, ":WAVeform:FORMat?\n");
	//BYTeorder
	err = viPrintf(vi, ":WAVeform:BYTeorder MSBFirst\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:BYTeorder");
		return;
	}
	viPrintf(vi, ":WAVeform:BYTeorder?\n");
	 //UNSigned
	err = viPrintf(vi, ":WAVeform:UNSigned 1\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:UNSigned ");
		return;
	}
	viPrintf(vi, ":WAVeform:UNSigned?\n");

	viPrintf(vi, ":WAVeform?\n");  // data
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "nastr: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	//DATA
	err = viPrintf(vi, ":WAVeform:DATA?\n");  // data
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:DATA?");
		return;
	}
	unsigned long retCount = 0;
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 2, &retCount);
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 8, &retCount);
	points_count = atoi(&buf[0]);
	char *buf2;
	buf2 = new char [points_count];
	memset(buf2,0,points_count);
	viRead(vi, (unsigned char *)( &buf2[0]), points_count, &retCount);
	Form1->Series1->Clear();
	for (int i = 0; i < retCount/2; i++)
	{
		WORD w = buf2[i*2]*0x100 + buf2[i*2+1];
		short dt = * ((short * )(&w));
		if (dt> 0 )
		{
			Y = -(abs(dt - yreference)*yincrement)+yorigin;
		}
		else
		{
			Y = (abs(-dt - yreference)*yincrement)+yorigin;
		}
		X = ((i - xreference)*xincrement)+xorigin;
		Form1->Series1->AddXY(X,Y,FloatToStr(X),clRed);
	}
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 10, &retCount);

	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
	unsigned int err;
	ViRsrc name = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
	ViSession defaultRM, vi;
	char buf [256] = {0};
	AnsiString str;

	myvisa_init();

	// Open session to USB device at address
	viOpenDefaultRM(&defaultRM);
	viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	//����������� �����
	viPrintf(vi, ":STATUS? CHANEL1\n");
	//����� ������
	viPrintf(vi, ":VIEW CHANnel1\n");

	// ������������ � ������
	err = viPrintf(vi, ":MTESt:AMASk:SOURce CHANnel1\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("�� ������� ������������ � ������ CHANnel1");
		return;
	}

	// X
	err = viPrintf(vi, ":MTESt:AMASk:XDELta 0.05\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}
	viPrintf(vi, ":MTESt:AMASk:XDELta?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;

	// Y
	err = viPrintf(vi, ":MTESt:AMASk:YDELta 0.05\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}
	viPrintf(vi, ":MTESt:AMASk:YDELta?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;

	// create mask
	err = viPrintf(vi, ":MTESt:AMASk:CREate\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}

	viClose (vi);
	viClose (defaultRM);
}

//---------------------------------------------------------------------------


void __fastcall TForm1::Button5Click(TObject *Sender)
{
	unsigned int err;
	ViRsrc name = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
	ViSession defaultRM, vi;
	char buf [256] = {0};
	AnsiString str;

	myvisa_init();

	// Open session to USB device at address
	viOpenDefaultRM(&defaultRM);
	viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	//����������� �����
	viPrintf(vi, ":STATUS? CHANEL1\n");
	//����� ������
	viPrintf(vi, ":VIEW CHANnel1\n");

	err = viPrintf(vi, ":MTESt:DELete\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("err fail_wave");
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)
{
	unsigned int err;
	ViRsrc name = "USB0::0x0957::0x17A4::MY53280597::0::INSTR";
	ViSession defaultRM, vi;
	char buf [256] = {0};
	AnsiString str;
	double amplitude,fulltime,restime,overshoot,failwaveform,count_test, timemask,datas,times;
	ofstream f;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;

	myvisa_init();

	// Open session to USB device at address
	viOpenDefaultRM(&defaultRM);
	viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	//����������� �����
	viPrintf(vi, ":STATUS? CHANEL1\n");
	//����� ������
	viPrintf(vi, ":VIEW CHANnel1\n");
	RichEdit2->Lines->Add("����� ������� ������������");

	// ��������� ��� ������
	err = viPrintf(vi, ":MTESt:RMODe:FACTion:STOP 1\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("FACTion:STOP");
		return;
	}

	for (int i = 0; i < 10000; i++)
	{

	// ����  STOP
	err = viPrintf(vi, ":OPERegister:CONDition?\n");
	if(err != 0)
	{
		RichEdit2->Lines->Add("�� ������� ������������ � ������ CHANnel1");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	if (str == "+544\n")
	{
		memset(buf,0,0);
		// ����
		err = viPrintf(vi, ":SYSTem:DATE?\n");
		memset(buf,0,0);
		if(err != 0)
		{
			RichEdit2->Lines->Add(":SYSTem:DATE?");
			return;
		}
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		datas = atof(&buf[0]);
		memset(buf,0,0);
		// time
		err = viPrintf(vi, ":SYSTem:TIME? hours,minutes,seconds\n");
		memset(buf,0,0);
		if(err != 0)
		{
			RichEdit2->Lines->Add("err hours,minutes,seconds");
			return;
		}
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		times = atof(&buf[0]);

		// ���������� fail waveform
		memset(buf,0,0);
		err = viPrintf(vi, ":MTESt:COUNt:FWAVeforms? [CHANnel1]\n");
		memset(buf,0,0);
		if(err != 0)
		{
			RichEdit2->Lines->Add("err fail_wave");
			return;
		}
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		failwaveform = atof(&buf[0]);
		// ����� ��������� ���������
		memset(buf,0,0);
		err = viPrintf(vi, ":MTESt:COUNt:WAVeforms?\n");
		if(err != 0)
		{
			RichEdit2->Lines->Add("err COUNt:WAVeforms?");
			return;
		}
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		count_test = atof(&buf[0]);
		// ����� �� ��������� �����
		memset(buf,0,0);
		err = viPrintf(vi, ":MTESt:COUNt:TIME?\n");
		if(err != 0)
		{
			RichEdit2->Lines->Add("err COUNt:time?");
			return;
		}
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		timemask = atof(&buf[0]);

		// ���������
		memset(buf,0,0);
		err = viPrintf(vi, ":MEASure:VAMPlitude[1]\n");
		if(err != 0)
		{
			RichEdit1->Lines->Add("�� ������� ������� ���������");
			return;
		}
		viPrintf(vi, ":MEASure:VAMPlitude?\n");
		// Read results
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		amplitude = atof(&buf[0]);
		// ������������ ����� ������
		memset(buf,0,0);
		err = viPrintf(vi, ":MEASure:FALLtime[1]\n");
		if (err != 0)
		{
			RichEdit1->Lines->Add("�� ������� ������� ���� ������");
			return;
		}
		viPrintf(vi, ":MEASure:FALLtime?\n");
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		fulltime = atof(&buf[0]);
		// ������������ ���������� ������
		memset(buf,0,0);
		err = viPrintf(vi, ":MEASure:RISetime[1]\n");
		if (err != 0)
		{
			RichEdit1->Lines->Add("�� ������� ������� ���������� ������");
			return ;
		}
		viPrintf(vi, ":MEASure:RISetime?\n");
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		restime = atof(&buf[0]);
		// ����������
		memset(buf,0,0);
		err = viPrintf(vi, ":MEASure:OVERshoot[CHANnel1]\n");
		if (err != 0)
		{
			RichEdit1->Lines->Add("�� ������� ������� ����������");
			return ;
		}
		viPrintf(vi, ":MEASure:OVERshoot?\n");
		viScanf (vi, "%t", &buf[0]);
		str = buf;
		overshoot = atof(&buf[0]);

 //----------------------------------------------------------------------------------
		err = viPrintf(vi, ":ACQuire:TYPE NORMal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts:MODE Normal\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts mode");
		return;
	}

	err = viPrintf(vi, ":WAVeform:POINts 1000\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:POINts");
		return;
	}
	viPrintf(vi, ":WAVeform:POINts?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "POINts " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//XREFerence
	err = viPrintf(vi, ":WAVeform:XREFerence?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xreference");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xreference = atof(&buf[0]);
	str = "xreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	// XORigin
	err = viPrintf(vi, ":WAVeform:XORigin?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xorogin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xorigin = atof(&buf[0]);
	str = "xorigin: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	// XINCrement?
	err = viPrintf(vi, ":WAVeform:XINCrement?\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Xincrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xincrement = atof(&buf[0]);
	str = "xincrement: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YREFerence
	err = viPrintf(vi, ":WAVeform:YREFerence?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yrefenence");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yreference = atof(&buf[0]);
	str = "yreference: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();
	//YORigin
	err = viPrintf(vi, ":WAVeform:YORigin?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� Yorigin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yorigin = atof(&buf[0]);
	str  = "yorigin : " + str ;
	Form1->RichEdit1->Lines->Add(str );
	Application->ProcessMessages();
	//YINCrement
	err = viPrintf(vi, ":WAVeform:YINCrement?\n");  // Y
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("�� ������� ������� ����� YINCrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yincrement = atof(&buf[0]);
	str  = "yincrement : " + str ;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();


	//Format
	err = viPrintf(vi, ":WAVeform:FORMat WORD\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:FORMat word");
		return;
	}
	viPrintf(vi, ":WAVeform:FORMat?\n");
	//BYTeorder
	err = viPrintf(vi, ":WAVeform:BYTeorder MSBFirst\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:BYTeorder");
		return;
	}
	viPrintf(vi, ":WAVeform:BYTeorder?\n");
	 //UNSigned
	err = viPrintf(vi, ":WAVeform:UNSigned 1\n");
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:UNSigned ");
		return;
	}
	viPrintf(vi, ":WAVeform:UNSigned?\n");

	viPrintf(vi, ":WAVeform?\n");  // data
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "nastr: " + str;
	Form1->RichEdit1->Lines->Add(str);
	Application->ProcessMessages();

	//DATA
	err = viPrintf(vi, ":WAVeform:DATA?\n");  // data
	if(err != 0)
	{
		Form1->RichEdit1->Lines->Add("err :WAVeform:DATA?");
		return;
	}
	unsigned long retCount = 0;
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 2, &retCount);
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 8, &retCount);
	points_count = atoi(&buf[0]);
	char *buf2;
	buf2 = new char [points_count];
	memset(buf2,0,points_count);
	viRead(vi, (unsigned char *)( &buf2[0]), points_count, &retCount);
	Form1->Series1->Clear();
	for (int i = 0; i < retCount/2; i++)
	{
		WORD w = buf2[i*2]*0x100 + buf2[i*2+1];
		short dt = * ((short * )(&w));
		if (dt> 0 )
		{
			Y = -(abs(dt - yreference)*yincrement)+yorigin;
		}
		else
		{
			Y = (abs(-dt - yreference)*yincrement)+yorigin;
		}
		X = ((i - xreference)*xincrement)+xorigin;
		Form1->Series1->AddXY(X,Y,FloatToStr(X),clRed);
	}
	memset(buf,0,10);
	viRead(vi, (unsigned char *)( &buf[0]), 10, &retCount);
//----------------------------------------------------------------------------------

		//RichEdit2->Lines->SaveToFile("C:\\Documents and Settings\\�������������\\������� ����\\TestOscil_norm\\err_wav.doc");
		Chart1->SaveToBitmapFile("C:\\Documents and Settings\\�������������\\������� ����\\TestOscil_norm\\report\\err_wav.bmp");
		SaveDialog1->Title = "save data";
		SaveDialog1->FileName = "err_data.html";
		if (SaveDialog1->Execute())
		{
			//RichEdit2->Lines->SaveToFile(SaveDialog1->FileName);
			str = "C:\\Documents and Settings\\�������������\\������� ����\\TestOscil_norm\\report\\err_data.html";
		f.open("C:\\Documents and Settings\\�������������\\������� ����\\TestOscil_norm\\report\\err_data.html");
		f<<"<head>"<<"\n"<<"<meta http-equiv=Content-Type content="<<"text/html; charset=windows-1251"<<"></head>";
		f<<"<p>"<<datas<<"</p>";
		f<<"<p>"<<times<<"</p>";
		f<<"<p>"<<"����� ���������� ���������:  "<<count_test<<"</p>";
		f<<"<Body>"<<"\n"<<"<p>"<<"���������� �����: "<<failwaveform<<"</p>";
		f<<"<p>"<<"���������� �������: "<<timemask<<"</p>";
		f<<"<p>"<<"���������: "<<amplitude<<"</p>";
		f<<"<p>"<<"����������: "<<overshoot<<"</p>";
		f<<"<p>"<<"���������� ������: "<<restime<<"</p>";
		f<<"<p>"<<"���� ������: "<<fulltime<<"</p>";
		f<<"<p align=center><img src="<<"err_wav.bmp" <<"></p>"<<"\n"<<"</body></html>";
		f.close();
		}

		ShellExecute(0, "open", "C:\\Documents and Settings\\�������������\\������� ����\\TestOscil_norm\\report\\err_data.html", NULL, NULL, SW_RESTORE);
	}
	}
	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------


