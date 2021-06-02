#ifndef __MANAGER_H__
#define __MANAGER_H__

/* !!! Gate, Item쪽 구현은 Snake쪽 틀이 좀 잡힌 다음 구체화하는게 좋을 것 같음. 변경 가능성 높음. !!!*/
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Snake.h"
#include "Objects.h"

// 맵을 구성하는 요소들의 코드
const int EMPTY_CODE = 0;          // 빈 공간
const int WALL_CODE = 1;           // 벽
const int IMMUNE_WALL_CODE = 2;    // 게이트가 생기지 않는 벽
const int INIT_snakeHead_CODE = 3; // Snake의 머리 부분
const int GROWTH_CODE = 4;
const int POISON_CODE = 5;
const int GATE_CODE = 6;

// 맵 파일을 여는데 실패했을 때 발생하는 예외
class MapFIleOpenFailedException
{
};

// 맵 파일에 유효하지 않은 심볼이 있을 때 발생하는 예외
class InvalidSymbolException
{
};

/* 맵과 Snake의 정보를 저장할 매니저 클래스 */
class Manager
{
private:
  // 스테이지 번호
  int STAGE_NUM;
  // 맵의 X-Y 방향 크기
  /* !!! snakegame.cpp의 BOARD_SIZE_Y와 BOARD_SIZE_X와 같아야 함 !!! */
  int SIZE_X = 55;
  int SIZE_Y = 25;
  // 각 요소(Wall, Snake Head, ...) 등의 초기 상태를 코드로 저장할 2차원 배열
  int **mapCodes;
  // 맵의 현재 상태(아이템 객체 등)를 Object 객체로 저장할 2차원 배열
  Object **mapStatus;

  // Snake를 가리키고 있음 -> 매 스테이지가 끝날 때마다 새로 갱신해주어야 함
  // Constructor에서 다시 instance를 만들어 제대로 초기화해줌
  Snake snake;

  // 현재 생성된 아이템의 개수를 기록하는 변수
  bool isGrowthCreated = false;
  bool isPoisonCreated = false;
  //현재 생성된 아이템의 좌표를 기록하는 변수
  Pos growthPos = Pos(0, 0);
  Pos poisonPos = Pos(0, 0);
  //생성된 아이템의 시각을 기록하는 변수
  time_t growthCreatedTime;
  time_t poisonCreatedTime;
  // 아이템의 유지 시간(초)
  const int GROWTH_DURATION = 60;
  const int POISON_DURATION = 5;

  // 현재 게이트가 생성되어 있는지 여부를 기록하는 변수
  bool isGateCreated = false;
  // 들어가는 게이트와 나오는 게이트의 좌표
  Pos gateEntrancePos = Pos(0, 0);
  Pos gateExitPos = Pos(0, 0);
  // 게이트 생성이 시작되는 snake의 길이
  const int GATE_GENERATE_LEN = 3;
  // 게이트 유지 시간(초)
  const int GATE_DURATION_SEC = 60;
  // 게이트 생성 주기
  const int GATE_COOLTIME = 2;
  // 게이트가 snake의 위치를 갱신할(snake가 앞으로 위치를 갱신할 수 있는) 횟수
  int gateActivationLeft = 0;
  // 마지막으로 게이트가 생성된 시각
  time_t gateGenerated = 0;

public:
  // Constructor/Destructor Overriding begins =======================
  // 1부터 시작하는 스테이지 번호를 입력받아 맵 파일을 읽어옴
  Manager(int stageNumber);
  ~Manager();

  // Constructor/Destructor Overriding finished =====================
  // Getter begins ==================================================
  // 맵 코드 정보를 반환
  int **getMapCodes() { return mapCodes; }
  // 맵 상태 정보를 반환
  Object **getMapStatus() { return mapStatus; }
  // Snake 인스턴스를 반환
  Snake getSnake() const { return snake; }

  // Getter finished ================================================
  // items begins ===================================================
  /* Growth를 생성하는 메서드
       Growth가 생성되어 있다면 Growth를 생성하지 않고 false를 반환한다.
       Growth가 생성되어 있지 않다면 Growth를 생성하고 true를 반환한다. */
  bool createGrowth();

  /* Growth를 삭제하는 메서드
    Growth가 생성되어 있다면 Growth 자리를 비워두고 true를 반환한다.
    Growth가 생성되어 있지 않다면 false를 반환한다. */
  bool removeGrowth(int check);

  /* Poison을 생성하는 메서드
       Poison이 생성되어 있다면 Poison을 생성하지 않고 false를 반환한다.
       Poison이 생성되어 있지 않다면 Poison를 생성하고 true를 반환한다. */
  bool createPoison();

  /* Poison을 삭제하는 메서드
    Poison이 생성되어 있다면 Poison 자리를 비워두고 true를 반환한다.
    Poison이 생성되어 있지 않다면 false를 반환한다. */
  bool removePoison(int check);

  // Snake Head가 아이템의 좌표와 일치하는지 확인하고 사용하는 메서드
  bool useItem(Snake &s);

  // items finished =================================================
  // gate begins ====================================================
  /* 게이트가 활성화되어 있는지(사용중인지) 판별하는 메서드 */
  bool isGateActivated();

  /* 게이트를 생성하는 메서드
    게이트가 생성되어 있거나 게이트를 통과하고 있는 경우 false를 반환하고,
    게이트가 생성되어 있지 않은 경우 게이트를 생성한 후 true를 반환함*/
  bool createGate(Snake &s);

  // 게이트를 통과할 때마다 남은 게이트 활성화 횟수를 줄여줌
  void updateGateActivationLeft();

  /* 게이트를 삭제하는 메서드
       게이트가 활성화 되어 있거나 유지 시간이 끝난 경우 게이트를 닫음 */
  void removeGate();

  /* snake.useGate를 호출하는 메서드
     Precondition: turnOnGate가 호출되는 시점에 head의 좌표가 반드시 진입 좌표여야 한다. */
  void turnOnGate(Snake &s);

  /* Snake Head가 게이트 좌표와 일치하는지 확인하는 메서드 */
  bool isHeadAtGate(Snake &s);

  /* Snake의 마지막 Body가 진입 게이트와 겹치는지 확인하는 메서드*/
  bool isTailAtEntranceGate(Snake &s);

  /* 출구 게이트의 좌표가 가장자리인지 확인하는 메서드
     가장자리인 경우 상 우 하 좌 방향을 판별하여 각각 'U', 'R', 'D', 'L' 코드를 반환하고
     가장자리가 아닌 경우 -1을 반환한다. */
  char isExitGateAtEdge();

  // gate finished ==================================================
}; // Manager class finished;

#endif
