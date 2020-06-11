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
typedef int(*dmacRead)(int, DWORD, LPVOID, LPVOID);
typedef int(*dmacWrite)(int, DWORD, int, LPVOID);
typedef int(*dmacClose)(int, LPVOID);

HMODULE dmacdll = LoadLibrary(TEXT("iDmacDrv32.dll"));

dmacOpen iDmacDrvOpen = (dmacOpen)GetProcAddress(dmacdll, "iDmacDrvOpen");
dmacRead iDmacDrvRegisterRead = (dmacRead)GetProcAddress(dmacdll, "iDmacDrvRegisterRead");
dmacWrite iDmacDrvRegisterWrite = (dmacWrite)GetProcAddress(dmacdll, "iDmacDrvRegisterWrite");
dmacClose iDmacDrvClose = (dmacClose)GetProcAddress(dmacdll, "iDmacDrvClose");


int deviceIndex = 1;
int deviceId;
int buttonsAddressP1P2;
int buttonsAddressP3P4;

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


BOOL __cdecl FIO_Open()
{
	int flags = 0x0;
	return iDmacDrvOpen(deviceIndex, &deviceId, &flags) == 0;
}


BOOL __cdecl FIO_RegRead(DWORD address, int &data_out)
{
	int flags = 0x0;
	return iDmacDrvRegisterRead(deviceId, address, &data_out, &flags) == 0;
}


BOOL assignPlayers()
{
	int data;
	BOOL port1HasConnection = FIO_RegRead(0x4000, data) && data & 0xff;
	BOOL port2HasConnection = FIO_RegRead(0x4004, data) && data & 0xff;

	if (port1HasConnection)
	{
		buttonsAddressP1P2 = 0x4120;
		LOG_VERBOSE("Port 1: Player 1 + Player 2\n");
		if (port2HasConnection)
		{
			buttonsAddressP3P4 = 0x41a0;
			LOG_VERBOSE("Port 2: Player 3 + Player 4\n");
		}
		else
		{
			buttonsAddressP3P4 = NULL;
			LOG_VERBOSE("Port 2: Empty\n");
		}
	}
	else
	{
		buttonsAddressP3P4 = NULL;
		LOG_VERBOSE("Port 1: Empty\n");
		if (port2HasConnection)
		{
			buttonsAddressP1P2 = 0x41a0;
			LOG_VERBOSE("Port 2: Player 1 + Player 2\n");
		}
		else
		{
			buttonsAddressP1P2 = NULL;
			LOG_VERBOSE("Port 2: Empty\n");
			return FALSE;
		}
	}

	return TRUE;
}


