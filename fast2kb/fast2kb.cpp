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
#include <iostream>
#include "INIReader.h"

struct INPUTS
{
	int p1_up;
	int p1_down;
	int p1_left;
	int p1_right;
	int p1_b1;
	int p1_b2;
	int p1_b3;
	int p1_b4;
	int p1_b5;
	int p1_b6;
	int p1_b7;
	int p1_b8;
	int p1_start;
	int p1_service;
	
	int p2_up;
	int p2_down;
	int p2_left;
	int p2_right;
	int p2_b1;
	int p2_b2;
	int p2_b3;
	int p2_b4;
	int p2_b5;
	int p2_b6;
	int p2_b7;
	int p2_b8;
	int p2_start;
	int p2_service;
	
	int test;
	
	// Flags
	int flag_p1_up = KEYEVENTF_SCANCODE;
	int flag_p1_down = KEYEVENTF_SCANCODE;
	int flag_p1_left = KEYEVENTF_SCANCODE;
	int flag_p1_right = KEYEVENTF_SCANCODE;
	int flag_p1_b1 = KEYEVENTF_SCANCODE;
	int flag_p1_b2 = KEYEVENTF_SCANCODE;
	int flag_p1_b3 = KEYEVENTF_SCANCODE;
	int flag_p1_b4 = KEYEVENTF_SCANCODE;
	int flag_p1_b5 = KEYEVENTF_SCANCODE;
	int flag_p1_b6 = KEYEVENTF_SCANCODE;
	int flag_p1_b7 = KEYEVENTF_SCANCODE;
	int flag_p1_b8 = KEYEVENTF_SCANCODE;
	int flag_p1_start = KEYEVENTF_SCANCODE;
	int flag_p1_service = KEYEVENTF_SCANCODE;

	int flag_p2_up = KEYEVENTF_SCANCODE;
	int flag_p2_down = KEYEVENTF_SCANCODE;
	int flag_p2_left = KEYEVENTF_SCANCODE;
	int flag_p2_right = KEYEVENTF_SCANCODE;
	int flag_p2_b1 = KEYEVENTF_SCANCODE;
	int flag_p2_b2 = KEYEVENTF_SCANCODE;
	int flag_p2_b3 = KEYEVENTF_SCANCODE;
	int flag_p2_b4 = KEYEVENTF_SCANCODE;
	int flag_p2_b5 = KEYEVENTF_SCANCODE;
	int flag_p2_b6 = KEYEVENTF_SCANCODE;
	int flag_p2_b7 = KEYEVENTF_SCANCODE;
	int flag_p2_b8 = KEYEVENTF_SCANCODE;
	int flag_p2_start = KEYEVENTF_SCANCODE;
	int flag_p2_service = KEYEVENTF_SCANCODE;

	int flag_test = KEYEVENTF_SCANCODE;
};

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
	int kb7_flag;
	int k2b7_flag;
	int kb8_flag;
	int k2b8_flag;
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

