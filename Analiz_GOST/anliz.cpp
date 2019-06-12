//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "myvisa.h"
#include "start_work.h"

#include "anliz.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma link "Series"
#pragma resource "*.dfm"
TForm1 *Form1;


fviOpenDefaultRM viOpenDefaultRM;
fviOpen viOpen;
fviPrintf viPrintf;
fviScanf viScanf;
fviClose viClose;
fviWrite viWrite;
fviRead viRead;

unsigned int err;
ViSession defaultRM, vi;


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

void myprint(AnsiString str)
{
	Form2->RichEdit1->Lines->Add(str);
	Form2->ShowModal();
}

void open_ses()
{
	AnsiString str;
	myvisa_init();

	if (Form1->ComboBox1->Text == "")
	{
		ShowMessage("�� ������ �����");
		viClose (vi);
		viClose (defaultRM);
	}

	// Open session to USB device at address
	err = viOpenDefaultRM(&defaultRM);
	if (err != 0)
	{
		myprint("������ ������ ������� viOpenDefaultRM. �� ������� ������� ������");
		return ;
	}

	err = viOpen(defaultRM, "USB0::0x0957::0x17A4::MY53280597::0::INSTR", VI_NULL, VI_NULL, &vi);
	if (err!=0)
	{
		myprint("������ ������ ������� viOpen. �� ������� ���������� ����� � �����������. �������� �����������");
		return ;
	}

	//����������� �����

	str = ":STATUS? CHANEL" + Form1->ComboBox1->Text + "\n";
	err = viPrintf(vi, str.c_str());
	if (err!=0)
	{
		myprint("�� ������� ���������� ����� � ��������� �������. �������� �����������");
		return ;
	}

	//����� ������
	str = ":VIEW CHANnel" + Form1->ComboBox1->Text + "\n" ;
	err = viPrintf(vi, str.c_str());
	if (err != 0)
	{
		myprint("�� ������� ������������ � ��������� ������. �������� �����������");
		return ;
	}

}

