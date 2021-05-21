/*************************************
 * 게임 시작 entry point 메인함수
 * 게임 로직과 화면 제어를 수행한다.
***************************************/
#include <clocale>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <string>

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

//사용자의 키 입력 확인 함수
bool kbhit()
{
  int ch;
  bool ret;

  nodelay(stdscr, TRUE);

  ch = getch();
  if (ch == ERR)
  {
    ret = false; //키 입력이 없을 경우 false 반환
  }
  else
  {
    ret = true;  //키 입력이 있을 경우 true 반환
    ungetch(ch); // 마지막에 받은 문자를 버퍼에 다시 넣어서 다음 getch()가 받을 수 있도록 함
  }

  nodelay(stdscr, FALSE);
  return ret;
}

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
  //화면에 커서 보이지 않게
  curs_set(0);
  // 입력한 키 값을 화면에 보이지 않게
  noecho();
  keypad(stdscr, TRUE);
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
    //현재 스테이지의 맵 상태 객체 생성
    Object **mapStatus = manager.getMapStatus();
    //현 스테이지에 나타낼 snake 생성
    Snake snake = manager.getSnake();
    // 스테이지 시작
    bool isGameOver = false;
    while (!isGameOver)
    {
      //사용자가 입력한 방향을 나타낼 변수(기본 방향 (0, 0))
      Direction newdirection;
      //사용자의 입력을 받는 기능 구현 및 clock() 함수를 이용한 sleep 기능 구현======================
      /* sleep함수가 진행되는 동안은 완전히 lock 되어버려서 그 시간 동안 프로그램은 모든 동작을 잠정 중단,보류가 됨
                 사용자가 연속적으로 키를 입력해도 sleep함수가 진행되는 동안은 입력이 되지 않아서 반영되지 않음
                 이것을 해결하기 위해, clock() 함수를 이용한 새로운 sleep 기능 구현 */
      long int refTime = clock(); //프로그램이 시작한 시점부터 현재까지 경과한 시간
      long int currentTime = 0;
      while (1)
      {
        if (kbhit())
        {                   //키 입력이 확인되었을 때
          int ch = getch(); //키 입력 받기
          switch (ch)
          {
          case KEY_LEFT:
            newdirection.set_direction(-1, 0);
            break;
          case KEY_RIGHT:
            newdirection.set_direction(1, 0);
            break;
          case KEY_UP:
            newdirection.set_direction(0, -1);
            break;
          case KEY_DOWN:
            newdirection.set_direction(0, 1);
            break;
          }
          /*사용자가 입력한 방향을 snake head의 새로운 방향으로 갱신
                      이때 기존 snake head의 뱡향과 반대되는 뱡향이 입력 될 경우에는 게임 오버*/
          if (snake.change_head_direction(newdirection) != true)
          {
            isGameOver = true;
          }
        }
        /*프로그램이 시작한 시점부터 키 입력 확인 및 반영 기능 이후까지 경과한 시간에서
                    프로그램 시작 시점부터 입력 받기 전까지 경과한 시간의 차*/
        currentTime += clock() - refTime;
        if (currentTime > 50000000000)
        { //그 시간 차가 50000000000을 넘었을 때 데이터 갱신 및 렌더닝 기능으로 넘어가기
          currentTime = 0;
          break;
        }
        /* 값을 50000000000으로 설정한 것은 이 값보다 작게 설정하면 너무 짧은 시간 간격으로 렌더링이 이뤄져서
                     지렁이 이동속도가 넘무 빠르고 너무 큰 수를 하면 너무 delay가 심해서 적당한 값을 찾다가 50000000000으로 설정하였습니다!
                     값 수정하셔도 좋습니다!*/
      }
      //이하 데이터 갱신===============================================================
      //snake의 몸통 이동 데이터 갱신
      snake.moveTo();
      //이상 데이터 갱신===============================================================

      // 상태 점검===================================================================
      //벽 부딪침 확인
      /* 게임 매니저와 상호작용하여 현재 벽에 부딪힌 상태인지(머리가 벽과 겹쳐 있는지) 확인하는 메서드
        벽에 부딪힌 상태라면 게임오버를 반환함
        (snake.h에서 구현을 하였으나 'Invalid use of incomplete type'가 발생하였고, 오류를 해결할 방법을 찾지 못해
        일단 여기서 구현을 하였습니다ㅜㅜ)*/
      int **map = manager.getMapCodes();
      vector<Body> bodies(snake.getBodies());
      for(int i=0; i<BOARD_SIZE_Y; i++){
        for(int j=0; j<BOARD_SIZE_X; j++){
          if((map[i][j] == 1)&&(bodies[0].get_currentx()==j+1)&&(bodies[0].get_currenty()==i+1)){
            isGameOver = true;
          }
        }
      }
      //몸체 부딪침 확인
      if(snake.isBumpedToBody())
        isGameOver = true;
      //벽과 지렁이가 있는 곳이 아닌 지점에 아이템 랜덤생성
      manager.createGrowth();
      manager.createPoison();
      //아이템과 snake가 접촉하였는지 확인 및 아이템 사용 후, snake 길이가 3보다 짧아질 경우
      if(manager.useItem(snake) == false)
        isGameOver = true;
      //아이템 사용을 하지 않았고 생성된지 5초가 경과되었을 경우 아이템 삭제
      manager.removeGrowth(0);
      manager.removePoison(0);
      //다시 아이템 랜덤 생성
      manager.createGrowth();
      manager.createPoison();

      // 이하 렌더링 =================================================================
      // 게임 보드 렌더링
      for (int i = 0; i < BOARD_SIZE_Y; i++)
        for (int j = 0; j < BOARD_SIZE_X; j++)
          mvwprintw(gameWindow, i + 1, j + 1, mapStatus[i][j].getSymbol().c_str());

      // Snake 렌더링
      for (int i = 0; i < bodies.size(); i++)
      {
        Pos pos = bodies[i].getPos();
        // 머리
        if (i == 0)
        {
          mvwprintw(gameWindow, pos.y, pos.x, "@");
        }
        else
          mvwprintw(gameWindow, pos.y, pos.x, "O");
      }
      wrefresh(gameWindow);
      // 스코어 보드 렌더링
      mvwprintw(scoreWindow, 4, 3, "Play Time:");
      mvwprintw(scoreWindow, 4, 14, to_string(time(NULL) - startTime).c_str());
      mvwprintw(scoreWindow, 5, 3, "B:");
      mvwprintw(scoreWindow, 5, 6, to_string(snake.getLength()).c_str());
      mvwprintw(scoreWindow, 6, 3, "+:");
      mvwprintw(scoreWindow, 6, 6, to_string(snake.getGrowthCnt()).c_str());
      mvwprintw(scoreWindow, 7, 3, "-:");
      mvwprintw(scoreWindow, 7, 6, to_string(snake.getPoisonCnt()).c_str());
      mvwprintw(scoreWindow, 8, 3, "G: 0");
      wrefresh(scoreWindow);

      // 미션 보드 렌더링
      mvwprintw(missionWindow, 4, 3, "B: ");
      mvwprintw(missionWindow, 5, 3, "+: ");
      mvwprintw(missionWindow, 6, 3, "-: ");
      mvwprintw(missionWindow, 7, 3, "G: ");
      wrefresh(missionWindow);

      //메세지 보드 렌더링
      /* 사용자가 입력하는 방향키가 어떤 것인지 게임상에서 확인 할 수 있도록
                   메세치 창에 입력값이 보여지게 설정해보았습니다! 수정하셔도 상관없습니다!*/
      string s;
      s += snake.getlastdirection().getSymbol();
      mvwprintw(messageWindow, 2, 6, "Your Keypad : ");
      mvwprintw(messageWindow, 2, 20, s.c_str());
      wrefresh(messageWindow);
    }
    //게임 오버일 때 메세지 창에 게임오버 띄우기
    mvwprintw(messageWindow, 2, 6, "! Game Over !");
    wrefresh(messageWindow);
  }
  getch();
  delwin(gameWindow);
  delwin(scoreWindow);
  delwin(messageWindow);
  endwin();

  cout << time(NULL) - startTime;

  return 0;
}
