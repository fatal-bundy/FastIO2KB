// fastio2kb.cpp : Defines the entry point for the console application.
//
#pragma once

#include "stdafx.h"
#include <windows.h>
//#include <tchar.h>
#include <stdio.h>
#include <io.h>
#include <dinput.h>
#include "key_config.h"



struct JVSKEY
{
	int kup_flag;
	int k2up_flag;
	int kdown_flag;
	int k2down_flag;
	int kleft_flag;
	int k2left_flag;
	int kright_flag;
	int k2right_flag;
	int kb1_flag;
	int k2b1_flag;
	int kb2_flag;
	int k2b2_flag;
	int kb3_flag;
	int k2b3_flag;
	int kb4_flag;
	int k2b4_flag;
	int kb5_flag;
	int k2b5_flag;
	int kb6_flag;
	int k2b6_flag;
	int kstart_flag;
	int k2start_flag;
	int ktest_flag;
	int kservice_flag;
	int k2service_flag;
	int kesc_flag;
};

typedef int(*dmacOpen)(int, LPVOID, LPVOID);
typedef int(*dmacRead)(int, DWORD, int, LPVOID);
typedef int(*dmacWrite)(int, DWORD, int, LPVOID);
typedef int(*dmacClose)(int, int);
HMODULE dmacdll = LoadLibrary(TEXT("iDmacDrv32.dll"));
dmacOpen iDmacDrvOpen = (dmacOpen)GetProcAddress(dmacdll, "iDmacDrvOpen");
dmacRead iDmacDrvRegisterRead = (dmacRead)GetProcAddress(dmacdll, "iDmacDrvRegisterRead");
dmacWrite iDmacDrvRegisterWrite = (dmacWrite)GetProcAddress(dmacdll, "iDmacDrvRegisterWrite");
dmacClose iDmacDrvClose = (dmacClose)GetProcAddress(dmacdll, "iDmacDrvClose");
int dword_9F47B4;
int dword_9F47AC;
int dword_9B4208 = 1;

BOOL __cdecl FIO_RegRead(int a1, int a2)
{
	return iDmacDrvRegisterRead(dword_9F47B4, a1, a2, &dword_9F47AC) == 0;
}

int __cdecl FIO_Open()
{
	int result; // eax@10
	unsigned int v1; // [sp+0h] [bp-8h]@1
	int v2; // [sp+4h] [bp-4h]@15

	v1 = iDmacDrvOpen(dword_9B4208, &dword_9F47B4, &dword_9F47AC);
	if (v1)
	{
		return 0;
	}

	if (FIO_RegRead(1024, (int)&v2))
	{
		if (FIO_RegRead(0x4000, (int)&v2))
		{

			if (FIO_RegRead(16388, (int)&v2))
			{
				result = 1;
			}
			else
			{
				result = 0;
			}
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		result = 0;
	}
	return result;
}
void SetupJVSKEY(JVSKEY *jkey, INPUT *ip, LPDIRECTINPUT8 *di)
{
	ip->type = INPUT_KEYBOARD;
	ip->ki.dwFlags = KEYEVENTF_SCANCODE;
	ip->ki.wScan = 0;
	ip->ki.time = 0;
	ip->ki.dwExtraInfo = 0;
}
int dmacpoll(JVSKEY *jkey, LPINPUT ip)
{
	int readButtons;
	Sleep(10);
	FIO_RegRead(0x4120, int(&readButtons));
	//printf("%x\n", readButtons);
	if (int(readButtons) & 0x4)
	{
		//printf("p1 service\n");
	}
	if (int(readButtons) & 0x10)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_START;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kstart_flag = 1;
	}
	else if (jkey->kstart_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_START;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kstart_flag = 0;

		//printf("p1 start\n");
	}
	if (int(readButtons) & 0x4)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = S_SERVICE1;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kservice_flag = 1;


	}
	else if (jkey->kservice_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = S_SERVICE1;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kservice_flag = 0;

		//printf("p1 service\n");
	}
	if (int(readButtons) & 0x40)
	{
		//printf("Test\n");
	}
	if (int(readButtons) & 0x100)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
		ip->ki.wScan = P1_UP;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kup_flag = 1;

	}
	else if (jkey->kup_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_UP;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kup_flag = 0;
		//printf("p1 up\n");
	}
	if (int(readButtons) & 0x400)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
		ip->ki.wScan = P1_DOWN;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kdown_flag = 1;

	}
	else if (jkey->kdown_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_DOWN;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kdown_flag = 0;
		//printf("p1 down\n");
	}
	if (int(readButtons) & 0x1000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
		ip->ki.wScan = P1_LEFT;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kleft_flag = 1;

	}
	else if (jkey->kleft_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_LEFT;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kleft_flag = 0;

		//printf("p1 left\n");
	}
	if (int(readButtons) & 0x4000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
		ip->ki.wScan = P1_RIGHT;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kright_flag = 1;


	}
	else if (jkey->kright_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_RIGHT;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kright_flag = 0;

		//printf("p1 right\n");
	}
	if (int(readButtons) & 0x10000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_BTN1;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb1_flag = 1;

	}
	else if (jkey->kb1_flag)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_BTN1;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb1_flag = 0;

		//printf("p1 Button 1\n");
	}
	if (int(readButtons) & 0x40000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_BTN2;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb2_flag = 1;


	}
	else if (jkey->kb2_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_BTN2;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb2_flag = 0;

		//printf("p1 Button 2\n");
	}
	if (int(readButtons) & 0x100000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_BTN3;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb3_flag = 1;


	}
	else if (jkey->kb3_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_BTN3;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb3_flag = 0;

		//printf("p1 Button 3\n");
	}
	if (int(readButtons) & 0x400000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_BTN4;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb4_flag = 1;


	}
	else if (jkey->kb4_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_BTN4;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb4_flag = 0;

		//printf("p1 button 4\n");
	}
	if (int(readButtons) & 0x1000000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_BTN5;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb5_flag = 1;


	}
	else if (jkey->kb5_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_BTN5;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb5_flag = 0;

		//printf("p1 button 5\n");
	}
	if (int(readButtons) & 0x4000000)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE;
		ip->ki.wScan = P1_BTN6;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb6_flag = 1;


	}
	else if (jkey->kb6_flag == 1)
	{
		ip->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip->ki.wScan = P1_BTN6;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb6_flag = 0;

		//printf("p1 button 6\n");
	}
	return 0;
}

int main()
{
	INPUT ip = { 0 };
	LPDIRECTINPUT8 di;
	JVSKEY jkey = { 0 };
	SetupJVSKEY(&jkey, &ip, &di);
	//int readButtons;

	int drvOpen = FIO_Open();
	if (!drvOpen)
	{
		DWORD error;
		error = GetLastError();
		printf("An error loading DmacOpenDrv has occered Error code: %d\n", error);
	}
	else
	{
		while (true)
		{
			dmacpoll(&jkey, &ip);
		}
	}
	return 0;
}