BOOL setup_keys(INPUTS *inputs)
{
	INIReader reader("z:\config.ini");
	if (reader.ParseError() != 0) {
		std::cout << "Can't load 'config.ini'\n"; // Defults from reader.Get will be honored even config cant be found
	}
	int ext_ket_array[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_RMENU, DIK_RCONTROL, DIK_INSERT, DIK_DELETE, DIK_HOME, DIK_END, DIK_PGDN, DIK_PGUP, DIK_NUMLOCK, DIK_PAUSE, DIK_DIVIDE, DIK_NUMPADENTER };
	// Player 1
	inputs->p1_up = std::stoi(reader.Get("controls", "P1_UP", "0xC8"), 0, 16);
	inputs->p1_down = std::stoi(reader.Get("controls", "P1_DOWN", "0xD0"), 0, 16);
	inputs->p1_left = std::stoi(reader.Get("controls", "P1_LEFT", "0xCB"), 0, 16);
	inputs->p1_right = std::stoi(reader.Get("controls", "P1_RIGHT", "0xCD"), 0, 16);
	inputs->p1_b1 = std::stoi(reader.Get("controls", "P1_BTN1", "0x1D"), 0, 16);
	inputs->p1_b2 = std::stoi(reader.Get("controls", "P1_BTN2", "0x38"), 0, 16);
	inputs->p1_b3 = std::stoi(reader.Get("controls", "P1_BTN3", "0x39"), 0, 16);
	inputs->p1_b4 = std::stoi(reader.Get("controls", "P1_BTN4", "0x2A"), 0, 16);
	inputs->p1_b5 = std::stoi(reader.Get("controls", "P1_BTN5", "0x2C"), 0, 16);
	inputs->p1_b6 = std::stoi(reader.Get("controls", "P1_BTN6", "0x2D"), 0, 16);
	inputs->p1_b7 = std::stoi(reader.Get("controls", "P1_BTN7", "0x2E"), 0, 16);
	inputs->p1_b8 = std::stoi(reader.Get("controls", "P1_BTN8", "0x2F"), 0, 16);
	inputs->p1_start = std::stoi(reader.Get("controls", "P1_START", "0x02"), 0, 16);
	inputs->p1_service = std::stoi(reader.Get("controls", "P1_SERVICE", "0x0A"), 0, 16);
	
	//player 2
	inputs->p2_up = std::stoi(reader.Get("controls", "P2_UP", "0x13"), 0, 16);
	inputs->p2_down = std::stoi(reader.Get("controls", "P2_DOWN", "0x21"), 0, 16);
	inputs->p2_left = std::stoi(reader.Get("controls", "P2_LEFT", "0x20"), 0, 16);
	inputs->p2_right = std::stoi(reader.Get("controls", "P2_RIGHT", "0x22"), 0, 16);
	inputs->p2_b1 = std::stoi(reader.Get("controls", "P2_BTN1", "0x1E"), 0, 16);
	inputs->p2_b2 = std::stoi(reader.Get("controls", "P2_BTN2", "0x1F"), 0, 16);
	inputs->p2_b3 = std::stoi(reader.Get("controls", "P2_BTN3", "0x10"), 0, 16);
	inputs->p2_b4 = std::stoi(reader.Get("controls", "P2_BTN4", "0x11"), 0, 16);
	inputs->p2_b5 = std::stoi(reader.Get("controls", "P2_BTN5", "0x12"), 0, 16);
	inputs->p2_b6 = std::stoi(reader.Get("controls", "P2_BTN6", "0x15"), 0, 16);
	inputs->p2_b7 = std::stoi(reader.Get("controls", "P2_BTN7", "0x23"), 0, 16);
	inputs->p2_b8 = std::stoi(reader.Get("controls", "P2_BTN8", "0x16"), 0, 16);
	inputs->p2_start = std::stoi(reader.Get("controls", "P2_START", "0x03"), 0, 16);
	inputs->p2_service = std::stoi(reader.Get("controls", "P2_SERVICE", "0x0B"), 0, 16);

	//test
	inputs->test = std::stoi(reader.Get("controls", "TEST", "0x3C"), 0, 16);


	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_up) != (ext_ket_array + 16))
	{
		inputs->flag_p1_up = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_down) != (ext_ket_array + 16))
	{
		inputs->flag_p1_down = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_left) != (ext_ket_array + 16))
	{
		inputs->flag_p1_left = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_right) != (ext_ket_array + 16))
	{
		inputs->flag_p1_right = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b1) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b1 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b2) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b2 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b3) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b3 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b4) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b4 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b5) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b5 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b6) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b6 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b7) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b7 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_b8) != (ext_ket_array + 16))
	{
		inputs->flag_p1_b8 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_start) != (ext_ket_array + 16))
	{
		inputs->flag_p1_start = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p1_service) != (ext_ket_array + 16))
	{
		inputs->flag_p1_service = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_up) != (ext_ket_array + 16))
	{
		inputs->flag_p2_up = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_down) != (ext_ket_array + 16))
	{
		inputs->flag_p2_down = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_left) != (ext_ket_array + 16))
	{
		inputs->flag_p2_left = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_right) != (ext_ket_array + 16))
	{
		inputs->flag_p2_right = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b1) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b1 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b2) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b2 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b3) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b3 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b4) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b4 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b5) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b5 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b6) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b6 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b6) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b7 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_b6) != (ext_ket_array + 16))
	{
		inputs->flag_p2_b8 = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_start) != (ext_ket_array + 16))
	{
		inputs->flag_p2_start = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->p2_service) != (ext_ket_array + 16))
	{
		inputs->flag_p2_service = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	if (std::find(ext_ket_array, ext_ket_array + 16, inputs->test) != (ext_ket_array + 16))
	{
		inputs->flag_test = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
	}
	return 0;
}

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
int dmacpoll(JVSKEY *jkey, LPINPUT ip, INPUTS *inputs)
{
	int readButtons;
	Sleep(10);
	FIO_RegRead(0x4120, int(&readButtons));
	//printf("%x\n", readButtons);
	if (int(readButtons) & 0x10)
	{
		ip->ki.dwFlags = inputs->flag_p1_start;
		ip->ki.wScan = inputs->p1_start;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kstart_flag = 1;
	}
	else if (jkey->kstart_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_start | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_start;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kstart_flag = 0;

		//printf("p1 start\n");
	}
	if (int(readButtons) & 0x4)
	{
		ip->ki.dwFlags = inputs->flag_p1_service;
		ip->ki.wScan = inputs->p1_service;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kservice_flag = 1;


	}
	else if (jkey->kservice_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_service | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_service;

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
		ip->ki.dwFlags = inputs->flag_p1_up;
	    ip->ki.wScan = inputs->p1_up;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kup_flag = 1;

	}
	else if (jkey->kup_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_up | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_up;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kup_flag = 0;
		//printf("p1 up\n");
	}
	if (int(readButtons) & 0x400)
	{
		ip->ki.dwFlags = inputs->flag_p1_down;
		ip->ki.wScan = inputs->p1_down;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kdown_flag = 1;

	}
	else if (jkey->kdown_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_down | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_down;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kdown_flag = 0;
		//printf("p1 down\n");
	}
	if (int(readButtons) & 0x1000)
	{
		ip->ki.dwFlags = inputs->flag_p1_left;
		ip->ki.wScan = inputs->p1_left;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kleft_flag = 1;

	}
	else if (jkey->kleft_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_left | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_left;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kleft_flag = 0;

		//printf("p1 left\n");
	}
	if (int(readButtons) & 0x4000)
	{
		ip->ki.dwFlags = inputs->flag_p1_right;
		ip->ki.wScan = inputs->p1_right;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kright_flag = 1;


	}
	else if (jkey->kright_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_right | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_right;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kright_flag = 0;

		//printf("p1 right\n");
	}
	if (int(readButtons) & 0x10000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b1;
		ip->ki.wScan = inputs->p1_b1;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb1_flag = 1;

	}
	else if (jkey->kb1_flag)
	{
		ip->ki.dwFlags = inputs->flag_p1_b1 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b1;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb1_flag = 0;

		//printf("p1 Button 1\n");
	}
	if (int(readButtons) & 0x40000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b2;
		ip->ki.wScan = inputs->p1_b2;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb2_flag = 1;


	}
	else if (jkey->kb2_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b2 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b2;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb2_flag = 0;

		//printf("p1 Button 2\n");
	}
	if (int(readButtons) & 0x100000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b3;
		ip->ki.wScan = inputs->p1_b3;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb3_flag = 1;


	}
	else if (jkey->kb3_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b3 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b3;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb3_flag = 0;

		//printf("p1 Button 3\n");
	}
	if (int(readButtons) & 0x400000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b4;
		ip->ki.wScan = inputs->p1_b4;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb4_flag = 1;


	}
	else if (jkey->kb4_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b4 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b4;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb4_flag = 0;

		//printf("p1 button 4\n");
	}
	if (int(readButtons) & 0x1000000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b5;
		ip->ki.wScan = inputs->p1_b5;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb5_flag = 1;


	}
	else if (jkey->kb5_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b5 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b5;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb5_flag = 0;

		//printf("p1 button 5\n");
	}
	if (int(readButtons) & 0x4000000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b6;
		ip->ki.wScan = inputs->p1_b6;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb6_flag = 1;


	}
	else if (jkey->kb6_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b6 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b6;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb6_flag = 0;

		//printf("p1 button 6\n");
	}
	if (int(readButtons) & 0x10000000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b7;
		ip->ki.wScan = inputs->p1_b7;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb7_flag = 1;


	}
	else if (jkey->kb7_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b7 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b7;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb7_flag = 0;

		//printf("p1 button 7\n");
	}
	if (int(readButtons) & 0x40000000)
	{
		ip->ki.dwFlags = inputs->flag_p1_b8;
		ip->ki.wScan = inputs->p1_b8;
		SendInput(1, ip, sizeof(INPUT));
		jkey->kb8_flag = 1;


	}
	else if (jkey->kb8_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p1_b8 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p1_b8;

		SendInput(1, ip, sizeof(INPUT));
		jkey->kb8_flag = 0;

		//printf("p1 button 8\n");
	}
	if (int(readButtons) & 0x20)
	{
		ip->ki.dwFlags = inputs->flag_p2_start;
		ip->ki.wScan = inputs->p2_start;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2start_flag = 1;
	}
	else if (jkey->k2start_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_start | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_start;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2start_flag = 0;

		//printf("p2 start\n");
	}
	if (int(readButtons) & 0x200)
	{
		ip->ki.dwFlags = inputs->flag_p2_up;
		ip->ki.wScan = inputs->p2_up;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2up_flag = 1;
	}
	else if (jkey->k2up_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_up | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_up;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2up_flag = 0;

		//printf("p2 up\n");
	}
	if (int(readButtons) & 0x800)
	{
		ip->ki.dwFlags = inputs->flag_p2_down;
		ip->ki.wScan = inputs->p2_down;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2down_flag = 1;
	}
	else if (jkey->k2down_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_down | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_down;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2down_flag = 0;

		//printf("p2 down\n");
	}
	if (int(readButtons) & 0x2000)
	{
		ip->ki.dwFlags = inputs->flag_p2_left;
		ip->ki.wScan = inputs->p2_left;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2left_flag = 1;
	}
	else if (jkey->k2left_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_left | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_left;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2left_flag = 0;

		//printf("p2 left\n");
	}
	if (int(readButtons) & 0x8000)
	{
		ip->ki.dwFlags = inputs->flag_p2_right;
		ip->ki.wScan = inputs->p2_right;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2right_flag = 1;
	}
	else if (jkey->k2right_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_right | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_right;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2right_flag = 0;

		//printf("p2 right\n");
	}
	if (int(readButtons) & 0x20000)
	{
		ip->ki.dwFlags = inputs->flag_p2_b1;
		ip->ki.wScan = inputs->p2_b1;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b1_flag = 1;
	}
	else if (jkey->k2b1_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b1 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b1;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b1_flag = 0;

		//printf("p2 BTN 1\n");
	}
	if (int(readButtons) & 0x80000)
	{
		ip->ki.dwFlags = inputs->flag_p2_b2;
		ip->ki.wScan = inputs->p2_b2;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b2_flag = 1;
	}
	else if (jkey->k2b2_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b2 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b2;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b2_flag = 0;

		//printf("p2 BTN 2\n");
	}
	if (int(readButtons) & 0x200000)
	{
		ip->ki.dwFlags = inputs->flag_p2_b3;
		ip->ki.wScan = inputs->p2_b3;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b3_flag = 1;
	}
	else if (jkey->k2b3_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b3 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b3;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b3_flag = 0;

		//printf("p2 BTN 3\n");
	}
	if (int(readButtons) & 0x800000)
	{
		ip->ki.dwFlags = inputs->flag_p2_b4;
		ip->ki.wScan = inputs->p2_b4;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b4_flag = 1;
	}
	else if (jkey->k2b4_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b4 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b4;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b4_flag = 0;

		//printf("p2 BTN 4\n");
	}
	if (int(readButtons) & 0x2000000)
	{
		ip->ki.dwFlags = inputs->flag_p2_b5;
		ip->ki.wScan = inputs->p2_b5;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b5_flag = 1;
	}
	else if (jkey->k2b5_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b5 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b5;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b5_flag = 0;

		//printf("p2 BTN 5\n");
	}
	if (int(readButtons) & 0x8000000)
	{
		ip->ki.dwFlags = inputs->flag_p2_b6;
		ip->ki.wScan = inputs->p2_b6;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b6_flag = 1;
	}
	else if (jkey->k2b6_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b6 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b6;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b6_flag = 0;

		//printf("p2 BTN 6\n");
	}
	if (int(readButtons) & 0x20000000)
	{

		ip->ki.dwFlags = inputs->flag_p2_b7;
		ip->ki.wScan = inputs->p2_b7;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b7_flag = 1;
	}
	else if (jkey->k2b7_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b7 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b7;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b7_flag = 0;

		//printf("p2 BTN 7\n");
	}
	if (int(readButtons) & 0x80000000)
	{

		ip->ki.dwFlags = inputs->flag_p2_b8;
		ip->ki.wScan = inputs->p2_b8;
		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b8_flag = 1;
	}
	else if (jkey->k2b8_flag == 1)
	{
		ip->ki.dwFlags = inputs->flag_p2_b8 | KEYEVENTF_KEYUP;
		ip->ki.wScan = inputs->p2_b8;

		SendInput(1, ip, sizeof(INPUT));
		jkey->k2b8_flag = 0;

		//printf("p2 BTN 8\n");
	}
	return 0;
}

int main()
{
	INPUTS inputs;
	setup_keys(&inputs);
	
	INPUT ip = { 0 };
	LPDIRECTINPUT8 di;
	JVSKEY jkey = { 0 };
	SetupJVSKEY(&jkey, &ip, &di);
	int readButtons;
	
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
			dmacpoll(&jkey, &ip, &inputs);
		}
	}
	return 0;
}