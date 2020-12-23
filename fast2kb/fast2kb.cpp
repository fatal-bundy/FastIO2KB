// fast2kb.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifdef KEYMAP_PPSSPP
#include "keymap_ppsspp.h"
#elif KEYMAP_EX_BOARD
#include "keymap_ex_board.h"
#else
#include "keymap_mame.h"
#endif // KEYMAP_PPSSPP



#define VERSION_DESCRIPTION "2020.12.23.00"

#ifdef _WIN64
#define ARCHITECTURE_DESCRIPTION "x64"
#else
#define ARCHITECTURE_DESCRIPTION "x86"
#endif

#ifdef _DEBUG
#define PROFILE_DESCRIPTION "Debug"
#else
#define PROFILE_DESCRIPTION "Release"
#endif


#define STATUS_SUCCESS 0 // NTSTATUS value for success


#ifdef KEYMAP_PPSSPP
// PPSSPP's menu screens cannot handle input at submillisecond interval.
#define POLLING_INTERVAL 1ul // milliseconds
#else
#define POLLING_INTERVAL 0ul // milliseconds
#endif // KEYMAP_PPSSPP
#define POLLING_INTERVAL_RESOLUTION 5000ul // 10000 == 1 millisecond


#define JVS_FRIENDLY

#ifdef JVS_FRIENDLY
#define JVS_PORT L"COM2"

#define JVS_HEARTBEAT_INTERVAL 60000ul // milliseconds
#define JVS_HEARTBEAT_INTERVAL_RESOLUTION 30000ul // milliseconds
#endif // JVS_FRIENDLY


//#define VERBOSE

#ifdef VERBOSE
#include "keymap_verbose.h"
#define LOG_VERBOSE(...) (fprintf_s(stdout, __VA_ARGS__))
#define LOG_LEVEL_DESCRIPTION "Verbose"
#else
#define LOG_VERBOSE(...)
#define LOG_LEVEL_DESCRIPTION "Default"
#endif // VERBOSE

#define LOG_INFO(...) (fprintf_s(stdout, __VA_ARGS__))
#define LOG_ERROR(...) (fprintf_s(stderr, __VA_ARGS__))


#if defined(P3_START)    ||\
    defined(P3_UP)       ||\
    defined(P3_DOWN)     ||\
    defined(P3_LEFT)     ||\
    defined(P3_RIGHT)    ||\
    defined(P3_BTN1)     ||\
    defined(P3_BTN2)     ||\
    defined(P3_BTN3)     ||\
    defined(P3_BTN4)     ||\
    defined(P3_BTN5)     ||\
    defined(P3_BTN6)     ||\
    defined(P3_BTN7)     ||\
    defined(P3_BTN8)     ||\
    defined(P4_START)    ||\
    defined(P4_UP)       ||\
    defined(P4_DOWN)     ||\
    defined(P4_LEFT)     ||\
    defined(P4_RIGHT)    ||\
    defined(P4_BTN1)     ||\
    defined(P4_BTN2)     ||\
    defined(P4_BTN3)     ||\
    defined(P4_BTN4)     ||\
    defined(P4_BTN5)     ||\
    defined(P4_BTN6)     ||\
    defined(P4_BTN7)     ||\
    defined(P4_BTN8)     ||\
    defined(S_COIN3)     ||\
    defined(S_COIN4)     ||\
    defined(S_TEST2)     ||\
    defined(S_TILT2)     ||\
    defined(S_SERVICE3)  ||\
    defined(S_SERVICE4)  ||\
    defined(S_ESC3)      ||\
    defined(S_ESC4)
#define P3P4_ENABLED
#endif



typedef NTSTATUS(__stdcall *ntQueryTimerRes)(PULONG MinimumResolution,
    PULONG MaximumResolution, PULONG CurrentResolution);
typedef NTSTATUS(__stdcall *ntSetTimerRes)(ULONG DesiredResolution,
    BOOLEAN SetResolution, PULONG CurrentResolution);

HMODULE ntdll;

ntQueryTimerRes NtQueryTimerResolution;
ntSetTimerRes NtSetTimerResolution;


typedef int(*dmacOpen)(int, LPVOID, LPVOID);
typedef int(*dmacRead)(int, DWORD, LPVOID, LPVOID);
typedef int(*dmacWrite)(int, DWORD, int, LPVOID);
typedef int(*dmacClose)(int, LPVOID);

HMODULE iDmacDrv;

dmacOpen iDmacDrvOpen;
dmacRead iDmacDrvRegisterRead;
dmacWrite iDmacDrvRegisterWrite;
dmacClose iDmacDrvClose;


int deviceIndex = 1;
int deviceId;
int buttonsAddressP1P2;
int buttonsAddressP3P4;

HANDLE hMainThread;
BOOL keepPolling = TRUE;



