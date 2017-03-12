// game.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include "time.h"
#include "stdlib.h"
#include "Class.h"
#include "windows.h"
#pragma comment(lib, "LogitechLEDLib.lib")
#pragma comment(lib, "LogitechGArxControlLib.lib")
#include "LogitechLEDLib.h"
#include "LogitechGArxControlLib.h"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define ROW 4
#define COLUMN 12
#define NUM 4
int diff = 4;
int x = 2, y = 2,isend=0;
Map map = Map();
Bullet b[4];
int flag = 0,step=1;
HANDLE ghSemaphore,mutex;
using namespace LogiLed;
int win_w[] = { KeyName::ONE,KeyName::SIX,KeyName::Q,KeyName::E,KeyName::T,
KeyName::A,KeyName::S,KeyName::D,KeyName::F,KeyName::Z,KeyName::C };
int win_i[] = { KeyName::SEVEN,KeyName::EIGHT,KeyName::NINE,KeyName::V,KeyName::U,KeyName::H,KeyName::N,KeyName::B };
int win_n[] = { KeyName::M,KeyName::K,KeyName::O,KeyName::P,KeyName::ZERO,KeyName::ZERO+3,0x27,0x28,0x1b,0x35 };


const wchar_t *programTitleW = L"logitech-drawkb";
//int lose_l[] = { KeyName::ONE,KeyName::Q,KeyName::A,KeyName::Z };
//int lose_o[] = { KeyName::R,KeyName::T,KeyName::D,KeyName::G,KeyName::C,KeyName::V };
//int lose_s[] = { KeyName::I,KeyName::U,KeyName::J,KeyName::N,KeyName::B };
//int lose_e[] = { KeyName::O,KeyName::K,KeyName::L,KeyName::M+1,KeyName::M+2,KeyName::M+3};

int fail_f[] = { KeyName::ONE, KeyName::ONE + 1, KeyName::ONE + 2, KeyName::Q, KeyName::A, KeyName::S, KeyName::Z };
int fail_a[] = { KeyName::SIX, KeyName::T, KeyName::F, KeyName::C, KeyName::G, KeyName::H, KeyName::N, KeyName::Y };
int fail_i[] = { KeyName::L, KeyName::M+1, KeyName::M+2 , KeyName::O, KeyName::ZERO , KeyName::ZERO-1, };
int fail_l[] = { KeyName::COMMA+2, KeyName::L+2,KeyName::P+1,KeyName::ZERO+2,0x36};