void poll(FIOKEYS &fioKeys, INPUT &diEvent)
{
	int buttonsData;
	if (!FIO_RegRead(buttonsAddressP1P2, buttonsData))
	{
		return;
	}
	if (buttonsData != 0x0)
	{
		LOG_VERBOSE("buttonsData=0x%x\n", buttonsData);
	}

	// Player 1 Start + Player 1 Button 1 + Player 1 Button 3
	if (buttonsData & 0x10 && buttonsData & 0x10000 && buttonsData & 0x100000)
	{
		if (fioKeys.kesc_flag == 0)
		{
			diEvent.ki.dwFlags = S_ESC_FLAGS;
			diEvent.ki.wScan = S_ESC;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kesc_flag = 1;
			LOG_VERBOSE("Pressed: Escape (Player 1 Start + Button 1 + Button 3)\n");
		}
	}
	else if (fioKeys.kesc_flag == 1)
	{
		diEvent.ki.dwFlags = S_ESC_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_ESC;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kesc_flag = 0;
		LOG_VERBOSE("Released: Escape (Player 1 Start + Button 1 + Button 3)\n");
	}

	// Coin 1
	if (buttonsData & 0x1)
	{
		if (fioKeys.kcoin_flag == 0)
		{
			diEvent.ki.dwFlags = S_COIN1_FLAGS;
			diEvent.ki.wScan = S_COIN1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kcoin_flag = 1;
			LOG_VERBOSE("Pressed: Coin 1\n");
		}
	}
	else if (fioKeys.kcoin_flag == 1)
	{
		diEvent.ki.dwFlags = S_COIN1_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_COIN1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kcoin_flag = 0;
		LOG_VERBOSE("Released: Coin 1\n");
	}

	// Coin 2
	if (buttonsData & 0x2)
	{
		if (fioKeys.k2coin_flag == 0)
		{
			diEvent.ki.dwFlags = S_COIN2_FLAGS;
			diEvent.ki.wScan = S_COIN2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2coin_flag = 1;
			LOG_VERBOSE("Pressed: Coin 2\n");
		}
	}
	else if (fioKeys.k2coin_flag == 1)
	{
		diEvent.ki.dwFlags = S_COIN2_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_COIN2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2coin_flag = 0;
		LOG_VERBOSE("Released: Coin 2\n");
	}

	// Service 1
	if (buttonsData & 0x4)
	{
		if (fioKeys.kservice_flag == 0)
		{
			diEvent.ki.dwFlags = S_SERVICE1_FLAGS;
			diEvent.ki.wScan = S_SERVICE1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kservice_flag = 1;
			LOG_VERBOSE("Pressed: Service 1\n");
		}
	}
	else if (fioKeys.kservice_flag == 1)
	{
		diEvent.ki.dwFlags = S_SERVICE1_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_SERVICE1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kservice_flag = 0;
		LOG_VERBOSE("Released: Service 1\n");
	}

	// Service 2
	if (buttonsData & 0x8)
	{
		if (fioKeys.k2service_flag == 0)
		{
			diEvent.ki.dwFlags = S_SERVICE2_FLAGS;
			diEvent.ki.wScan = S_SERVICE2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2service_flag = 1;
			LOG_VERBOSE("Pressed: Service 2\n");
		}
	}
	else if (fioKeys.k2service_flag == 1)
	{
		diEvent.ki.dwFlags = S_SERVICE2_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_SERVICE2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2service_flag = 0;
		LOG_VERBOSE("Released: Service 2\n");
	}

	// Player 1 Start
	if (buttonsData & 0x10)
	{
		if (fioKeys.kstart_flag == 0)
		{
			diEvent.ki.dwFlags = P1_START_FLAGS;
			diEvent.ki.wScan = P1_START;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kstart_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Start\n");
		}
	}
	else if (fioKeys.kstart_flag == 1)
	{
		diEvent.ki.dwFlags = P1_START_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_START;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kstart_flag = 0;
		LOG_VERBOSE("Released: Player 1 Start\n");
	}

	// Player 2 Start
	if (buttonsData & 0x20)
	{
		if (fioKeys.k2start_flag == 0)
		{
			diEvent.ki.dwFlags = P2_START_FLAGS;
			diEvent.ki.wScan = P2_START;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2start_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Start\n");
		}
	}
	else if (fioKeys.k2start_flag == 1)
	{
		diEvent.ki.dwFlags = P2_START_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_START;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2start_flag = 0;
		LOG_VERBOSE("Released: Player 2 Start\n");
	}

	// Test
	if (buttonsData & 0x40)
	{
		if (fioKeys.ktest_flag == 0)
		{
			diEvent.ki.dwFlags = S_TEST_FLAGS;
			diEvent.ki.wScan = S_TEST;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.ktest_flag = 1;
			LOG_VERBOSE("Pressed: Test\n");
		}
	}
	else if (fioKeys.ktest_flag == 1)
	{
		diEvent.ki.dwFlags = S_TEST_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_TEST;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.ktest_flag = 0;
		LOG_VERBOSE("Released: Test\n");
	}

	// Tilt
	if (buttonsData & 0x80)
	{
		if (fioKeys.ktilt_flag == 0)
		{
			diEvent.ki.dwFlags = S_TILT_FLAGS;
			diEvent.ki.wScan = S_TILT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.ktilt_flag = 1;
			LOG_VERBOSE("Pressed: Tilt\n");
		}
	}
	else if (fioKeys.ktilt_flag == 1)
	{
		diEvent.ki.dwFlags = S_TILT_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = S_TILT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.ktilt_flag = 0;
		LOG_VERBOSE("Released: Tilt\n");
	}

	// Player 1 Up
	if (buttonsData & 0x100)
	{
		if (fioKeys.kup_flag == 0)
		{
			diEvent.ki.dwFlags = P1_UP_FLAGS;
			diEvent.ki.wScan = P1_UP;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kup_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Up\n");
		}
	}
	else if (fioKeys.kup_flag == 1)
	{
		diEvent.ki.dwFlags = P1_UP_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_UP;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kup_flag = 0;
		LOG_VERBOSE("Released: Player 1 Up\n");
	}

	// Player 2 Up
	if (buttonsData & 0x200)
	{
		if (fioKeys.k2up_flag == 0)
		{
			diEvent.ki.dwFlags = P2_UP_FLAGS;
			diEvent.ki.wScan = P2_UP;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2up_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Up\n");
		}
	}
	else if (fioKeys.k2up_flag == 1)
	{
		diEvent.ki.dwFlags = P2_UP_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_UP;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2up_flag = 0;
		LOG_VERBOSE("Released: Player 2 Up\n");
	}

	// Player 1 Down
	if (buttonsData & 0x400)
	{
		if (fioKeys.kdown_flag == 0)
		{
			diEvent.ki.dwFlags = P1_DOWN_FLAGS;
			diEvent.ki.wScan = P1_DOWN;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kdown_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Down\n");
		}
	}
	else if (fioKeys.kdown_flag == 1)
	{
		diEvent.ki.dwFlags = P1_DOWN_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_DOWN;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kdown_flag = 0;
		LOG_VERBOSE("Released: Player 1 Down\n");
	}

	// Player 2 Down
	if (buttonsData & 0x800)
	{
		if (fioKeys.k2down_flag == 0)
		{
			diEvent.ki.dwFlags = P2_DOWN_FLAGS;
			diEvent.ki.wScan = P2_DOWN;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2down_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Down\n");
		}
	}
	else if (fioKeys.k2down_flag == 1)
	{
		diEvent.ki.dwFlags = P2_DOWN_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_DOWN;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2down_flag = 0;
		LOG_VERBOSE("Released: Player 2 Down\n");
	}

	// Player 1 Left
	if (buttonsData & 0x1000)
	{
		if (fioKeys.kleft_flag == 0)
		{
			diEvent.ki.dwFlags = P1_LEFT_FLAGS;
			diEvent.ki.wScan = P1_LEFT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kleft_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Left\n");
		}
	}
	else if (fioKeys.kleft_flag == 1)
	{
		diEvent.ki.dwFlags = P1_LEFT_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_LEFT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kleft_flag = 0;
		LOG_VERBOSE("Released: Player 1 Left\n");
	}

	// Player 2 Left
	if (buttonsData & 0x2000)
	{
		if (fioKeys.k2left_flag == 0)
		{
			diEvent.ki.dwFlags = P2_LEFT_FLAGS;
			diEvent.ki.wScan = P2_LEFT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2left_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Left\n");
		}
	}
	else if (fioKeys.k2left_flag == 1)
	{
		diEvent.ki.dwFlags = P2_LEFT_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_LEFT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2left_flag = 0;
		LOG_VERBOSE("Released: Player 2 Left\n");
	}

	// Player 1 Right
	if (buttonsData & 0x4000)
	{
		if (fioKeys.kright_flag == 0)
		{
			diEvent.ki.dwFlags = P1_RIGHT_FLAGS;
			diEvent.ki.wScan = P1_RIGHT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kright_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Right\n");
		}
	}
	else if (fioKeys.kright_flag == 1)
	{
		diEvent.ki.dwFlags = P1_RIGHT_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_RIGHT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kright_flag = 0;
		LOG_VERBOSE("Released: Player 1 Right\n");
	}

	// Player 2 Right
	if (buttonsData & 0x8000)
	{
		if (fioKeys.k2right_flag == 0)
		{
			diEvent.ki.dwFlags = P2_RIGHT_FLAGS;
			diEvent.ki.wScan = P2_RIGHT;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2right_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Right\n");
		}
	}
	else if (fioKeys.k2right_flag == 1)
	{
		diEvent.ki.dwFlags = P2_RIGHT_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_RIGHT;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2right_flag = 0;
		LOG_VERBOSE("Released: Player 2 Right\n");
	}

	// Player 1 Button 1
	if (buttonsData & 0x10000)
	{
		if (fioKeys.kb1_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN1_FLAGS;
			diEvent.ki.wScan = P1_BTN1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb1_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 1\n");
		}
	}
	else if (fioKeys.kb1_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN1_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb1_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 1\n");
	}

	// Player 2 Button 1
	if (buttonsData & 0x20000)
	{
		if (fioKeys.k2b1_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN1_FLAGS;
			diEvent.ki.wScan = P2_BTN1;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b1_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 1\n");
		}
	}
	else if (fioKeys.k2b1_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN1_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN1;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b1_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 1\n");
	}

	// Player 1 Button 2
	if (buttonsData & 0x40000)
	{
		if (fioKeys.kb2_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN2_FLAGS;
			diEvent.ki.wScan = P1_BTN2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb2_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 2\n");
		}
	}
	else if (fioKeys.kb2_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN2_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb2_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 2\n");
	}

	// Player 2 Button 2
	if (buttonsData & 0x80000)
	{
		if (fioKeys.k2b2_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN2_FLAGS;
			diEvent.ki.wScan = P2_BTN2;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b2_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 2\n");
		}
	}
	else if (fioKeys.k2b2_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN2_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN2;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b2_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 2\n");
	}

	// Player 1 Button 3
	if (buttonsData & 0x100000)
	{
		if (fioKeys.kb3_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN3_FLAGS;
			diEvent.ki.wScan = P1_BTN3;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb3_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 3\n");
		}
	}
	else if (fioKeys.kb3_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN3_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN3;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb3_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 3\n");
	}

	// Player 2 Button 3
	if (buttonsData & 0x200000)
	{
		if (fioKeys.k2b3_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN3_FLAGS;
			diEvent.ki.wScan = P2_BTN3;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b3_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 3\n");
		}
	}
	else if (fioKeys.k2b3_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN3_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN3;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b3_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 3\n");
	}

	// Player 1 Button 4
	if (buttonsData & 0x400000)
	{
		if (fioKeys.kb4_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN4_FLAGS;
			diEvent.ki.wScan = P1_BTN4;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb4_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 4\n");
		}
	}
	else if (fioKeys.kb4_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN4_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN4;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb4_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 4\n");
	}

	// Player 2 Button 4
	if (buttonsData & 0x800000)
	{
		if (fioKeys.k2b4_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN4_FLAGS;
			diEvent.ki.wScan = P2_BTN4;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b4_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 4\n");
		}
	}
	else if (fioKeys.k2b4_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN4_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN4;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b4_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 4\n");
	}

	// Player 1 Button 5
	if (buttonsData & 0x1000000)
	{
		if (fioKeys.kb5_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN5_FLAGS;
			diEvent.ki.wScan = P1_BTN5;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb5_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 5\n");
		}
	}
	else if (fioKeys.kb5_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN5_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN5;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb5_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 5\n");
	}

	// Player 2 Button 5
	if (buttonsData & 0x2000000)
	{
		if (fioKeys.k2b5_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN5_FLAGS;
			diEvent.ki.wScan = P2_BTN5;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b5_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 5\n");
		}
	}
	else if (fioKeys.k2b5_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN5_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN5;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b5_flag = 0;
		LOG_VERBOSE("Pressed: Player 2 Button 5\n");
	}

	// Player 1 Button 6
	if (buttonsData & 0x4000000)
	{
		if (fioKeys.kb6_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN6_FLAGS;
			diEvent.ki.wScan = P1_BTN6;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb6_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 6\n");
		}
	}
	else if (fioKeys.kb6_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN6_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN6;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb6_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 6\n");
	}

	// Player 2 Button 6
	if (buttonsData & 0x8000000)
	{
		if (fioKeys.k2b6_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN6_FLAGS;
			diEvent.ki.wScan = P2_BTN6;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b6_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 6\n");
		}
	}
	else if (fioKeys.k2b6_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN6_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN6;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b6_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 6\n");
	}

	// Player 1 Button 7
	if (buttonsData & 0x10000000)
	{
		if (fioKeys.kb7_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN7_FLAGS;
			diEvent.ki.wScan = P1_BTN7;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb7_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 7\n");
		}
	}
	else if (fioKeys.kb7_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN7_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN7;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb7_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 7\n");
	}

	// Player 2 Button 7
	if (buttonsData & 0x20000000)
	{
		if (fioKeys.k2b7_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN7_FLAGS;
			diEvent.ki.wScan = P2_BTN7;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b7_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 7\n");
		}
	}
	else if (fioKeys.k2b7_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN7_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P2_BTN7;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.k2b7_flag = 0;
		LOG_VERBOSE("Released: Player 2 Button 7\n");
	}

	// Player 1 Button 8
	if (buttonsData & 0x40000000)
	{
		if (fioKeys.kb8_flag == 0)
		{
			diEvent.ki.dwFlags = P1_BTN8_FLAGS;
			diEvent.ki.wScan = P1_BTN8;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.kb8_flag = 1;
			LOG_VERBOSE("Pressed: Player 1 Button 8\n");
		}
	}
	else if (fioKeys.kb8_flag == 1)
	{
		diEvent.ki.dwFlags = P1_BTN8_FLAGS | KEYEVENTF_KEYUP;
		diEvent.ki.wScan = P1_BTN8;
		SendInput(1, &diEvent, sizeof(INPUT));
		fioKeys.kb8_flag = 0;
		LOG_VERBOSE("Released: Player 1 Button 8\n");
	}

	// Player 2 Button 8
	if (buttonsData & 0x80000000)
	{
		if (fioKeys.k2b8_flag == 0)
		{
			diEvent.ki.dwFlags = P2_BTN8_FLAGS;
			diEvent.ki.wScan = P2_BTN8;
			SendInput(1, &diEvent, sizeof(INPUT));
			fioKeys.k2b8_flag = 1;
			LOG_VERBOSE("Pressed: Player 2 Button 8\n");
		}
	}
	else if (fioKeys.k2b8_flag == 1)
	{
		diEvent.ki.dwFlags = P2_BTN8_FLAGS | KEYEVENTF_KEYUP;
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

	if (!FIO_Open())
	{
		DWORD error = GetLastError();
		LOG_ERROR("Failed to open iDmacDrv device. Error code: %d\n", error);
		return 1;
	}

	if (!assignPlayers())
	{
		LOG_ERROR("Failed to detect any I/O connection for any player\n");
	}

	FIOKEYS fioKeys = { 0 };

	INPUT diEvent = { 0 };
	diEvent.type = INPUT_KEYBOARD;

	LOG_VERBOSE("Starting to poll input registers...\n");
	while (keepPolling)
	{
		poll(fioKeys, diEvent);
		Sleep(POLLING_INTERVAL);
	}
	LOG_VERBOSE("Finished polling input registers\n");

	return 0;
}
