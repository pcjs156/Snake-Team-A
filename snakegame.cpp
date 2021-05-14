/*************************************
 * 게임 시작 entry point 메인함수       
 * 게임 로직과 화면 제어를 수행한다. 
**************************************/
#include <clocale>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>

#include "Snake.h"
#include "Manager.h"
using namespace std;

// 정적 전역 변수 설정
const int TICK = 0.5;       // 화면을 갱신할 주기
const int TOTAL_STAGES = 1; // 스테이지 개수

// 게임 보드의 실제 크기
const int BOARD_SIZE_Y = 25;
const int BOARD_SIZE_X = 55;

// 최상위 윈도우
const int COLOR_W = 0;
const int W_Y_SIZE = 29;
const int W_X_SIZE = 100;

// 게임 윈도우 크기
const int COLOR_G = 1;
const int GW_Y_SIZE = BOARD_SIZE_Y + 2; // 게임보드 크기는 25
const int GW_X_SIZE = BOARD_SIZE_X + 2; // 게임보드 크기는 55

// 점수 윈도우 크기
const int COLOR_S = 2;
const int SW_Y_SIZE = 11;
const int SW_X_SIZE = 39;

// 미션 윈도우 크기
const int COLOR_M = 3;
const int MW_Y_SIZE = 10;
const int MW_X_SIZE = 39;

// 메시지 윈도우 크기
const int COLOR_MSG = 4;
const int MSGW_Y_SIZE = 4;
const int MSGW_X_SIZE = 39;