DWORD WINAPI Runner(void* data) {
	char input;
	while (!flag) {
		Sleep(30);
		if (_kbhit()) {
			input = _getch();
			if (step > 0) {
				switch (input)
				{
				case KEY_UP:
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 0, 0, 0);
					x = (x - 1) % ROW;
					if (x < 0)x += ROW;
					break;
				case KEY_LEFT:
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 0, 0, 0);
					y = (y - 1) % COLUMN;
					if (y < 0)y = 0;
					break;
				case KEY_RIGHT:
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 0, 0, 0);
					y = (y + 1) % COLUMN;
					step--;
					break;
				case KEY_DOWN:
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 0, 0, 0);
					x = (x + 1) % ROW;
					break;
				}
				
			}
			fflush(stdin);
		}
		LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 0, 0, 100);
	}

	return 0;
}
DWORD WINAPI bullets(void* data) {
	
	int* j = (int*)data;
	int i = *j;
	DWORD dwWaitResult,wait;
	//printf("%d\n", i);
	while (!flag) {
		dwWaitResult = WaitForSingleObject(
			ghSemaphore,   // handle to semaphore
			0L);           // zero-second time-out interval
		switch (dwWaitResult) {
		case WAIT_OBJECT_0:
			b[i] = Bullet(i);
			while (b[i].get_y() != 0) {
				Sleep(120);
				b[i].move_left(map);
				if (b[i].get_y() == 0) {
					b[i].flush(map);
					step = 1;
				}
			}
			if (!ReleaseSemaphore(
				ghSemaphore,  // handle to semaphore
				1,            // increase count by one
				NULL))       // not interested in previous count
			{
				printf("ReleaseSemaphore error: %d\n", GetLastError());
			}
			break;
		case WAIT_TIMEOUT:
			break;
		}
	}
	wait = WaitForSingleObject(
		mutex,   // handle to semaphore
		0L);           // zero-second time-out interval
	switch (wait) {
	case WAIT_OBJECT_0:
		isend++;
		if (!ReleaseSemaphore(
			mutex,  // handle to semaphore
			1,            // increase count by one
			NULL))       // not interested in previous count
		{
			printf("ReleaseSemaphore error: %d\n", GetLastError());
		}
		break;
	case WAIT_TIMEOUT:
		break; 
	}
	return 0;
}
int main()
{
	// check if running twice same time
	if (FindWindow(NULL, programTitleW)) {
		wprintf(L"%s has been running\n", programTitleW);
		//system("pause");
		return 0;
	}
	SetConsoleTitle(programTitleW);
	
	char input;
	int redVal = 0;
	int greenVal = 0;
	int blueVal = 100;
	/*Bullet b[NUM];
	srand(time(NULL));
	for (int i = 0; i < NUM; i++) {
		b[i] = Bullet(rand()%4);
		Sleep(1);
	}*/

	HANDLE thread = CreateThread(NULL, 0, Runner, &map, 0, NULL);
	HANDLE bu[NUM];
	mutex = CreateSemaphore(
		NULL,           // default security attributes
		1,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore
	ghSemaphore = CreateSemaphore(
		NULL,           // default security attributes
		3,  // initial count
		3,  // maximum count
		NULL);          // unnamed semaphore
	int a[4] = {0,1,2,3};
	for (int i = 0; i < NUM;i++) {
		bu[i]= CreateThread(NULL, 0, bullets, a+(i%4), 0, NULL);
	}
	while (!flag) {
		LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(0, COLUMN - diff), 0, 100, 0);
		LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(1, COLUMN - diff), 0, 100, 0);
		LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(2, COLUMN - diff), 0, 100, 0);
		LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(3, COLUMN - diff), 0, 100, 0);
		for (int i = 0; i < NUM;i++){
			if (map.collision(b[i], x, y)) {
				flag = 1;
				while (isend < 4);
				//printf("GG\n");
				Sleep(500);
				LogiLedSetLighting(0, 0, 0);
				for (int i = 0, *fail = fail_f; i < sizeof(fail_f) / sizeof(fail_f[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)fail[i], 100, 100, 100);
				}
				for (int i = 0, *fail = fail_a; i < sizeof(fail_a) / sizeof(fail_a[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)fail[i], 0, 100, 100);
				}
				for (int i = 0, *fail = fail_i; i < sizeof(fail_i) / sizeof(fail_i[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)fail[i], 100, 0, 100);
				}
				for (int i = 0, *fail = fail_l; i < sizeof(fail_l) / sizeof(fail_l[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)fail[i], 100, 100, 0);
				}
				//return 1;
			}     
			else if (map.win(y)) {
				flag = 1;
				while (isend < 4);
				//printf("GGEZ\n");
				Sleep(500);
				LogiLedSetLighting(0, 0, 0);
				for (int i = 0, *win = win_w; i < sizeof(win_w) / sizeof(win_w[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)win[i], 100, 25, 25);
				}
				//LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)win_i[0], 50, 0, 50); // for checking brightness
				for (int i = 0, *win = win_i; i < sizeof(win_i) / sizeof(win_i[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)win[i], 100, 0, 100);
				}
				for (int i = 0, *win = win_n; i < sizeof(win_n) / sizeof(win_n[0]); i++) {
					LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)win[i], 75, 75, 0);
				}
				//return 1;
			}
		}
	}
	//system("pause");
	Sleep(3000);
    return 0;
}

