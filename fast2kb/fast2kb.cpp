// fast2kb.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifdef KEYMAP_PPSSPP
#include "keymap_ppsspp.h"
#else
#include "keymap_mame.h"
#endif



#define POLLING_INTERVAL 1ul // milliseconds


//#define LOG_VERBOSE_ENABLED

#ifdef LOG_VERBOSE_ENABLED
#define LOG_VERBOSE(...) (fprintf_s(stdout, __VA_ARGS__))
#else
#define LOG_VERBOSE(...)
#endif

#define LOG_ERROR(...) (fprintf_s(stderr, __VA_ARGS__))



struct FIOKEYS
{
	int kstart_flag;
	int k2start_flag;
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
	int kcoin_flag;
	int k2coin_flag;
	int ktest_flag;
	int ktilt_flag;
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

HANDLE hMainThread;
BOOL keepPolling = TRUE;



BOOL WINAPI ctrlHandler(DWORD signal)
{
	UNREFERENCED_PARAMETER(signal);

	keepPolling = FALSE;
	WaitForSingleObject(hMainThread, INFINITE);
	CloseHandle(hMainThread);

	return TRUE;
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


void dmacPoll(FIOKEYS &fioKeys, INPUT diEvent)
{
	int readButtons;
	FIO_RegRead(0x4120, int(&readButtons));
	if (readButtons != 0x0)
	{
		LOG_VERBOSE("readButtons=0x%x\n", readButtons);
	}

	// Player 1 Start + Player 1 Button 1 + Player 1 Button 3
	if (readButtons & 0x10 && readButtons & 0x10000 && readButtons & 0x100000)
	{
		if (fioKeys.kesc_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_ESC;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kesc_flag = 1;
			LOG_VERBOSE("Pressed: Escape (Player 1 Start + Button 1 + Button 3)\n");
		}
	}
	else if (fioKeys.kesc_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_ESC;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kesc_flag = 0;
		LOG_VERBOSE("Released: Escape (Player 1 Start + Button 1 + Button 3)\n");
	}

	// Coin 1
	if (readButtons & 0x1)
	{
		if (fioKeys.kcoin_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_COIN1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kcoin_flag = 1;
			LOG_VERBOSE("Pressed: Coin 1\n");
		}
	}
	else if (fioKeys.kcoin_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_COIN1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kcoin_flag = 0;
		LOG_VERBOSE("Released: Coin 1\n");
	}

	// Coin 2
	if (readButtons & 0x2)
	{
		if (fioKeys.k2coin_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_COIN2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2coin_flag = 1;
			LOG_VERBOSE("Pressed: Coin 2\n");
		}
	}
	else if (fioKeys.k2coin_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_COIN2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2coin_flag = 0;
		LOG_VERBOSE("Released: Coin 2\n");
	}

	// Service 1
	if (readButtons & 0x4)
	{
		if (fioKeys.kservice_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_SERVICE1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kservice_flag = 1;
			LOG_VERBOSE("Pressed: Service 1\n");
		}
	}
	else if (fioKeys.kservice_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_SERVICE1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kservice_flag = 0;
		LOG_VERBOSE("Released: Service 1\n");
	}

	// Service 2
	if (readButtons & 0x8)
	{
		if (fioKeys.k2service_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_SERVICE2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2service_flag = 1;
			LOG_VERBOSE("Pressed: Service 2\n");
		}
	}
	else if (fioKeys.k2service_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_SERVICE2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2service_flag = 0;
		LOG_VERBOSE("Released: Service 2\n");
	}

	// Player 1 Start
	if (readButtons & 0x10)
	{
		if (fioKeys.kstart_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_START;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kstart_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Start\n");
		}
	}
	else if (fioKeys.kstart_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_START;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kstart_flag = 0;
		LOG_VERBOSE("Released: Player 1 Start\n");
	}

	// Player 2 Start
	if (readButtons & 0x20)
	{
		if (fioKeys.k2start_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_START;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2start_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Start\n");
		}
	}
	else if (fioKeys.k2start_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_START;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2start_flag = 0;
		LOG_VERBOSE("Released: Player 2 Start\n");
	}

	// Test
	if (readButtons & 0x40)
	{
		if (fioKeys.ktest_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_TEST;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.ktest_flag = 1;
			LOG_VERBOSE("Pressed: Test\n");
		}
	}
	else if (fioKeys.ktest_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_TEST;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.ktest_flag = 0;
		LOG_VERBOSE("Released: Test\n");
	}

	// Tilt
	if (readButtons & 0x80)
	{
		if (fioKeys.ktilt_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = S_TILT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.ktilt_flag = 1;
			LOG_VERBOSE("Pressed: Tilt\n");
		}
	}
	else if (fioKeys.ktilt_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_TILT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.ktilt_flag = 0;
		LOG_VERBOSE("Released: Tilt\n");
	}

	// Player 1 Up
	if (readButtons & 0x100)
	{
		if (fioKeys.kup_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P1_UP;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kup_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Up\n");
		}
	}
	else if (fioKeys.kup_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_UP;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kup_flag = 0;
		LOG_VERBOSE("Released: Player 1 Up\n");
	}

	// Player 2 Up
	if (readButtons & 0x200)
	{
		if (fioKeys.k2up_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P2_UP;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2up_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Up\n");
		}
	}
	else if (fioKeys.k2up_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_UP;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2up_flag = 0;
		LOG_VERBOSE("Released: Player 2 Up\n");
	}

	// Player 1 Down
	if (readButtons & 0x400)
	{
		if (fioKeys.kdown_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P1_DOWN;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kdown_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Down\n");
		}
	}
	else if (fioKeys.kdown_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_DOWN;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kdown_flag = 0;
		LOG_VERBOSE("Released: Player 1 Down\n");
	}

	// Player 2 Down
	if (readButtons & 0x800)
	{
		if (fioKeys.k2down_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P2_DOWN;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2down_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Down\n");
		}
	}
	else if (fioKeys.k2down_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_DOWN;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2down_flag = 0;
		LOG_VERBOSE("Released: Player 2 Down\n");
	}

	// Player 1 Left
	if (readButtons & 0x1000)
	{
		if (fioKeys.kleft_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P1_LEFT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kleft_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Left\n");
		}
	}
	else if (fioKeys.kleft_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_LEFT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kleft_flag = 0;
		LOG_VERBOSE("Released: Player 1 Left\n");
	}

	// Player 2 Left
	if (readButtons & 0x2000)
	{
		if (fioKeys.k2left_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P2_LEFT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2left_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Left\n");
		}
	}
	else if (fioKeys.k2left_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_LEFT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2left_flag = 0;
		LOG_VERBOSE("Released: Player 2 Left\n");
	}

	// Player 1 Right
	if (readButtons & 0x4000)
	{
		if (fioKeys.kright_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P1_RIGHT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kright_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Right\n");
		}
	}
	else if (fioKeys.kright_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_RIGHT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kright_flag = 0;
		LOG_VERBOSE("Released: Player 1 Right\n");
	}

	// Player 2 Right
	if (readButtons & 0x8000)
	{
		if (fioKeys.k2right_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
			diEvent.ki.wScan = P2_RIGHT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2right_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Right\n");
		}
	}
	else if (fioKeys.k2right_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_RIGHT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2right_flag = 0;
		LOG_VERBOSE("Released: Player 2 Right\n");
	}

	// Player 1 Button 1
	if (readButtons & 0x10000)
	{
		if (fioKeys.kb1_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb1_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 1\n");
		}
	}
	else if (fioKeys.kb1_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb1_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 1\n");
	}

	// Player 2 Button 1
	if (readButtons & 0x20000)
	{
		if (fioKeys.k2b1_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b1_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 1\n");
		}
	}
	else if (fioKeys.k2b1_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b1_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 1\n");
	}

	// Player 1 Button 2
	if (readButtons & 0x40000)
	{
		if (fioKeys.kb2_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb2_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 2\n");
		}
	}
	else if (fioKeys.kb2_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb2_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 2\n");
	}

	// Player 2 Button 2
	if (readButtons & 0x80000)
	{
		if (fioKeys.k2b2_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b2_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 2\n");
		}
	}
	else if (fioKeys.k2b2_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b2_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 2\n");
	}

	// Player 1 Button 3
	if (readButtons & 0x100000)
	{
		if (fioKeys.kb3_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN3;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb3_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 3\n");
		}
	}
	else if (fioKeys.kb3_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN3;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb3_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 3\n");
	}

	// Player 2 Button 3
	if (readButtons & 0x200000)
	{
		if (fioKeys.k2b3_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN3;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b3_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 3\n");
		}
	}
	else if (fioKeys.k2b3_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN3;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b3_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 3\n");
	}

	// Player 1 Button 4
	if (readButtons & 0x400000)
	{
		if (fioKeys.kb4_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN4;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb4_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 4\n");
		}
	}
	else if (fioKeys.kb4_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN4;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb4_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 4\n");
	}

	// Player 2 Button 4
	if (readButtons & 0x800000)
	{
		if (fioKeys.k2b4_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN4;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b4_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 4\n");
		}
	}
	else if (fioKeys.k2b4_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN4;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b4_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 4\n");
	}

	// Player 1 Button 5
	if (readButtons & 0x1000000)
	{
		if (fioKeys.kb5_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN5;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb5_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 5\n");
		}
	}
	else if (fioKeys.kb5_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN5;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb5_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 5\n");
	}

	// Player 2 Button 5
	if (readButtons & 0x2000000)
	{
		if (fioKeys.k2b5_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN5;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b5_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 5\n");
		}
	}
	else if (fioKeys.k2b5_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN5;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b5_flag = 0;
		LOG_VERBOSE("Pressed: Player 2 Button 5\n");
	}

	// Player 1 Button 6
	if (readButtons & 0x4000000)
	{
		if (fioKeys.kb6_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN6;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb6_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 6\n");
		}
	}
	else if (fioKeys.kb6_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN6;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb6_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 6\n");
	}

	// Player 2 Button 6
	if (readButtons & 0x8000000)
	{
		if (fioKeys.k2b6_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN6;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b6_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 6\n");
		}
	}
	else if (fioKeys.k2b6_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN6;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b6_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 6\n");
	}

	// Player 1 Button 7
	if (readButtons & 0x10000000)
	{
		if (fioKeys.kb7_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN7;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb7_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 7\n");
		}
	}
	else if (fioKeys.kb7_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN7;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb7_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 7\n");
	}

	// Player 2 Button 7
	if (readButtons & 0x20000000)
	{
		if (fioKeys.k2b7_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN7;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b7_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 7\n");
		}
	}
	else if (fioKeys.k2b7_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN7;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b7_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 7\n");
	}

	// Player 1 Button 8
	if (readButtons & 0x40000000)
	{
		if (fioKeys.kb8_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P1_BTN8;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb8_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 8\n");
		}
	}
	else if (fioKeys.kb8_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN8;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb8_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 8\n");
	}

	// Player 2 Button 8
	if (readButtons & 0x80000000)
	{
		if (fioKeys.k2b8_flag == 0)
		{
			diEvent.ki.dwFlags = KEYEVENTF_SCANCODE;
			diEvent.ki.wScan = P2_BTN8;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b8_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 8\n");
		}
	}
	else if (fioKeys.k2b8_flag == 1)
	{
		diEvent.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN8;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b8_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 8\n");
	}
}


int main()
{
	HANDLE pseudoHProcess = GetCurrentProcess();
	HANDLE pseudoHMainThread = GetCurrentThread();

	if (!DuplicateHandle(pseudoHProcess, pseudoHMainThread, pseudoHProcess,
		&hMainThread, NULL, FALSE, DUPLICATE_SAME_ACCESS))
	{
		DWORD error = GetLastError();
		LOG_ERROR("Failed to get handle to main thread. Error code: %d\n", error);
		return 1;
	}

	if (!SetConsoleCtrlHandler(ctrlHandler, TRUE))
	{
		DWORD error = GetLastError();
		LOG_ERROR("Failed to set control handler. Error code: %d\n", error);
		return 1;
	}

	int drvOpen = FIO_Open();
	if (!drvOpen)
	{
		DWORD error = GetLastError();
		LOG_ERROR("Failed to open iDmacDrv device. Error code: %d\n", error);
		return 1;
	}

	FIOKEYS fioKeys = { 0 };

	INPUT diEvent = { 0 };
	diEvent.type = INPUT_KEYBOARD;

	LOG_VERBOSE("Starting to poll input registers...\n");
	while (keepPolling)
	{
		dmacPoll(fioKeys, diEvent);
		Sleep(POLLING_INTERVAL);
	}
	LOG_VERBOSE("Finished polling input registers\n");

	return 0;
}