BOOL loadNtdll()
{
    ntdll = LoadLibrary(TEXT("ntdll.dll"));
    if (ntdll == NULL)
    {
        LOG_ERROR("Failed to load ntdll.dll. Error code: %d\n", GetLastError());
        return FALSE;
    }

    NtQueryTimerResolution = (ntQueryTimerRes)GetProcAddress(
        ntdll, "NtQueryTimerResolution");
    if (NtQueryTimerResolution == NULL)
    {
        LOG_ERROR("Failed to get address of NtQueryTimerResolution. Error code: %d\n", GetLastError());
        return FALSE;
    }

    NtSetTimerResolution = (ntSetTimerRes)GetProcAddress(
        ntdll, "NtSetTimerResolution");
    if (NtSetTimerResolution == NULL)
    {
        LOG_ERROR("Failed to get address of NtSetTimerResolution. Error code: %d\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}


BOOL loadIDmacDrv()
{
#ifdef _WIN64
    iDmacDrv = LoadLibrary(TEXT("iDmacDrv64.dll"));
    if (iDmacDrv == NULL)
    {
        LOG_ERROR("Failed to load iDmacDrv64.dll. Error code: %d\n", GetLastError());
        return FALSE;
    }
#else
    iDmacDrv = LoadLibrary(TEXT("iDmacDrv32.dll"));
    if (iDmacDrv == NULL)
    {
        LOG_ERROR("Failed to load iDmacDrv32.dll. Error code: %d\n", GetLastError());
        return FALSE;
    }
#endif

    iDmacDrvOpen = (dmacOpen)GetProcAddress(iDmacDrv, "iDmacDrvOpen");
    if (iDmacDrvOpen == NULL)
    {
        LOG_ERROR("Failed to get address of iDmacDrvOpen. Error code: %d\n", GetLastError());
        return FALSE;
    }

    iDmacDrvRegisterRead = (dmacRead)GetProcAddress(iDmacDrv, "iDmacDrvRegisterRead");
    if (iDmacDrvRegisterRead == NULL)
    {
        LOG_ERROR("Failed to get address of iDmacRegisterRead. Error code: %d\n", GetLastError());
        return FALSE;
    }

    iDmacDrvRegisterWrite = (dmacWrite)GetProcAddress(iDmacDrv, "iDmacDrvRegisterWrite");
    if (iDmacDrvRegisterWrite == NULL)
    {
        LOG_ERROR("Failed to get address of iDmacRegisterWrite. Error code: %d\n", GetLastError());
        return FALSE;
    }

    iDmacDrvClose = (dmacClose)GetProcAddress(iDmacDrv, "iDmacDrvClose");
    if (iDmacDrvClose == NULL)
    {
        LOG_ERROR("Failed to get address of iDmacDrvClose. Error code: %d\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}


BOOL WINAPI ctrlHandler(DWORD signal)
{
    UNREFERENCED_PARAMETER(signal);

    keepPolling = FALSE;
    WaitForSingleObject(hMainThread, INFINITE);
    CloseHandle(hMainThread);

    return TRUE;
}


#ifdef JVS_FRIENDLY
HANDLE JVS_Open()
{
    HANDLE hCom = CreateFile(JVS_PORT, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hCom == INVALID_HANDLE_VALUE)
    {
        // This might not be an error per se, so it is not logged as an error.
        // Rather, it might just mean that no JVS device is present.
        LOG_VERBOSE("JVS_Open: CreateFile failed. Error code: %d\n", GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    DWORD errors;
    if (!ClearCommError(hCom, &errors, NULL))
    {
        LOG_ERROR("JVS_Open: ClearCommError failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    if (!SetupComm(hCom, 516ul, 516ul))
    {
        LOG_ERROR("JVS_Open: SetupComm failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(hCom, &dcb))
    {
        LOG_ERROR("JVS_Open: GetCommState failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8u;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hCom, &dcb))
    {
        LOG_ERROR("JVS_Open: SetCommState failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    if (!EscapeCommFunction(hCom, CLRRTS)) // Not sending
    {
        LOG_ERROR("JVS_Open: EscapeCommFunction failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    if (!EscapeCommFunction(hCom, SETDTR)) // Open for communications
    {
        LOG_ERROR("JVS_Open: EscapeCommFunction failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    if (!SetCommMask(hCom, EV_RXCHAR))
    {
        LOG_ERROR("JVS_Open: SetCommMask failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS comTimeouts = { 0 };
    if (!SetCommTimeouts(hCom, &comTimeouts))
    {
        LOG_ERROR("JVS_Open: SetCommTimeouts failed. Error code: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    return hCom;
}


void JVS_Close(HANDLE hCom)
{
    EscapeCommFunction(hCom, CLRDTR); // Closed for communications
    CloseHandle(hCom);
}


BOOL JVS_Write(HANDLE hCom, const byte *message, DWORD messageLength)
{
    if (!EscapeCommFunction(hCom, SETRTS)) // Not sending (yet)
    {
        LOG_ERROR("JVS_Write: EscapeCommFunction failed. Error code: %d\n", GetLastError());
        return FALSE;
    }

    DWORD numBytesWritten;
    if (!WriteFile(hCom, message, messageLength, &numBytesWritten, NULL))
    {
        LOG_ERROR("JVS_Write: WriteFile failed. Error code: %d\n", GetLastError());
        return FALSE;
    }

    if (numBytesWritten != messageLength)
    {
        LOG_ERROR("JVS_Write: WriteFile failed to write enough bytes: messageLength=%d, numBytesWritten=%d\n",
            messageLength, numBytesWritten);
        return FALSE;
    }

    if (!FlushFileBuffers(hCom))
    {
        LOG_ERROR("JVS_Write: FlushFileBuffers failed. Error code: %d\n", GetLastError());
        return FALSE;
    }

    DWORD errors;
    if (!ClearCommError(hCom, &errors, NULL))
    {
        LOG_ERROR("JVS_Write: ClearCommError failed. Error code: %d\n", GetLastError());
        return FALSE;
    }

    if (!EscapeCommFunction(hCom, SETRTS)) // Sending
    {
        LOG_ERROR("JVS_Write: EscapeCommFunction failed. Error code: %d\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}


BOOL JVS_Reset(HANDLE hCom)
{
    static const DWORD resetMessageLength = 6ul;
    static const byte resetMessage[resetMessageLength] = {
        0xe0, 0xff, 0x03, 0xf0, 0xd9, 0xcb
    };

    if (!JVS_Write(hCom, resetMessage, resetMessageLength))
    {
        LOG_ERROR("JVS_Reset: Failed\n");
        return FALSE;
    }

    LOG_VERBOSE("JVS_Reset: Succeeded\n");
    return TRUE;
}


BOOL JVS_Register(HANDLE hCom)
{
    static const DWORD registerMessageLength = 6ul;
    static const byte registerMessage[registerMessageLength] = {
        0xe0, 0xff, 0x03, 0xf1, 0x01, 0xf4
    };

    if (!JVS_Write(hCom, registerMessage, registerMessageLength))
    {
        LOG_ERROR("JVS_Register: Failed\n");
        return FALSE;
    }

    LOG_VERBOSE("JVS_Register: Succeeded\n");
    return TRUE;
}


void CALLBACK JVS_HeartbeatCallback(UINT uTimerID, UINT uMsg,
    DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    UNREFERENCED_PARAMETER(uTimerID);
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(dw1);
    UNREFERENCED_PARAMETER(dw2);

    HANDLE hCom = (HANDLE)dwUser;
    JVS_Reset(hCom);
}
#endif // JVS_FRIENDLY


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
        LOG_VERBOSE("Fast I/O Port 1: Player 1 + Player 2\n");
        if (port2HasConnection)
        {
            buttonsAddressP3P4 = 0x41a0;
            LOG_VERBOSE("Fast I/O Port 2: Player 3 + Player 4\n");
        }
        else
        {
            buttonsAddressP3P4 = NULL;
            LOG_VERBOSE("Fast I/O Port 2: Empty\n");
        }
    }
    else
    {
        buttonsAddressP3P4 = NULL;
        LOG_VERBOSE("Fast I/O Port 1: Empty\n");
        if (port2HasConnection)
        {
            buttonsAddressP1P2 = 0x41a0;
            LOG_VERBOSE("Fast I/O Port 2: Player 1 + Player 2\n");
        }
        else
        {
            buttonsAddressP1P2 = NULL;
            LOG_VERBOSE("Fast I/O Port 2: Empty\n");
            return FALSE;
        }
    }

    return TRUE;
}


void pollP1P2(int &buttonsData, int &prevButtonsData, INPUT &diEvent)
{
    prevButtonsData = buttonsData;
    if (!FIO_RegRead(buttonsAddressP1P2, buttonsData) ||
        buttonsData == prevButtonsData)
    {
        // The read failed or none of the button states have changed.
        return;
    }
    LOG_VERBOSE("pollP1P2: buttonsData=0x%x\n", buttonsData);

    int xorButtonsData = buttonsData ^ prevButtonsData;

#ifdef S_ESC1
    // Player 1 Start + Player 1 Button 1 + Player 1 Button 3
    if (xorButtonsData & 0x10 || xorButtonsData & 0x10000 || xorButtonsData & 0x100000)
    {
        if (buttonsData & 0x10 && buttonsData & 0x10000 && buttonsData & 0x100000)
        {
            LOG_VERBOSE("Pressed: Escape 1 (Player 1 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC1_FLAGS;
            diEvent.ki.wScan = S_ESC1;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
        else if (prevButtonsData & 0x10 && prevButtonsData & 0x10000 && prevButtonsData & 0x100000)
        {
            LOG_VERBOSE("Released: Escape 1 (Player 1 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC1_FLAGS | KEYEVENTF_KEYUP;
            diEvent.ki.wScan = S_ESC1;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
    }
#endif // S_ESC1

#ifdef S_ESC2
    // Player 2 Start + Player 2 Button 1 + Player 2 Button 3
    if (xorButtonsData & 0x20 || xorButtonsData & 0x20000 || xorButtonsData & 0x200000)
    {
        if (buttonsData & 0x20 && buttonsData & 0x20000 && buttonsData & 0x200000)
        {
            LOG_VERBOSE("Pressed: Escape 2 (Player 2 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC2_FLAGS;
            diEvent.ki.wScan = S_ESC2;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
        else if (prevButtonsData & 0x20 && prevButtonsData & 0x20000 && prevButtonsData & 0x200000)
        {
            LOG_VERBOSE("Released: Escape 2 (Player 2 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC2_FLAGS | KEYEVENTF_KEYUP;
            diEvent.ki.wScan = S_ESC2;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
    }
#endif // S_ESC2

#ifdef S_COIN1
    // Coin 1
    if (xorButtonsData & 0x1)
    {
        if (buttonsData & 0x1)
        {
            LOG_VERBOSE("Pressed: Coin 1\n");
            diEvent.ki.dwFlags = S_COIN1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Coin 1\n");
            diEvent.ki.dwFlags = S_COIN1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_COIN1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_COIN1

#ifdef S_COIN2
    // Coin 2
    if (xorButtonsData & 0x2)
    {
        if (buttonsData & 0x2)
        {
            LOG_VERBOSE("Pressed: Coin 2\n");
            diEvent.ki.dwFlags = S_COIN2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Coin 2\n");
            diEvent.ki.dwFlags = S_COIN2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_COIN2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_COIN2

#ifdef S_SERVICE1
    // Service 1
    if (xorButtonsData & 0x4)
    {
        if (buttonsData & 0x4)
        {
            LOG_VERBOSE("Pressed: Service 1\n");
            diEvent.ki.dwFlags = S_SERVICE1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Service 1\n");
            diEvent.ki.dwFlags = S_SERVICE1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_SERVICE1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_SERVICE1

#ifdef S_SERVICE2
    // Service 2
    if (xorButtonsData & 0x8)
    {
        if (buttonsData & 0x8)
        {
            LOG_VERBOSE("Pressed: Service 2\n");
            diEvent.ki.dwFlags = S_SERVICE2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Service 2\n");
            diEvent.ki.dwFlags = S_SERVICE2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_SERVICE2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_SERVICE2

#ifdef P1_START
    // Player 1 Start
    if (xorButtonsData & 0x10)
    {
        if (buttonsData & 0x10)
        {
            LOG_VERBOSE("Pressed: Player 1 Start\n");
            diEvent.ki.dwFlags = P1_START_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Start\n");
            diEvent.ki.dwFlags = P1_START_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_START;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_START

#ifdef P2_START
    // Player 2 Start
    if (xorButtonsData & 0x20)
    {
        if (buttonsData & 0x20)
        {
            LOG_VERBOSE("Pressed: Player 2 Start\n");
            diEvent.ki.dwFlags = P2_START_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Start\n");
            diEvent.ki.dwFlags = P2_START_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_START;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_START

#ifdef S_TEST1
    // Test 1
    if (xorButtonsData & 0x40)
    {
        if (buttonsData & 0x40)
        {
            LOG_VERBOSE("Pressed: Test 1\n");
            diEvent.ki.dwFlags = S_TEST1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Test 1\n");
            diEvent.ki.dwFlags = S_TEST1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_TEST1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_TEST1

#ifdef S_TILT1
    // Tilt 1
    if (xorButtonsData & 0x80)
    {
        if (buttonsData & 0x80)
        {
            LOG_VERBOSE("Pressed: Tilt 1\n");
            diEvent.ki.dwFlags = S_TILT1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Tilt 1\n");
            diEvent.ki.dwFlags = S_TILT1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_TILT1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_TILT1

#ifdef P1_UP
    // Player 1 Up
    if (xorButtonsData & 0x100)
    {
        if (buttonsData & 0x100)
        {
            LOG_VERBOSE("Pressed: Player 1 Up\n");
            diEvent.ki.dwFlags = P1_UP_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Up\n");
            diEvent.ki.dwFlags = P1_UP_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_UP;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_UP

#ifdef P2_UP
    // Player 2 Up
    if (xorButtonsData & 0x200)
    {
        if (buttonsData & 0x200)
        {
            LOG_VERBOSE("Pressed: Player 2 Up\n");
            diEvent.ki.dwFlags = P2_UP_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Up\n");
            diEvent.ki.dwFlags = P2_UP_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_UP;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_UP

#ifdef P1_DOWN
    // Player 1 Down
    if (xorButtonsData & 0x400)
    {
        if (buttonsData & 0x400)
        {
            LOG_VERBOSE("Pressed: Player 1 Down\n");
            diEvent.ki.dwFlags = P1_DOWN_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Down\n");
            diEvent.ki.dwFlags = P1_DOWN_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_DOWN;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_DOWN

#ifdef P2_DOWN
    // Player 2 Down
    if (xorButtonsData & 0x800)
    {
        if (buttonsData & 0x800)
        {
            LOG_VERBOSE("Pressed: Player 2 Down\n");
            diEvent.ki.dwFlags = P2_DOWN_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Down\n");
            diEvent.ki.dwFlags = P2_DOWN_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_DOWN;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_DOWN

#ifdef P1_LEFT
    // Player 1 Left
    if (xorButtonsData & 0x1000)
    {
        if (buttonsData & 0x1000)
        {
            LOG_VERBOSE("Pressed: Player 1 Left\n");
            diEvent.ki.dwFlags = P1_LEFT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Left\n");
            diEvent.ki.dwFlags = P1_LEFT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_LEFT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_LEFT

#ifdef P2_LEFT
    // Player 2 Left
    if (xorButtonsData & 0x2000)
    {
        if (buttonsData & 0x2000)
        {
            LOG_VERBOSE("Pressed: Player 2 Left\n");
            diEvent.ki.dwFlags = P2_LEFT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Left\n");
            diEvent.ki.dwFlags = P2_LEFT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_LEFT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_LEFT

#ifdef P1_RIGHT
    // Player 1 Right
    if (xorButtonsData & 0x4000)
    {
        if (buttonsData & 0x4000)
        {
            LOG_VERBOSE("Pressed: Player 1 Right\n");
            diEvent.ki.dwFlags = P1_RIGHT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Right\n");
            diEvent.ki.dwFlags = P1_RIGHT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_RIGHT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_RIGHT

#ifdef P2_RIGHT
    // Player 2 Right
    if (xorButtonsData & 0x8000)
    {
        if (buttonsData & 0x8000)
        {
            LOG_VERBOSE("Pressed: Player 2 Right\n");
            diEvent.ki.dwFlags = P2_RIGHT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Right\n");
            diEvent.ki.dwFlags = P2_RIGHT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_RIGHT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_RIGHT

#ifdef P1_BTN1
    // Player 1 Button 1
    if (xorButtonsData & 0x10000)
    {
        if (buttonsData & 0x10000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 1\n");
            diEvent.ki.dwFlags = P1_BTN1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 1\n");
            diEvent.ki.dwFlags = P1_BTN1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN1

#ifdef P2_BTN1
    // Player 2 Button 1
    if (xorButtonsData & 0x20000)
    {
        if (buttonsData & 0x20000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 1\n");
            diEvent.ki.dwFlags = P2_BTN1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 1\n");
            diEvent.ki.dwFlags = P2_BTN1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN1

#ifdef P1_BTN2
    // Player 1 Button 2
    if (xorButtonsData & 0x40000)
    {
        if (buttonsData & 0x40000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 2\n");
            diEvent.ki.dwFlags = P1_BTN2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 2\n");
            diEvent.ki.dwFlags = P1_BTN2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN2

#ifdef P2_BTN2
    // Player 2 Button 2
    if (xorButtonsData & 0x80000)
    {
        if (buttonsData & 0x80000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 2\n");
            diEvent.ki.dwFlags = P2_BTN2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 2\n");
            diEvent.ki.dwFlags = P2_BTN2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN2

#ifdef P1_BTN3
    // Player 1 Button 3
    if (xorButtonsData & 0x100000)
    {
        if (buttonsData & 0x100000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 3\n");
            diEvent.ki.dwFlags = P1_BTN3_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 3\n");
            diEvent.ki.dwFlags = P1_BTN3_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN3;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN3

#ifdef P2_BTN3
    // Player 2 Button 3
    if (xorButtonsData & 0x200000)
    {
        if (buttonsData & 0x200000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 3\n");
            diEvent.ki.dwFlags = P2_BTN3_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 3\n");
            diEvent.ki.dwFlags = P2_BTN3_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN3;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN3

#ifdef P1_BTN4
    // Player 1 Button 4
    if (xorButtonsData & 0x400000)
    {
        if (buttonsData & 0x400000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 4\n");
            diEvent.ki.dwFlags = P1_BTN4_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 4\n");
            diEvent.ki.dwFlags = P1_BTN4_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN4;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN4

#ifdef P2_BTN4
    // Player 2 Button 4
    if (xorButtonsData & 0x800000)
    {
        if (buttonsData & 0x800000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 4\n");
            diEvent.ki.dwFlags = P2_BTN4_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 4\n");
            diEvent.ki.dwFlags = P2_BTN4_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN4;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN4

#ifdef P1_BTN5
    // Player 1 Button 5
    if (xorButtonsData & 0x1000000)
    {
        if (buttonsData & 0x1000000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 5\n");
            diEvent.ki.dwFlags = P1_BTN5_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 5\n");
            diEvent.ki.dwFlags = P1_BTN5_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN5;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN5

#ifdef P2_BTN5
    // Player 2 Button 5
    if (xorButtonsData & 0x2000000)
    {
        if (buttonsData & 0x2000000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 5\n");
            diEvent.ki.dwFlags = P2_BTN5_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 5\n");
            diEvent.ki.dwFlags = P2_BTN5_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN5;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN5

#ifdef P1_BTN6
    // Player 1 Button 6
    if (xorButtonsData & 0x4000000)
    {
        if (buttonsData & 0x4000000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 6\n");
            diEvent.ki.dwFlags = P1_BTN6_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 6\n");
            diEvent.ki.dwFlags = P1_BTN6_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN6;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN6

#ifdef P2_BTN6
    // Player 2 Button 6
    if (xorButtonsData & 0x8000000)
    {
        if (buttonsData & 0x8000000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 6\n");
            diEvent.ki.dwFlags = P2_BTN6_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 6\n");
            diEvent.ki.dwFlags = P2_BTN6_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN6;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN6

#ifdef P1_BTN7
    // Player 1 Button 7
    if (xorButtonsData & 0x10000000)
    {
        if (buttonsData & 0x10000000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 7\n");
            diEvent.ki.dwFlags = P1_BTN7_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 7\n");
            diEvent.ki.dwFlags = P1_BTN7_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN7;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN7

#ifdef P2_BTN7
    // Player 2 Button 7
    if (xorButtonsData & 0x20000000)
    {
        if (buttonsData & 0x20000000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 7\n");
            diEvent.ki.dwFlags = P2_BTN7_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 7\n");
            diEvent.ki.dwFlags = P2_BTN7_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN7;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN7

#ifdef P1_BTN8
    // Player 1 Button 8
    if (xorButtonsData & 0x40000000)
    {
        if (buttonsData & 0x40000000)
        {
            LOG_VERBOSE("Pressed: Player 1 Button 8\n");
            diEvent.ki.dwFlags = P1_BTN8_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 1 Button 8\n");
            diEvent.ki.dwFlags = P1_BTN8_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P1_BTN8;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P1_BTN8

#ifdef P2_BTN8
    // Player 2 Button 8
    if (xorButtonsData & 0x80000000)
    {
        if (buttonsData & 0x80000000)
        {
            LOG_VERBOSE("Pressed: Player 2 Button 8\n");
            diEvent.ki.dwFlags = P2_BTN8_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 2 Button 8\n");
            diEvent.ki.dwFlags = P2_BTN8_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P2_BTN8;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P2_BTN8
}


#ifdef P3P4_ENABLED
void pollP3P4(int& buttonsData, int& prevButtonsData, INPUT& diEvent)
{
    prevButtonsData = buttonsData;
    if (!FIO_RegRead(buttonsAddressP3P4, buttonsData) ||
        buttonsData == prevButtonsData)
    {
        // The read failed or none of the button states have changed.
        return;
    }
    LOG_VERBOSE("pollP3P4: buttonsData=0x%x\n", buttonsData);

    int xorButtonsData = buttonsData ^ prevButtonsData;

#ifdef S_ESC3
    // Player 3 Start + Player 3 Button 1 + Player 3 Button 3
    if (xorButtonsData & 0x10 || xorButtonsData & 0x10000 || xorButtonsData & 0x100000)
    {
        if (buttonsData & 0x10 && buttonsData & 0x10000 && buttonsData & 0x100000)
        {
            LOG_VERBOSE("Pressed: Escape 3 (Player 3 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC3_FLAGS;
            diEvent.ki.wScan = S_ESC3;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
        else if (prevButtonsData & 0x10 && prevButtonsData & 0x10000 && prevButtonsData & 0x100000)
        {
            LOG_VERBOSE("Released: Escape 3 (Player 3 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC3_FLAGS | KEYEVENTF_KEYUP;
            diEvent.ki.wScan = S_ESC3;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
    }
#endif // S_ESC3

#ifdef S_ESC4
    // Player 4 Start + Player 4 Button 1 + Player 4 Button 3
    if (xorButtonsData & 0x20 || xorButtonsData & 0x20000 || xorButtonsData & 0x200000)
    {
        if (buttonsData & 0x20 && buttonsData & 0x20000 && buttonsData & 0x200000)
        {
            LOG_VERBOSE("Pressed: Escape 4 (Player 4 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC4_FLAGS;
            diEvent.ki.wScan = S_ESC4;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
        else if (prevButtonsData & 0x20 && prevButtonsData & 0x20000 && prevButtonsData & 0x200000)
        {
            LOG_VERBOSE("Released: Escape 4 (Player 4 Start + Button 1 + Button 3)\n");
            diEvent.ki.dwFlags = S_ESC4_FLAGS | KEYEVENTF_KEYUP;
            diEvent.ki.wScan = S_ESC4;
            SendInput(1, &diEvent, sizeof(INPUT));
        }
    }
#endif // S_ESC4

#ifdef S_COIN3
    // Coin 3
    if (xorButtonsData & 0x1)
    {
        if (buttonsData & 0x1)
        {
            LOG_VERBOSE("Pressed: Coin 3\n");
            diEvent.ki.dwFlags = S_COIN3_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Coin 3\n");
            diEvent.ki.dwFlags = S_COIN3_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_COIN3;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_COIN3

#ifdef S_COIN4
    // Coin 4
    if (xorButtonsData & 0x2)
    {
        if (buttonsData & 0x2)
        {
            LOG_VERBOSE("Pressed: Coin 4\n");
            diEvent.ki.dwFlags = S_COIN4_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Coin 4\n");
            diEvent.ki.dwFlags = S_COIN4_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_COIN4;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_COIN4

#ifdef S_SERVICE3
    // Service 3
    if (xorButtonsData & 0x4)
    {
        if (buttonsData & 0x4)
        {
            LOG_VERBOSE("Pressed: Service 3\n");
            diEvent.ki.dwFlags = S_SERVICE3_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Service 3\n");
            diEvent.ki.dwFlags = S_SERVICE3_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_SERVICE3;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_SERVICE3

#ifdef S_SERVICE4
    // Service 4
    if (xorButtonsData & 0x8)
    {
        if (buttonsData & 0x8)
        {
            LOG_VERBOSE("Pressed: Service 4\n");
            diEvent.ki.dwFlags = S_SERVICE4_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Service 4\n");
            diEvent.ki.dwFlags = S_SERVICE4_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_SERVICE4;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_SERVICE4

#ifdef P3_START
    // Player 3 Start
    if (xorButtonsData & 0x10)
    {
        if (buttonsData & 0x10)
        {
            LOG_VERBOSE("Pressed: Player 3 Start\n");
            diEvent.ki.dwFlags = P3_START_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Start\n");
            diEvent.ki.dwFlags = P3_START_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_START;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_START

#ifdef P4_START
    // Player 4 Start
    if (xorButtonsData & 0x20)
    {
        if (buttonsData & 0x20)
        {
            LOG_VERBOSE("Pressed: Player 4 Start\n");
            diEvent.ki.dwFlags = P4_START_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Start\n");
            diEvent.ki.dwFlags = P4_START_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_START;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_START

#ifdef S_TEST2
    // Test 2
    if (xorButtonsData & 0x40)
    {
        if (buttonsData & 0x40)
        {
            LOG_VERBOSE("Pressed: Test 2\n");
            diEvent.ki.dwFlags = S_TEST2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Test 2\n");
            diEvent.ki.dwFlags = S_TEST2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_TEST2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_TEST2

#ifdef S_TILT2
    // Tilt 2
    if (xorButtonsData & 0x80)
    {
        if (buttonsData & 0x80)
        {
            LOG_VERBOSE("Pressed: Tilt 2\n");
            diEvent.ki.dwFlags = S_TILT2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Tilt 2\n");
            diEvent.ki.dwFlags = S_TILT2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = S_TILT2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // S_TILT2

#ifdef P3_UP
    // Player 3 Up
    if (xorButtonsData & 0x100)
    {
        if (buttonsData & 0x100)
        {
            LOG_VERBOSE("Pressed: Player 3 Up\n");
            diEvent.ki.dwFlags = P3_UP_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Up\n");
            diEvent.ki.dwFlags = P3_UP_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_UP;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_UP

#ifdef P4_UP
    // Player 4 Up
    if (xorButtonsData & 0x200)
    {
        if (buttonsData & 0x200)
        {
            LOG_VERBOSE("Pressed: Player 4 Up\n");
            diEvent.ki.dwFlags = P4_UP_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Up\n");
            diEvent.ki.dwFlags = P4_UP_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_UP;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_UP

#ifdef P3_DOWN
    // Player 3 Down
    if (xorButtonsData & 0x400)
    {
        if (buttonsData & 0x400)
        {
            diEvent.ki.dwFlags = P3_DOWN_FLAGS;
            LOG_VERBOSE("Pressed: Player 3 Down\n");
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Down\n");
            diEvent.ki.dwFlags = P3_DOWN_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_DOWN;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_DOWN

#ifdef P4_DOWN
    // Player 4 Down
    if (xorButtonsData & 0x800)
    {
        if (buttonsData & 0x800)
        {
            LOG_VERBOSE("Pressed: Player 4 Down\n");
            diEvent.ki.dwFlags = P4_DOWN_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Down\n");
            diEvent.ki.dwFlags = P4_DOWN_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_DOWN;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_DOWN

#ifdef P3_LEFT
    // Player 3 Left
    if (xorButtonsData & 0x1000)
    {
        if (buttonsData & 0x1000)
        {
            LOG_VERBOSE("Pressed: Player 3 Left\n");
            diEvent.ki.dwFlags = P3_LEFT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Left\n");
            diEvent.ki.dwFlags = P3_LEFT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_LEFT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_LEFT

#ifdef P4_LEFT
    // Player 4 Left
    if (xorButtonsData & 0x2000)
    {
        if (buttonsData & 0x2000)
        {
            LOG_VERBOSE("Pressed: Player 4 Left\n");
            diEvent.ki.dwFlags = P4_LEFT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Left\n");
            diEvent.ki.dwFlags = P4_LEFT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_LEFT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_LEFT

#ifdef P3_RIGHT
    // Player 3 Right
    if (xorButtonsData & 0x4000)
    {
        if (buttonsData & 0x4000)
        {
            LOG_VERBOSE("Pressed: Player 3 Right\n");
            diEvent.ki.dwFlags = P3_RIGHT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Right\n");
            diEvent.ki.dwFlags = P3_RIGHT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_RIGHT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_RIGHT

#ifdef P4_RIGHT
    // Player 4 Right
    if (xorButtonsData & 0x8000)
    {
        if (buttonsData & 0x8000)
        {
            LOG_VERBOSE("Pressed: Player 4 Right\n");
            diEvent.ki.dwFlags = P4_RIGHT_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Right\n");
            diEvent.ki.dwFlags = P4_RIGHT_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_RIGHT;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_RIGHT

#ifdef P3_BTN1
    // Player 3 Button 1
    if (xorButtonsData & 0x10000)
    {
        if (buttonsData & 0x10000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 1\n");
            diEvent.ki.dwFlags = P3_BTN1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 1\n");
            diEvent.ki.dwFlags = P3_BTN1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN1

#ifdef P4_BTN1
    // Player 4 Button 1
    if (xorButtonsData & 0x20000)
    {
        if (buttonsData & 0x20000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 1\n");
            diEvent.ki.dwFlags = P4_BTN1_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 1\n");
            diEvent.ki.dwFlags = P4_BTN1_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN1;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN1

#ifdef P3_BTN2
    // Player 3 Button 2
    if (xorButtonsData & 0x40000)
    {
        if (buttonsData & 0x40000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 2\n");
            diEvent.ki.dwFlags = P3_BTN2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 2\n");
            diEvent.ki.dwFlags = P3_BTN2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN2

#ifdef P4_BTN2
    // Player 4 Button 2
    if (xorButtonsData & 0x80000)
    {
        if (buttonsData & 0x80000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 2\n");
            diEvent.ki.dwFlags = P4_BTN2_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 2\n");
            diEvent.ki.dwFlags = P4_BTN2_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN2;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN2

#ifdef P3_BTN3
    // Player 3 Button 3
    if (xorButtonsData & 0x100000)
    {
        if (buttonsData & 0x100000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 3\n");
            diEvent.ki.dwFlags = P3_BTN3_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 3\n");
            diEvent.ki.dwFlags = P3_BTN3_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN3;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN3

#ifdef P4_BTN3
    // Player 4 Button 3
    if (xorButtonsData & 0x200000)
    {
        if (buttonsData & 0x200000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 3\n");
            diEvent.ki.dwFlags = P4_BTN3_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 3\n");
            diEvent.ki.dwFlags = P4_BTN3_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN3;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN3

#ifdef P3_BTN4
    // Player 3 Button 4
    if (xorButtonsData & 0x400000)
    {
        if (buttonsData & 0x400000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 4\n");
            diEvent.ki.dwFlags = P3_BTN4_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 4\n");
            diEvent.ki.dwFlags = P3_BTN4_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN4;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN4

#ifdef P4_BTN4
    // Player 4 Button 4
    if (xorButtonsData & 0x800000)
    {
        if (buttonsData & 0x800000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 4\n");
            diEvent.ki.dwFlags = P4_BTN4_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 4\n");
            diEvent.ki.dwFlags = P4_BTN4_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN4;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN4

#ifdef P3_BTN5
    // Player 3 Button 5
    if (xorButtonsData & 0x1000000)
    {
        if (buttonsData & 0x1000000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 5\n");
            diEvent.ki.dwFlags = P3_BTN5_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 5\n");
            diEvent.ki.dwFlags = P3_BTN5_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN5;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN5

#ifdef P4_BTN5
    // Player 4 Button 5
    if (xorButtonsData & 0x2000000)
    {
        if (buttonsData & 0x2000000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 5\n");
            diEvent.ki.dwFlags = P4_BTN5_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 5\n");
            diEvent.ki.dwFlags = P4_BTN5_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN5;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN5

#ifdef P3_BTN6
    // Player 3 Button 6
    if (xorButtonsData & 0x4000000)
    {
        if (buttonsData & 0x4000000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 6\n");
            diEvent.ki.dwFlags = P3_BTN6_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 6\n");
            diEvent.ki.dwFlags = P3_BTN6_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN6;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN6

#ifdef P4_BTN6
    // Player 4 Button 6
    if (xorButtonsData & 0x8000000)
    {
        if (buttonsData & 0x8000000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 6\n");
            diEvent.ki.dwFlags = P4_BTN6_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 6\n");
            diEvent.ki.dwFlags = P4_BTN6_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN6;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN6

#ifdef P3_BTN7
    // Player 3 Button 7
    if (xorButtonsData & 0x10000000)
    {
        if (buttonsData & 0x10000000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 7\n");
            diEvent.ki.dwFlags = P3_BTN7_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 7\n");
            diEvent.ki.dwFlags = P3_BTN7_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN7;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN7

#ifdef P4_BTN7
    // Player 4 Button 7
    if (xorButtonsData & 0x20000000)
    {
        if (buttonsData & 0x20000000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 7\n");
            diEvent.ki.dwFlags = P4_BTN7_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 7\n");
            diEvent.ki.dwFlags = P4_BTN7_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN7;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN7

#ifdef P3_BTN8
    // Player 3 Button 8
    if (xorButtonsData & 0x40000000)
    {
        if (buttonsData & 0x40000000)
        {
            LOG_VERBOSE("Pressed: Player 3 Button 8\n");
            diEvent.ki.dwFlags = P3_BTN8_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 3 Button 8\n");
            diEvent.ki.dwFlags = P3_BTN8_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P3_BTN8;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P3_BTN8

#ifdef P4_BTN8
    // Player 4 Button 8
    if (xorButtonsData & 0x80000000)
    {
        if (buttonsData & 0x80000000)
        {
            LOG_VERBOSE("Pressed: Player 4 Button 8\n");
            diEvent.ki.dwFlags = P4_BTN8_FLAGS;
        }
        else
        {
            LOG_VERBOSE("Released: Player 4 Button 8\n");
            diEvent.ki.dwFlags = P4_BTN8_FLAGS | KEYEVENTF_KEYUP;
        }
        diEvent.ki.wScan = P4_BTN8;
        SendInput(1, &diEvent, sizeof(INPUT));
    }
#endif // P4_BTN8
}
#endif // P3P4_ENABLED


int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "/h") == 0)
        {
            LOG_INFO(
                "--------------------------------------------->\n"
                "- FastIO2KB --------------------------------->\n"
                "--------------------------------------------->\n"
                "  Version:       %s\n"
                "  Architecture:  %s\n"
                "  Profile:       %s\n"
                "  Keymap:        %s\n"
                "  Log Level:     %s\n",
                VERSION_DESCRIPTION,
                ARCHITECTURE_DESCRIPTION,
                PROFILE_DESCRIPTION,
                KEYMAP_DESCRIPTION,
                LOG_LEVEL_DESCRIPTION);
            return 0;
        }
    }

    if (!loadNtdll())
    {
        LOG_ERROR("Windows Native API (ntdll) is missing or incompatible\n");
        return 1;
    }

    if (!loadIDmacDrv())
    {
        LOG_ERROR("DMAC Driver Interface (iDmacDrv) is missing or incompatible\n");
        return 1;
    }

    HANDLE pseudoHProcess = GetCurrentProcess();
    HANDLE pseudoHMainThread = GetCurrentThread();

    if (!DuplicateHandle(pseudoHProcess, pseudoHMainThread, pseudoHProcess,
        &hMainThread, NULL, FALSE, DUPLICATE_SAME_ACCESS))
    {
        LOG_ERROR("Failed to get handle to main thread. Error code: %d\n", GetLastError());
        return 1;
    }

    if (!SetConsoleCtrlHandler(ctrlHandler, TRUE))
    {
        LOG_ERROR("Failed to set control handler. Error code: %d\n", GetLastError());
        return 1;
    }

    if (!FIO_Open())
    {
        LOG_ERROR("Failed to open iDmacDrv device. Error code: %d\n", GetLastError());
        return 1;
    }

    if (!assignPlayers())
    {
        LOG_ERROR("Failed to detect Fast I/O connection for any player\n");
        return 1;
    }

#ifdef JVS_FRIENDLY
    // Try to open JVS communications.
    // If a JVS device is present, periodic communications with it might be
    // necessary in order to reset the watchdog timer and prevent the system
    // from power-cycling.
    HANDLE hCom = JVS_Open();
    UINT uTimerID;
    if (hCom == INVALID_HANDLE_VALUE)
    {
        LOG_VERBOSE("JVS Port: Closed\n");
        uTimerID = NULL;
    }
    else
    {
        LOG_VERBOSE("JVS Port: Open\n");
        JVS_Reset(hCom);
        uTimerID = timeSetEvent(JVS_HEARTBEAT_INTERVAL,
            JVS_HEARTBEAT_INTERVAL_RESOLUTION, JVS_HeartbeatCallback,
            (DWORD_PTR)hCom, TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
        if (uTimerID == NULL)
        {
            LOG_ERROR("Failed to set periodic callback for JVS heartbeat\n");
        }
    }
#endif // JVS_FRIENDLY

    int buttonsDataP1P2 = 0x0;
    int prevButtonsDataP1P2 = 0x0;
#ifdef P3P4_ENABLED
    int buttonsDataP3P4 = 0x0;
    int prevButtonsDataP3P4 = 0x0;
#endif

    INPUT diEvent = { 0 };
    diEvent.type = INPUT_KEYBOARD;

    // Determine the preferred timer resolution, within system capabilities.
    ULONG minTimerResolution; // Highest possible delay
    ULONG maxTimerResolution; // Lowest possible delay
    ULONG currentTimerResolution;
    ULONG preferredTimerResolution;
    if (NtQueryTimerResolution(&minTimerResolution, &maxTimerResolution,
            &currentTimerResolution) != STATUS_SUCCESS)
    {
        preferredTimerResolution = POLLING_INTERVAL_RESOLUTION;
    }
    else
    {
        preferredTimerResolution = min(
            max(maxTimerResolution, POLLING_INTERVAL_RESOLUTION),
            minTimerResolution);
    }

    // Ask the system to start using the preferred timer resolution.
    if (NtSetTimerResolution(preferredTimerResolution, TRUE,
            &currentTimerResolution) != STATUS_SUCCESS)
    {
        LOG_VERBOSE("Timer resolution: Unknown\n");
    }
    else
    {
        LOG_VERBOSE("Timer resolution: %.4fms\n",
            preferredTimerResolution * 0.0001);
    }

    LOG_VERBOSE("Starting to poll input registers...\n");
    if (buttonsAddressP3P4 == NULL)
    {
        while (keepPolling)
        {
            pollP1P2(buttonsDataP1P2, prevButtonsDataP1P2, diEvent);
            Sleep(POLLING_INTERVAL);
        }
    }
    else
    {
        while (keepPolling)
        {
            pollP1P2(buttonsDataP1P2, prevButtonsDataP1P2, diEvent);
#ifdef P3P4_ENABLED
            pollP3P4(buttonsDataP3P4, prevButtonsDataP3P4, diEvent);
#endif // P3P4_ENABLED
            Sleep(POLLING_INTERVAL);
        }
    }
    LOG_VERBOSE("Finished polling input registers\n");

#ifdef JVS_FRIENDLY
    if (hCom != INVALID_HANDLE_VALUE)
    {
        if (uTimerID != NULL)
        {
            timeKillEvent(uTimerID);
        }
        JVS_Close(hCom);
    }
#endif // JVS_FRIENDLY

    // Ask the system to stop using the preferred timer resolution.
    NtSetTimerResolution(preferredTimerResolution, FALSE,
        &currentTimerResolution);

    return 0;
}