int main()
{
    // 이하 유니코드 사용 설정 (손대지 말 것) =================================
    setlocale(LC_ALL, "");
    addch(ACS_LTEE);
    addch(ACS_RTEE);
    addch(ACS_HLINE);
    // 이상 유니코드 사용 설정 (손대지 말 것) =================================

    // 스크린 초기화
    initscr();
    // 문자가 입력되지 않도록 초기화
    noecho();

    // 파레트 설정
    start_color();
    init_pair(COLOR_G, COLOR_BLACK, COLOR_WHITE); // 게임 윈도우 팔레트
    init_pair(COLOR_S, COLOR_BLACK, COLOR_WHITE); // 점수 윈도우 팔레트

    // 이하 최상위 윈도우 초기 설정 =============================================
    // 윈도우 선언
    WINDOW *gameWindow;    // 게임 윈도우(Snake가 돌아다니는)
    WINDOW *scoreWindow;   // 점수 윈도우
    WINDOW *missionWindow; // 미션 윈도우
    WINDOW *messageWindow; // 메시지 윈도우

    // 최상위 윈도우 크기 조정
    resize_term(W_Y_SIZE, W_X_SIZE);
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    // 최상위 윈도우 렌더링
    refresh();
    // 이하 최상위 윈도우 초기 설정 =============================================

    // 이하 게임 윈도우 초기 설정 ===============================================
    gameWindow = newwin(GW_Y_SIZE, GW_X_SIZE, 1, 2);
    wbkgd(gameWindow, COLOR_PAIR(COLOR_G));
    wattron(gameWindow, COLOR_PAIR(COLOR_G));
    wborder(gameWindow, '|', '|', '-', '-', '@', '@', '@', '@');
    wrefresh(gameWindow);
    // 이상 게임 윈도우 초기 설정 ===============================================

    // 이하 점수 윈도우 초기 설정 ===============================================
    scoreWindow = newwin(SW_Y_SIZE, SW_X_SIZE, 1, GW_X_SIZE + 3);
    wbkgd(scoreWindow, COLOR_PAIR(COLOR_S));
    wattron(scoreWindow, COLOR_PAIR(COLOR_S));
    wborder(scoreWindow, '|', '|', '=', '-', '@', '@', '@', '@');
    mvwprintw(scoreWindow, 1, 2, "SCORE BOARD");

    // 구분선 그리기
    string SW_hr = "";
    for (int i = 0; i < SW_X_SIZE - 2; i++)
    {
        SW_hr += "=";
    }
    mvwprintw(scoreWindow, 2, 1, SW_hr.c_str());

    wrefresh(scoreWindow);
    // 이상 점수 윈도우 초기 설정 ===============================================

    // 이하 점수 윈도우 초기 설정 ===============================================
    missionWindow = newwin(MW_Y_SIZE, MW_X_SIZE, SW_Y_SIZE + 2, GW_X_SIZE + 3);
    wbkgd(missionWindow, COLOR_PAIR(COLOR_S));
    wattron(missionWindow, COLOR_PAIR(COLOR_S));
    wborder(missionWindow, '|', '|', '=', '-', '@', '@', '@', '@');
    mvwprintw(missionWindow, 1, 1, "MISSION BOARD");

    // 구분선 그리기
    string MW_hr = "";
    for (int i = 0; i < SW_X_SIZE - 2; i++)
    {
        MW_hr += "=";
    }
    mvwprintw(missionWindow, 2, 1, MW_hr.c_str());

    wrefresh(missionWindow);
    // 이상 점수 윈도우 초기 설정 ===============================================

    // 이하 메시지 윈도우 초기 설정 =============================================
    messageWindow = newwin(MSGW_Y_SIZE, MSGW_X_SIZE, SW_Y_SIZE + MW_Y_SIZE + 3, GW_X_SIZE + 3);
    wbkgd(messageWindow, COLOR_PAIR(COLOR_S));
    wattron(messageWindow, COLOR_PAIR(COLOR_S));
    wborder(messageWindow, '|', '|', '=', '-', '@', '@', '@', '@');
    mvwprintw(messageWindow, 1, 1, "MESSAGE)");
    mvwprintw(messageWindow, 2, 1, ">>> ");
    wrefresh(messageWindow);
    // 이상 메시지 윈도우 초기 설정 =============================================

    int currentStage = 0;          // 스테이지 번호
    time_t startTime = time(NULL); // 게임 시작 시각(초)
    // 게임 루프 시작
    while (currentStage++ < TOTAL_STAGES)
    {
        // 스테이지 매니저 객체 생성
        Manager manager(currentStage);
        Object **mapStatus = manager.getMapStatus();

        // 스테이지 시작
        bool isGameOver = false;
        while (!isGameOver)
        {
            // 이하 사용자 입력 ============================================================
            // !!! NOT IMPLEMENTED YET !!!

            // 이하 데이터 갱신/판별 =======================================================
            // !!! NOT IMPLEMENTED YET !!!

            // 이하 렌더링 =================================================================
            // 게임 보드 렌더링
            for (int i = 0; i < BOARD_SIZE_Y; i++)
                for (int j = 0; j < BOARD_SIZE_X; j++)
                    mvwprintw(gameWindow, i + 1, j + 1, mapStatus[i][j].getSymbol().c_str());

            // Snake 렌더링
            Snake snake = manager.getSnake();
            vector<Body> bodies = snake.getBodies();
            for (int i = 0; i < bodies.size(); i++)
            {
                Pos pos = bodies[i].getPos();
                // 머리
                if (i == 0)
                    mvwprintw(gameWindow, pos.y + 21, pos.x + 49, "@");
                else
                    mvwprintw(gameWindow, pos.y + 21, pos.x + 49, "O");
            }
            wrefresh(gameWindow);

            // 스코어 보드 렌더링
            mvwprintw(scoreWindow, 4, 3, "Play Time:");
            mvwprintw(scoreWindow, 4, 14, to_string(time(NULL) - startTime).c_str());
            mvwprintw(scoreWindow, 5, 3, "B: 0");
            mvwprintw(scoreWindow, 6, 3, "+: 0");
            mvwprintw(scoreWindow, 7, 3, "-: 0");
            mvwprintw(scoreWindow, 8, 3, "G: 0");
            wrefresh(scoreWindow);

            // 미션 보드 렌더링
            mvwprintw(missionWindow, 4, 3, "B: ");
            mvwprintw(missionWindow, 5, 3, "+: ");
            mvwprintw(missionWindow, 6, 3, "-: ");
            mvwprintw(missionWindow, 7, 3, "G: ");
            wrefresh(missionWindow);

            sleep(0.5);
        }
    }

    getch();
    delwin(gameWindow);
    delwin(scoreWindow);
    delwin(messageWindow);
    endwin();

    cout << time(NULL) - startTime;

    return 0;
}