#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include "time.h"
#include "stdlib.h"
#pragma comment(lib, "LogitechLEDLib.lib")
#include "LogitechLEDLib.h"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define ROW 4
#define COLUMN 12
extern int diff;
class Map;
class Bullet;
class Bullet {
private:
	int x;
	int y;
public:
	Bullet();
	Bullet(int i);
	int get_x();
	int get_y();
	void flush(Map& map);
	void light(Map& map);
	void move_left(Map& map);
};
class Map {
private:
	int key_val[ROW][COLUMN];
public:
	Map();
	int get(int i, int j);
	int win(int y);
	int collision(Bullet& b,int x,int y);
};
Bullet::Bullet() {
	x = 0;
	y = COLUMN - 1;
}
Bullet::Bullet(int i) {
	x = i;
	y = COLUMN - 1;
}
int Bullet::get_x() { return x; }
int Bullet::get_y() { return y; }
void Bullet::flush(Map& map) {
	LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 0, 0, 0);
}
void Bullet::light(Map& map) {
	LogiLedSetLightingForKeyWithKeyName((LogiLed::KeyName)map.get(x, y), 100, 0, 0);
}
void Bullet::move_left(Map& map) {
	flush(map);
	if (y>0)y = y - 1;
	light(map);
}
Map::Map() {
	if (!LogiLedInit())
	{
		printf("Error initialization\n");
	}
	LogiLedSetLighting(0, 0, 0);
	int redVal = 0;
	int greenVal = 0;
	int blueVal = 100;
	LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::S, redVal, greenVal, blueVal);
	key_val[0][0] = 0x29;
	key_val[1][0] = 0x0F;
	key_val[2][0] = 0x3A;
	key_val[3][0] = 0x2A;
	for (int i = 0; i < ROW; i++) {
		for (int j = 1; j < COLUMN; j++) {
			if (i == 0)key_val[i][j] = 0x02 + j - 0x01;
			else if (i == 1)key_val[i][j] = 0x10 + j - 0x01;
			else if (i == 2)key_val[i][j] = 0x1E + j - 0x01;
			else if (i == 3)key_val[i][j] = 0x2C + j - 0x01;
		}
	}
}
int Map::get(int i, int j) { return key_val[i][j]; }
int  Map::win(int y) {
	if (y == COLUMN-diff)return 1;
	return 0;
}
int Map::collision(Bullet& b,int x,int y) {
	//printf("%d %d %d %d %d \n", b.get_x(0), b.get_x(1), x, b.get_y(), y);
	if (b.get_x() == x  && b.get_y() == y) {
		return 1;
	}
	return 0;
}