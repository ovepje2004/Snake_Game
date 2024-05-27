#include "Constants.h"
#include "Snake.cpp"
#include "Map.cpp"
#include "Item.cpp"
#include "Gate.cpp"

#include <windows.h>
#include <string>
#include <curses.h>
#include <fstream>
#include <thread>
#include <ctime>

using namespace std;

void printb(const int row, const int column, const int color_pair) {
	move(row, column * 2);
	attron(COLOR_PAIR(color_pair));
	addch(ACS_BLOCK);
	addch(ACS_BLOCK);

}

// 맵 파일에서 맵 불러오기
Map map;
Gate getRandomGate1() {
	int x, y;
	do {
		x = rand() % MAP_SIZE;
		y = rand() % MAP_SIZE;
	} while (map.map[x][y] != 1);

	return Gate(x, y);
}

Gate getRandomGate2(int avoidX, int avoidY) {
	int x, y;
	do {
		x = rand() % MAP_SIZE;
		y = rand() % MAP_SIZE;
	} while ((x == avoidX && y == avoidY) || map.map[x][y] != 1);

	return Gate(x, y);
}

void Item_respawn(Item& item, const Map map) {
	// 위치가 겹치지 않게 아이템 생성
	int new_x, new_y;
	do {
		new_x = std::rand() % MAP_SIZE;
		new_y = std::rand() % MAP_SIZE;
	} while (map.map[new_x][new_y] != 0);

	item.x = new_x;
	item.y = new_y;
	item.Item_type = std::rand() % 2;
	item.active = true;
}

int main() {
	initscr();

	start_color();
	init_pair(1, COLOR_RED, COLOR_RED);
	init_pair(2, COLOR_BLUE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, COLOR_WHITE);
	init_pair(4, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(5, COLOR_GREEN, COLOR_GREEN);
	init_pair(6, COLOR_BLACK, COLOR_BLACK);
	init_pair(7, COLOR_CYAN, COLOR_CYAN);
	init_pair(8, COLOR_MAGENTA, COLOR_MAGENTA);
	// 필요하면 컬러페어 수정하거나 추가해주세요!

	keypad(stdscr, true);
	noecho();

	int itemNum;
	int itemTick;
	int snakeTick;
	int goal;

	bool gateActive;
	bool victory;
	int tick;
	int keyboardInput;

	// 게임 난이도 변수
	// 추후 플레이어가 메인화면에서 바꿀 수 있도록 할 예정
	goal = 15;
	snakeTick = 1;
	itemTick = 40;
	itemNum = 3;

	halfdelay(3);
	// 스테이지 클리어하면 stage 변수가 1씩 증가
	// victory가 true인 상태로 반복문 탈출하면 게임 최종 승리, 아니면 실패
	for (int stage = 5; ; ++stage) {
		// 더 불러올 맵이 없으면 승리 처리 및 반복문 탈출
		std::ifstream fin("map" + to_string(stage) + ".txt");
		if (!fin) {
			victory = true;
			break;
		}

		// 최종적으로 화면에 출력할 내용을 배열로 구현
		int screen[MAP_SIZE][MAP_SIZE];

		for (int row = 0; row < MAP_SIZE; ++row)
			for (int column = 0; column < MAP_SIZE; ++column)
				fin >> map.map[row][column];

		// 길이 3칸, 왼쪽, 맵 중앙으로 지렁이 초기화
		Snake snake(3, LEFT, MAP_SIZE / 2, MAP_SIZE / 2);

		//items 객체 생성 및 초기화 
		Item items[3];
		for (int i = 0; i < itemNum; i++) {
			Item_respawn(items[i], map);
		}

		Gate gates[2];

		srand(time(nullptr));

		Gate randomGate1 = getRandomGate1();
		int x1 = randomGate1.getX();
		int y1 = randomGate1.getY();
		gates[0] = Gate(x1, y1);

		Gate randomGate2 = getRandomGate2(x1, y1);
		int x2 = randomGate2.getX();
		int y2 = randomGate2.getY();
		gates[1] = Gate(x2, y2);

		gateActive = false;
		victory = false;
		tick = 0;

		// victory가 true인 상태로 반복문 탈출하면 스테이지 성공, 아니면 실패
		while (true) {
			// screen에 map 올리기
			for (int row = 0; row < MAP_SIZE; ++row)
				for (int column = 0; column < MAP_SIZE; ++column)
					screen[row][column] = map.map[row][column];

			// screen에 item 올리기
			for (int index = 0; index < itemNum; ++index) {
				screen[items[index].x][items[index].y]
					= (items[index].Item_type == 1) ? GROWTH_ITEM : POISON_ITEM;
			}

			// screen에 gate 올리기
			// 정은: 아래 코드를 완성해주세요.
