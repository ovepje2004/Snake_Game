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
			for (Gate gate : gates) {
				// gate의 위치에 해당하는 위치를 screen에서 GATE(7)로 설정
				screen[gate.x][gate.y] = 7;

			}

			// screen에 snake 올리기
			for (int index = 0; index < snake.getLength(); ++index) {
				screen[snake.getPositionOf(index)[0]][snake.getPositionOf(index)[1]]
					= (index == 0) ? SNAKE_HEAD : SNAKE_BODY;
			}

			// screen을 화면에 출력
			for (int row = 0; row < MAP_SIZE; ++row)
				for (int column = 0; column < MAP_SIZE; ++column)
					printb(START_X + row, START_Y + column, screen[row][column] + 1);
			refresh();

			keyboardInput = getch();
			++tick;

			// 지렁이가 움직일 때
			if (tick % snakeTick == 0) {
				// 반대방향 입력하면 실패
				if ((keyboardInput == KEY_UP && snake.getDirection() == DOWN) ||
					(keyboardInput == KEY_DOWN && snake.getDirection() == UP) ||
					(keyboardInput == KEY_RIGHT && snake.getDirection() == LEFT) ||
					(keyboardInput == KEY_LEFT && snake.getDirection() == RIGHT))
					break;

				// 이동
				switch (keyboardInput) {
				case KEY_UP:    snake.setDirectionTo(UP);  break;
				case KEY_DOWN:  snake.setDirectionTo(DOWN); break;
				case KEY_RIGHT: snake.setDirectionTo(RIGHT); break;
				case KEY_LEFT:  snake.setDirectionTo(LEFT);  break;
				}
				snake.move();

				// 자기 몸체에 충돌하면 실패
				bool crashed = false;
				for (int index = 1; index < snake.getLength(); ++index)
					if (snake.getPositionOf(0)[0] == snake.getPositionOf(index)[0] &&
						snake.getPositionOf(0)[1] == snake.getPositionOf(index)[1]) {
						crashed = true;
						break;
					}
				if (crashed) break;

				// 벽에 부딪히면 실패
				if ((map.map[snake.getPositionOf(0)[0]][snake.getPositionOf(0)[1]] == WALL ||
					map.map[snake.getPositionOf(0)[0]][snake.getPositionOf(0)[1]] == IMMUNE_WALL) &&
					!((snake.getPositionOf(0)[0] == gates[0].x && snake.getPositionOf(0)[1] == gates[0].y) ||
					(snake.getPositionOf(0)[0] == gates[1].x && snake.getPositionOf(0)[1] == gates[1].y))) {
					break;
				}

				// 아이템 획득
				for (int index = 0; index < itemNum; ++index) {
					// items[index]와 위치가 겹치면
					if (snake.getPositionOf(0)[0] == items[index].x &&
						snake.getPositionOf(0)[1] == items[index].y) {
						if (items[index].Item_type == 1) //growthItem일 경우 길이 증가
							snake.increaseLength();
						else //poisonItem일 경우 길이 감소
							snake.decreaseLength();


						Item_respawn(items[index], map);
						//아이템이 먹힌 경우 리스폰 뿐만 아니라, 아이템을 화면에서 잠시 숨겨야 함.
						items[index].active = false;

						break;
					}
				}

				// 목표 길이 도달하면 스테이지 클리어
				if (snake.getLength() >= goal) {
					victory = true;
					break;
				}

				// 최소 길이보다 짧아지면 실패
				if (snake.getLength() < MIN_LENGTH)
					break;

				for (int i = 0; i < 2; ++i) {
					if (snake.getPositionOf(0)[0] == gates[i].x && snake.getPositionOf(0)[1] == gates[i].y) {
						int oppositeGateIndex = (i == 0) ? 1 : 0;
						int wall = gates[oppositeGateIndex].checkWall();
						int XgoTo = 0; int YgoTo = 0;
						switch (wall) {
						case 1:
							snake.setDirectionTo(DOWN);
							XgoTo = gates[oppositeGateIndex].x + 1;
							YgoTo = gates[oppositeGateIndex].y;
							break;
						case RIGHT:
							snake.setDirectionTo(UP);
							XgoTo = gates[oppositeGateIndex].x - 1;
							YgoTo = gates[oppositeGateIndex].y;
							break;
						case 3:
							snake.setDirectionTo(RIGHT);
							XgoTo = gates[oppositeGateIndex].x;
							YgoTo = gates[oppositeGateIndex].y + 1;
							break;
						case 4:
							snake.setDirectionTo(LEFT);
							XgoTo = gates[oppositeGateIndex].x;
							YgoTo = gates[oppositeGateIndex].y - 1;
							break;
						default:
							break;
						}
						snake.setPositionTo(XgoTo, YgoTo);
						break;
					}
				}
			}

			if (tick % itemTick == 0) {
				//아이템 재출현
				for (int i = 0; i < itemNum; i++)  Item_respawn(items[i], map);
			}
		}

		// victory가 false인 상태로 반복문 탈출했으면
		// 실패 처리하고 게임 종료
		if (!victory)
			break;

		// 다음 스테이지로 넘어가기까지 잠깐 멈춤
		Sleep(5000);
	}

	endwin();
	if (victory)
		cout << "contratulations!" << endl;
	else
		cout << "failed!" << endl;

	return 0;
}
