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

using namespace std;


void printb(const int row, const int column, const int color_pair) {
    move(row, column * 2);
    attron(COLOR_PAIR(color_pair));
    addch(ACS_BLOCK);
    addch(ACS_BLOCK);
}

int main() {
    initscr();

    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_BLUE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_WHITE);
    init_pair(4, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(5, COLOR_GREEN, COLOR_GREEN);
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

    // 스테이지 클리어하면 stage 변수가 1씩 증가
    // victory가 true인 상태로 반복문 탈출하면 게임 최종 승리, 아니면 실패
    for (int stage = 1; ; ++stage) {
        // 더 불러올 맵이 없으면 승리 처리 및 반복문 탈출
        std::ifstream fin("map" + to_string(stage) + ".txt");
        if (!fin) {
            victory = true;
            break;
        }

        // 최종적으로 화면에 출력할 내용을 배열로 구현
        int screen[MAP_SIZE][MAP_SIZE];

        // 맵 파일에서 맵 불러오기
        Map map;
        for (int row = 0; row < MAP_SIZE; ++row)
            for (int column = 0; column < MAP_SIZE; ++column)
                fin >> map.map[row][column];

        // 길이 3칸, 왼쪽, 맵 중앙으로 지렁이 초기화
        Snake snake(3, LEFT, MAP_SIZE / 2, MAP_SIZE / 2);

        Item items[3];
        /*준서: items 초기화하기
        * Item 객체 배열 items를 위치가 랜덤하게 초기화하세요.
        * Item의 종류(GrowthItem, PoisonItem)는 무작위입니다.
        */

        Gate gates[2];

        gateActive = false;
        victory = false;
        tick = 0;

        // victory가 true인 상태로 반복문 탈출하면 스테이지 성공, 아니면 실패
        while (true) {
            // screen에 map 올리기
            for (int row = 0; row < MAP_SIZE; ++row)
                for (int column = 0; column < MAP_SIZE; ++column)
                    screen[row][column] = map.map[row][column];

            // screen에 gate 올리기
            // 정은: 아래 코드를 완성해주세요.
            for (Gate gate : gates) {
                // gate의 위치에 해당하는 위치를 screen에서 GATE(7)로 설정
            }

            // screen에 item 올리기
            // 준서: 아래 코드의 조건문을 완성해주세요.
            for (int index = 0; index < itemNum; ++index) {
                screen[items[index].x][items[index].y]
                    = (/*items[index]가 GrowthItem의 객체일 때 true*/) ? GROWTH_ITEM : POISON_ITEM;
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

            // 준서: 1틱(0.25초)동안 입력 받으며 대기하기
            // 0.25초가 지나면 tick 변수를 1 증가시킨다.
            // ClassName time = now();
            //while (/* now() - time < 250ms */)
                keyboardInput = getch();    // 이 방법은 문제가 있다!
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
                case KEY_RIGHT: snake.setDirectionTo(RIGHT);    break;
                case KEY_LEFT:  snake.setDirectionTo(LEFT);  break;
                }
                snake.move();

                // 자기 몸체에 충돌하면 실패
                for (int index = 1; index < snake.getLength(); ++index)
                    if (snake.getPositionOf(0) == snake.getPositionOf(index))
                       break;

                // 벽에 부딪히면 실패
                if (map.map[snake.getPositionOf(0)[0]][snake.getPositionOf(0)[1]] == WALL ||
                    map.map[snake.getPositionOf(0)[0]][snake.getPositionOf(0)[1]] == IMMUNE_WALL) {
                    break;
                }

                // 아이템 획득
                for (int index = 0; index < itemNum; ++index) {
                    // items[index]와 위치가 겹치면
                    if (snake.getPositionOf(0)[0] == items[index].x &&
                        snake.getPositionOf(0)[1] == items[index].y) {
                       if (/*items[index]가 GrowthItem의 객체일 경우 true*/)
                            snake.increaseLength();
                        else
                            snake.decreaseLength();

                        /*준서: 아이템 리스폰
                        * 획득한 해당 아이템, 즉 items[index]를 재생성하세요.
                        * GrowthItem과 PoisonItem 중 하나로 무작위생성합니다.
                        * 위치도 다른 요소들과 겹치지 않도록 무작위로 설정합니다.
                        */

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

                // if (지렁이가 gate 들어가면) {}
                /*정은: snake가 gate에 들어가는 것 구현
                * 지렁이 head를 snake.setPositionTo(x, y)로 이동시키면
                * 나머지 몸체는 알아서 따라옵니다.
                * 지렁이 관련해서 궁금한 점이 있으면 연락주세요.
                */
            }

            if (tick % itemTick == 0) {
                /*준서: 아이템 재출현
                * items에 있는 itemNum개의 아이템을 재출현시켜주세요.
                * Item의 종류(GrowthItem, PoisonItem)는 무작위입니다.
                */
            }

            if (!gateActive && snake.getLength() > 10) {
                /*정은: 게이트 출현
                * 게이트를 출현시켜주세요.
                * gateActive를 true로 설정해주세요.
                */
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
