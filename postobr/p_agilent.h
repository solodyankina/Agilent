//---------------------------------------------------------------------------

#ifndef p_agilentH
#define p_agilentH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TLabel *LabelFileName;
	TEdit *EditFileName;
	TButton *Button1;
	TLabel *LabelTime;
	TLabel *LabelDate;
	TSpeedButton *SpeedButton1;
	TOpenDialog *OpenDialog1;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *EditFileHtml;
	TSpeedButton *SpeedButton2;
	TOpenDialog *OpenDialog2;
	TButton *Button2;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