void wave()
{
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;

	//:ACQuire:TYPE
	err = viPrintf(vi, ":ACQuire:TYPE NORMal\n");
	if(err != 0)
	{
		myprint("err :WAVeform:POINts");
		return;
	}
	// POINts:MODE
	err = viPrintf(vi, ":WAVeform:POINts:MODE Normal\n");
	if(err != 0)
	{
		myprint("err :WAVeform:POINts");
		return;
	}
	// points
	err = viPrintf(vi, ":WAVeform:POINts 1000\n");
	if(err != 0)
	{
		myprint("err :WAVeform:POINts");
		return;
	}
	viPrintf(vi, ":WAVeform:POINts?\n");

	//XREFerence
	err = viPrintf(vi, ":WAVeform:XREFerence?\n");
	if(err != 0)
	{
		myprint("�� ������� ������� ����� Xreference");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xreference = atof(&buf[0]);
	str = "xreference: " + str;
	//myprint(str);
	// XORigin
	err = viPrintf(vi, ":WAVeform:XORigin?\n");
	if(err != 0)
	{
		myprint("�� ������� ������� ����� Xorogin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xorigin = atof(&buf[0]);
	str = "xreference: " + str;
	// XINCrement?
	err = viPrintf(vi, ":WAVeform:XINCrement?\n");
	if(err != 0)
	{
		myprint("�� ������� ������� ����� Xincrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	xincrement = atof(&buf[0]);
	str = "xreference: " + str;
	//YREFerence
	err = viPrintf(vi, ":WAVeform:YREFerence?\n");  // Y
	if(err != 0)
	{
		myprint("�� ������� ������� ����� Yrefenence");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yreference = atof(&buf[0]);
	str = "yreference: " + str;
	//YORigin
	err = viPrintf(vi, ":WAVeform:YORigin?\n");  // Y
	if(err != 0)
	{
		myprint("�� ������� ������� ����� Yorigin");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yorigin = atof(&buf[0]);
	str = "yreference: " + str;
	//YINCrement
	err = viPrintf(vi, ":WAVeform:YINCrement?\n");  // Y
	if(err != 0)
	{
		myprint("�� ������� ������� ����� YINCrement");
		return;
	}
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	yincrement = atof(&buf[0]);
	str = "yreference: " + str;
	//Format
	err = viPrintf(vi, ":WAVeform:FORMat WORD\n");
	if(err != 0)
	{
		myprint("err :WAVeform:FORMat word");
		return;
	}
	viPrintf(vi, ":WAVeform:FORMat?\n");
	//BYTeorder
	err = viPrintf(vi, ":WAVeform:BYTeorder MSBFirst\n");
	if(err != 0)
	{
		myprint("err :WAVeform:BYTeorder");
		return;
	}
	viPrintf(vi, ":WAVeform:BYTeorder?\n");
	//UNSigned
	err = viPrintf(vi, ":WAVeform:UNSigned 1\n");
	if(err != 0)
	{
		myprint("err :WAVeform:UNSigned ");
		return;
	}
	viPrintf(vi, ":WAVeform:UNSigned?\n");

	viPrintf(vi, ":WAVeform?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	str = "nastr: " + str;

	//DATA
	err = viPrintf(vi, ":WAVeform:DATA?\n");
	if(err != 0)
	{
		myprint("err :WAVeform:DATA?");
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
	for (int i = 0; i < points_count/10; i++)
	{
	viRead(vi, (unsigned char *)( &buf2[i*10]), 10, &retCount);
	}
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
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	//  ��� ���������� �� ����� ������
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;

	open_ses();

	// �������� ����� ���������
	err = viPrintf(vi, ":TRIGger:SWEep NORMal\n");
	if (err != 0)
	{
		Form2->RichEdit1->Lines->Add("�� ������� ��������� ���������� ����� ���������");
		Form2->ShowModal();
		return ;
	}

	// ������������� ���������
	str = ":MEASure:SOURce CHANnel" + ComboBox1->Text + "\n";
	err = viPrintf(vi, str.c_str());
	if (err != 0)
	{
		Form2->RichEdit1->Lines->Add("�� ������� ������������ � ��������� ������. �������� �����������");
		Form2->ShowModal();
		return ;
	}
   /*	else
	{
		Form2->RichEdit1->Lines->Add("������ ������� �������");
		Form2->RichEdit1->Lines->Add("����� � USB ������� ������������");
		Form2->RichEdit1->Lines->Add("����� � ��������� ������� ������� ������������");
		Form2->RichEdit1->Lines->Add("����������� � ���������� ������ ������� ����������");
		Form2->RichEdit1->Lines->Add("��������� ������� ������������");
		Application->ProcessMessages();
		Form2->Show();
		Sleep(1);
		Form2->Close();
	 }   */

	// ���������
	str = ":MEASure:VAMPlitude[" + ComboBox1->Text + "]\n";
	err = viPrintf(vi, str.c_str());
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("�� ������� ������� ���������");
		Form2->ShowModal();
		return;
	}
	viPrintf(vi, ":MEASure:VAMPlitude? \n");
	// Read results
	viScanf (vi, "%t", &buf[0]);
	// Print results
	str = buf;
	Edit1->Text = str;
	Application->ProcessMessages();

	// ����������
	str = ":MEASure:OVERshoot[" + ComboBox1->Text + "]\n";
	err = viPrintf(vi, str.c_str());
	if (err != 0)
	{
		Form2->RichEdit1->Lines->Add("�� ������� ������� ����������");
		Form2->ShowModal();
		return ;
	}
	viPrintf(vi, ":MEASure:OVERshoot?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	Edit2->Text = str;
	Application->ProcessMessages();

	// ������������ ����� ������
	str = ":MEASure:FALLtime[" + ComboBox1->Text + "]\n";
	err = viPrintf(vi, str.c_str());
	if (err != 0)
	{
		Form2->RichEdit1->Lines->Add("�� ������� ������� ���� ������");
		Form2->ShowModal();
		return;
	}
	viPrintf(vi, ":MEASure:FALLtime?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	Edit3->Text = str;
	Application->ProcessMessages();
	// ������������ ���������� ������
	str = ":MEASure:RISetime[" + ComboBox1->Text + "]\n";
	err = viPrintf(vi, str.c_str());
	if (err != 0)
	{
		Form2->RichEdit1->Lines->Add("�� ������� ������� ���������� ������");
		Form2->ShowModal();
		return ;
	}
	viPrintf(vi, ":MEASure:RISetime?\n");
	viScanf (vi, "%t", &buf[0]);
	str = buf;
	Edit4->Text = str;
	Application->ProcessMessages();

//------------------------------------------------------------------------------
	// ������ ������
	// ������������ ���������
	str = ":CHANnel" + ComboBox1->Text + ":SCALe 5[V]\n";
	err = viPrintf(vi, str.c_str()); // points
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("err :CHANnel1:SCALe");
		return;
	}
	str = ":CHANnel" + ComboBox1->Text + ":SCALe?\n" ;
	viPrintf(vi, str.c_str());
	str = ":CHANnel" + ComboBox1->Text + ":OFFSet 0\n";
	err = viPrintf(vi, str.c_str());
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("err :CHANnel1:OFFSet");
		return;
	}
	str = ":CHANnel" + ComboBox1->Text + ":OFFSet?\n";
	viPrintf(vi, str.c_str());
	//�������������� ���������
	err = viPrintf(vi, ":TIMebase:SCALe 0.000002\n");
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("err :TIMebase:SCALe");
		return;
	}
	viPrintf(vi, ":TIMebase:SCALe?\n");
	str = ":TIMebase:POSition 0.00\n";
	err = viPrintf(vi, str.c_str());
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("err :TIMebase:POSition?");
		return;
	}
	viPrintf(vi, ":TIMebase:POSition?\n");

	wave();

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

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	//  ��� ���������� �� ����� ������
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;


	open_ses();

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
			Form2->RichEdit1->Lines->Add("err :TIMebase:POSition?");
			return;
	   }
	   viPrintf(vi, ":TIMebase:POSition?\n");
	}
	wave();

	// :SBUS1:M1553:TRIGger:TYPE?
	viPrintf(vi, ":SBUS1:M1553:TRIGger:TYPE?\n");

	// Close session
	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
	//  ��� ���������� �� ����� ������
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;


	open_ses();

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
			Form2->RichEdit1->Lines->Add("err :TIMebase:POSition?");
			return;
	   }
	   viPrintf(vi, ":TIMebase:POSition?\n");
	}

	wave();

	// Close session
	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
	//  ��� ���������� �� ����� ������
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;


	open_ses();

	//�������������� ���������
	float i = 0.000001;
	str = MyFloatToStr(i);
	str = ":TIMebase:SCALe " + FloatToStr(i) + "\n";
	int k = str.Pos(",");
	if (k>0)
		str[k] = '.';
	//return str;
	err = viPrintf(vi, str.c_str());
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("err :TIMebase:SCALe?");
		return;
	}
	viPrintf(vi, ":TIMebase:SCALe?\n");

	wave();

	// Close session
	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton2Click(TObject *Sender)
{
	//  ��� ���������� �� ����� ������
	char strres [20];
	unsigned long actual;
	char buf [256] = {0};
	AnsiString str;
	double xreference, xincrement, xorigin, yreference, yincrement, yorigin;
	unsigned int points_count;
	float X, Y;


	open_ses();

	//�������������� ���������
	float i = 0.0000005;
	str = MyFloatToStr(i);
	str = ":TIMebase:SCALe " + FloatToStr(i) + "\n";
	int k = str.Pos(",");
	if (k>0)
		str[k] = '.';
	//return str;
	err = viPrintf(vi, str.c_str());
	if(err != 0)
	{
		Form2->RichEdit1->Lines->Add("err :TIMebase:SCALe?");
		return;
	}
	viPrintf(vi, ":TIMebase:SCALe?\n");

	wave();

	// Close session
	viClose (vi);
	viClose (defaultRM);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button4Click(TObject *Sender)
{
	// ��������� � �����
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBox1Change(TObject *Sender)
{
	Edit1->Text = "9.74";
	Edit2->Text = "5.4";
	Edit3->Text = "32.13";
	Edit4->Text = "15.36";
}
//---------------------------------------------------------------------------

