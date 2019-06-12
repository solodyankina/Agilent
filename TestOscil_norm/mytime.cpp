//---------------------------------------------------------------------------


#pragma hdrstop

#include "mytime.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#include <windows.h>

//---------------------------------------------------------------------------
//������ �������
DWORDLONG myGetMKSTicks()             //int64
{
  LARGE_INTEGER lpPerformanceCount, Frequency;
  QueryPerformanceCounter(&lpPerformanceCount);
  QueryPerformanceFrequency(&Frequency);
  return DWORDLONG(
	DWORDLONG(lpPerformanceCount.LowPart + lpPerformanceCount.HighPart * 0x100000000 )
	*1.0 / DWORDLONG(Frequency.LowPart + Frequency.HighPart * 0x100000000 ) * 1000000 );
}

//---------------------------------------------------------------------------
// �������� � ��
void mydelay(unsigned int deltaMs)
{
	DWORDLONG st = myGetMKSTicks();
	while (myGetMKSTicks()- st<deltaMs*1000);
}
