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
const int EMPTY_CODE = 0;           // 빈 공간
const int WALL_CODE = 1;            // 벽
const int IMMUNE_WALL_CODE = 2;     // 게이트가 생기지 않는 벽
const int INIT_SNAKE_HEAD_CODE = 3; // Snake의 머리 부분
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
  int growthCnt = 0;
  int poisonCnt = 0;
  //현재 생성된 아이템의 좌표를 기록하는 변수
  int growth_item_x = 0;
  int growth_item_y = 0;
  int poison_item_x = 0;
  int poison_item_y = 0;
  //생성된 아이템의 시각을 기록하는 변수
  time_t init_growth;
  time_t init_poison;
  // 아이템의 유지 시간(초)
  const int GROWTH_DURATION = 60;
  const int POISON_DURATION = 5;

  // 현재 게이트가 생성되어 있는지 여부를 기록하는 변수
  bool isGateCreated = false;
  // 들어가는 게이트와 나오는 게이트의 좌표
  // 안정성을 위해 아이템이 생성되어 있지 않을 때에는 -1로 할당해 놓을 것!
  int gateEntranceX, gateEntranceY;
  int gateExitX, gateExitY;
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
  // filename을 입력받아 맵을 초기화함
  Manager(int stageNumber) : STAGE_NUM(stageNumber)
  {
    // 맵 정보를 담을 2차원 배열을 동적 할당함
    mapCodes = new int *[SIZE_Y];
    for (int i = 0; i < SIZE_Y; i++)
      mapCodes[i] = new int[SIZE_X];
    // 맵 상태 정보
    mapStatus = new Object *[SIZE_Y];
    for (int i = 0; i < SIZE_Y; i++)
      mapStatus[i] = new Object[SIZE_X];

    // 맵 파일을 여는데 실패하면 예외 발생(처리 안함)
    ifstream mapFile("./maps/map" + to_string(STAGE_NUM) + ".txt");
    if (!mapFile.is_open())
      throw MapFIleOpenFailedException();
    // 맵 파일을 열었으면 map에 정수로 데이터 유형 저장
    else
    {
      string buffer;
      for (int i = 0; i < SIZE_Y; i++)
      {
        getline(mapFile, buffer);
        // 맵 코드를 저장하고, 객체를 생성해 저장함
        for (int j = 0; j < SIZE_X; j++)
        {
          mapCodes[i][j] = buffer.at(j) - '0';
          switch (mapCodes[i][j])
          {
          // 빈 공간
          // Snake Head
          case 0:
          case 3:
            mapStatus[i][j] = Empty();
            break;
          // 벽
          case 1:
            mapStatus[i][j] = Wall();
            break;
          // 게이트가 생성되지 않는 벽
          case 2:
            mapStatus[i][j] = ImmuneWall();
            break;
          // 유효하지 않은 심볼
          default:
            cout << mapCodes[i][j] << endl;
            throw InvalidSymbolException();
            break;
          }
        }
      }
    }
    // 맵 파일 속 초기 Snake 위치/인스턴스 초기화
    int initSnakeX, initSnakeY;
    for (int i = 0; i < SIZE_Y; i++)
    {
      for (int j = 0; j < SIZE_X; j++)
      {
        if (mapCodes[i][j] == INIT_SNAKE_HEAD_CODE)
        {
          initSnakeY = i + 1;
          initSnakeX = j + 1;
          mapCodes[i][j] = 0; // 초기화에만 Snake Head 정보가 필요하므로 Empty로 놓음
        }
      }
    }
    //스테이지 초기 Snake 몸통의 좌표값을 설정
    Snake temp(initSnakeX, initSnakeY);
    snake = temp;
  }
  ~Manager()
  {
    // 맵 정보를 담은 동적 배열 해제
    for (int i = 0; i < SIZE_Y; i++)
      delete[] mapCodes[i];
    delete[] mapCodes;

    for (int i = 0; i < SIZE_Y; i++)
      delete[] mapStatus[i];
    delete[] mapStatus;
  }

  /* 맵 코드 정보를 외부에 반환하는 메서드 */
  int **getMapCodes()
  {
    return mapCodes;
  }
  Object **getMapStatus()
  {
    return mapStatus;
  }
  /* Snake 인스턴스를 반환 */
  Snake getSnake() const { return snake; }

  /* Growth를 생성하는 메서드
       Growth가 생성되어 있다면 Growth를 생성하지 않고 false를 반환한다.
       Growth가 생성되어 있지 않다면 Growth를 생성하고 true를 반환한다. */
  bool createGrowth()
  {
    if (growthCnt == 1)
    {
      return false;
    }
    else
    {
      Growth growth_item;
      init_growth = growth_item.time_check();
      srand((unsigned int)time(NULL));
      while (mapCodes[growth_item_y][growth_item_x] != 0)
      {
        growth_item_y = rand() % SIZE_Y;
        growth_item_x = rand() % SIZE_X;
      }
      mapStatus[growth_item_y][growth_item_x] = growth_item;
      mapCodes[growth_item_y][growth_item_x] = GROWTH_CODE;
      growthCnt = 1;
    }
  }

  /* Growth를 삭제하는 메서드
    Growth가 생성되어 있다면 Growth 자리를 nullptr로 두고 좌표를 -1로 변경한 뒤 true를 반환한다.
    Growth가 생성되어 있지 않다면 false를 반환한다. */
  bool removeGrowth(int check)
  {
    time_t end = time(NULL);
    if ((check == 0) && (growthCnt == 1) && (end - init_growth > GROWTH_DURATION))
    {
      mapStatus[growth_item_y][growth_item_x] = Empty();
      mapCodes[growth_item_y][growth_item_x] = 0;
      growthCnt = 0;
      growth_item_y = 0;
      growth_item_x = 0;
      return true;
    }
    else if (check == 1)
    {
      mapStatus[growth_item_y][growth_item_x] = Empty();
      mapCodes[growth_item_y][growth_item_x] = 0;
      growthCnt = 0;
      growth_item_y = 0;
      growth_item_x = 0;
      return true;
    }
    else
      return false;
  }

  /* Poison을 생성하는 메서드
       Poison이 생성되어 있다면 Poison을 생성하지 않고 false를 반환한다.
       Poison이 생성되어 있지 않다면 Poison를 생성하고 true를 반환한다. */
  bool createPoison()
  {
    if (poisonCnt == 1)
    {
      return false;
    }
    else
    {
      Poison poison_item;
      init_poison = poison_item.time_check();
      srand((unsigned int)time(NULL));
      while (mapCodes[poison_item_y][poison_item_x] != 0)
      {
        poison_item_y = rand() % SIZE_Y;
        poison_item_x = rand() % SIZE_X;
      }
      mapStatus[poison_item_y][poison_item_x] = poison_item;
      mapCodes[poison_item_y][poison_item_x] = POISON_CODE;
      poisonCnt = 1;
    }
  }

  /* Poison을 삭제하는 메서드
    Poison이 생성되어 있다면 Poison 자리를 nullptr로 두고 좌표를 -1로 변경한 뒤 true를 반환한다.
    Poison이 생성되어 있지 않다면 false를 반환한다. */
  bool removePoison(int check)
  {
    time_t end = time(NULL);
    if ((check == 0) && (poisonCnt == 1) && (end - init_poison > POISON_DURATION))
    {
      mapStatus[poison_item_y][poison_item_x] = Empty();
      mapCodes[poison_item_y][poison_item_x] = 0;
      poisonCnt = 0;
      poison_item_y = 0;
      poison_item_x = 0;
      return true;
    }
    else if (check == 1)
    {
      mapStatus[poison_item_y][poison_item_x] = Empty();
      mapCodes[poison_item_y][poison_item_x] = 0;
      poisonCnt = 0;
      poison_item_y = 0;
      poison_item_x = 0;
      return true;
    }
    else
      return false;
  }

  /* 게이트가 활성화되어 있는지(사용중인지) 판별하는 메서드 */
  bool isGateActivated()
  {
    return gateActivationLeft > 0;
  }

  /* 게이트를 생성하는 메서드
    게이트가 생성되어 있거나 게이트를 통과하고 있는 경우 false를 반환하고,
    게이트가 생성되어 있지 않은 경우 게이트를 생성한 후 true를 반환함*/
  bool createGate(Snake &s)
  {
    // 이미 게이트가 만들어져 있거나, 기준 길이보다 짧거나, 게이트가 쿨타임이 돌지 않은 경우 게이트를 생성하지 않음
    if (isGateCreated || (s.getLength() < GATE_GENERATE_LEN) || (time(NULL) - gateGenerated < GATE_COOLTIME))
    {
      return false;
    }
    else
    {
      // 게이트 생성 시각 초기화
      gateGenerated = time(NULL);
      // 게이트가 생성되었음을 표시
      isGateCreated = true;

      // 벽의 위치 중 랜덤하게 게이트의 위치를 고름
      // 일단 들어가는 게이트와 나오는 게이트의 위치를 고정하되,
      // snake head가 먼저 gate에 접촉하는 쪽을 entrance로 나중에 수정해 주어야 함
      srand(time(NULL));
      // 들어가는 게이트
      while (true)
      {
        gateEntranceX = rand() % SIZE_X;
        gateEntranceY = rand() % SIZE_Y;
        if (mapCodes[gateEntranceY][gateEntranceX] == WALL_CODE)
          break;
      }
      // 나오는 게이트
      while (true)
      {
        gateExitX = rand() % SIZE_X;
        gateExitY = rand() % SIZE_Y;
        if ((mapCodes[gateExitY][gateExitX] == WALL_CODE) &&
            (gateExitX != gateEntranceX && gateExitY != gateEntranceY))
          break;
      }

      mapStatus[gateEntranceY][gateEntranceX] = Gate();
      mapStatus[gateExitY][gateExitX] = Gate();
      mapCodes[gateEntranceY][gateEntranceX] = GATE_CODE;
      mapCodes[gateExitY][gateExitX] = GATE_CODE;

      return true;
    }
  }

  // 게이트를 통과할 때마다 남은 게이트 활성화 횟수를 줄여줌
  void updateGateActivationLeft()
  {
    gateActivationLeft -= 1;
  }

  /* 게이트를 삭제하는 메서드
       게이트가 활성화 되어 있거나 유지 시간이 끝난 경우 게이트를 닫음 */
  void removeGate()
  {
    if (isGateCreated && (!isGateActivated()) && (time(NULL) - gateGenerated > GATE_DURATION_SEC))
    {
      isGateCreated = false;
      // 맵에 기록되어 있는 게이트 정보 삭제
      mapCodes[gateEntranceY][gateEntranceX] = WALL_CODE;
      mapStatus[gateEntranceY][gateEntranceX] = Wall();
      mapCodes[gateExitY][gateExitX] = WALL_CODE;
      mapStatus[gateExitY][gateExitX] = Wall();
    }
  }

  /* snake.useGate를 호출하는 메서드
     Precondition: turnOnGate가 호출되는 시점에 head의 좌표가 반드시 진입 좌표여야 한다. */
  void turnOnGate(Snake &s)
  {
    Body head = *(s.getHead());
    Pos headPos = head.getPos();

    // gateAtivationLeft 기록을 시작함(snake의 몸 길이만큼 게이트 활성화를 유지함)
    // turnOnGate -> 해당 변수 감소 순서대로 수행되고, 머리의 위치를 미리 옮겨 놓으므로 몸 길이보다 1 작게 초기화 해주는 것이 맞음.
    gateActivationLeft = s.getLength() - 1;

    // 만약 head가 들어가는 게이트가 이전에 설정해놓은 진입 게이트가 아니라면,
    // 진입 게이트와 출구 게이트를 서로 바꿔 현재 접촉한 게이트가 진입 게이트임을 보장한다.
    if (headPos != Pos(gateEntranceX + 1, gateEntranceY + 1))
    {
      int tmp = gateEntranceX;
      gateEntranceX = gateExitX;
      gateExitX = tmp;

      tmp = gateEntranceY;
      gateEntranceY = gateExitY;
      gateExitY = tmp;
    }

    // 위치와 스케줄 갱신이 모두 완료된 상태이므로, 규칙에 따라 현재 게이트와 겹쳐 있는
    // 헤드의 좌표를 반대쪽 헤드로 옮겨주고 이어지는 Body들의 스케줄을 조정해주면 됨
    char symbolExitGateAtEdge = isExitGateAtEdge();

    // 경우 1. 출구 게이트가 가장자리에 있을 경우
    if (symbolExitGateAtEdge != -1)
    {
      // 안쪽 방향으로 진출해야함 = 벽이 있던 방향의 반대 방향으로 진출해야함
      Direction outDirection = Direction::getOppositeDirection(symbolExitGateAtEdge);

      // 진행 방향을 수정해줌
      s.setLastDirection(outDirection);

      // 나가는 게이트의 진출 방향으로 head의 위치를 바꿔줌
      // int outX = gateExitX - outDirection.getXDirection();
      // int outY = gateExitY - outDirection.getYDirection();

      int outX = gateExitX;
      int outY = gateExitY;

      switch (symbolExitGateAtEdge)
      {
      case 'L':
        outX += 2;
        outY += 1;
        break;
      case 'R':
        outY += 1;
        break;
      case 'U':
        outX += 1;
        outY += 2;
        break;
      case 'D':
        outX += 1;
        break;
      }

      (*(s.getHead())).setLastSchedule(Pos(outX, outY));
    }
    // 경우 2. 출구 게이트가 가장자리에 있지 않을 경우
    else
    {
      // 원래 진출하려던 방향의 상대좌표
      int directionX = s.getlastdirection().getXDirection();
      int directionY = s.getlastdirection().getYDirection();

      // 규칙 1. 원래 진출하려던 방향이 뚫려 있는 경우 그대로 진출
      if (mapCodes[gateExitY + directionY][gateExitX + directionX] == EMPTY_CODE)
      {
        int outX = gateExitX + directionX;
        int outY = gateExitY + directionY;

        switch (s.getlastdirection().getSymbol())
        {
        case 'L':
          outX += 1;
          outY += 1;
          break;
        case 'R':
          outX += 1;
          outY += 1;
          break;
        case 'U':
          outX += 1;
          outY += 1;
          break;
        case 'D':
          outX += 1;
          outY += 1;
          break;
        }

        (*(s.getHead())).setLastSchedule(Pos(outX, outY));
      }
      // 규칙 2. 원래 진출하려던 방향이 막혀 있는 경우
      else
      {
        // 1. 시계 방향 시도
        Direction clockwiseDir = s.getlastdirection().getClockwise();
        int clockwiseX = clockwiseDir.getXDirection();
        int clockwiseY = clockwiseDir.getYDirection();
        if (mapCodes[gateExitY + clockwiseY][gateExitX + clockwiseX] == EMPTY_CODE)
        {
          int outX = gateExitX + clockwiseX;
          int outY = gateExitY + clockwiseY;

          switch (s.getlastdirection().getSymbol())
          {
          case 'L':
            outX += 1;
            outY += 1;
            break;
          case 'R':
            outX += 1;
            outY += 1;
            break;
          case 'U':
            outX += 1;
            outY += 1;
            break;
          case 'D':
            outX += 1;
            outY += 1;
            break;
          }

          (*(s.getHead())).setLastSchedule(Pos(outX, outY));
          s.setLastDirection(clockwiseDir);
          return;
        }

        // 2. 반시계 방향 시도
        Direction counterClockwiseDir = s.getlastdirection().getCounterClockwise();
        int counterX = counterClockwiseDir.getXDirection();
        int counterY = counterClockwiseDir.getYDirection();
        if (mapCodes[gateExitY + counterY][gateExitX + counterX] == EMPTY_CODE)
        {
          int outX = gateExitX + counterX;
          int outY = gateExitY + counterY;

          switch (s.getlastdirection().getSymbol())
          {
          case 'L':
            outX += 1;
            outY += 1;
            break;
          case 'R':
            outX += 1;
            outY += 1;
            break;
          case 'U':
            outX += 1;
            outY += 1;
            break;
          case 'D':
            outX += 1;
            outY += 1;
            break;
          }

          (*(s.getHead())).setLastSchedule(Pos(outX, outY));
          s.setLastDirection(counterClockwiseDir);
          return;
        }

        // 3. 반대 방향 시도
        Direction oppositeDir = Direction::getOppositeDirection(s.getlastdirection().getSymbol());
        int oppositeX = oppositeDir.getXDirection();
        int oppositeY = oppositeDir.getYDirection();
        if (mapCodes[gateExitY + oppositeY][gateExitX + oppositeX] == EMPTY_CODE)
        {
          int outX = gateExitX + oppositeX;
          int outY = gateExitY + oppositeY;

          switch (s.getlastdirection().getSymbol())
          {
          case 'L':
            outX += 1;
            outY += 1;
            break;
          case 'R':
            outX += 1;
            outY += 1;
            break;
          case 'U':
            outX += 1;
            outY += 1;
            break;
          case 'D':
            outX += 1;
            outY += 1;
            break;
          }

          (*(s.getHead())).setLastSchedule(Pos(outX, outY));
          s.setLastDirection(oppositeDir);
          return;
        }
      }
    }
  }

  /* Snake Head가 게이트 좌표와 일치하는지 확인하는 메서드 */
  bool isHeadAtGate(Snake &s)
  {
    Body head = *(s).getHead();
    Pos headPos = head.getPos();

    return ((headPos == Pos(gateExitX + 1, gateExitY + 1)) || (headPos == Pos(gateEntranceX + 1, gateEntranceY + 1)));
  }

  /* Snake의 마지막 Body가 진입 게이트와 겹치는지 확인하는 메서드*/
  bool isTailAtEntranceGate(Snake &s)
  {
    Body tail = s.getLastBody();
    Pos tailPos = tail.getPos();

    return (tailPos == Pos(gateEntranceX + 1, gateEntranceY + 1));
  }

  /* 출구 게이트의 좌표가 가장자리인지 확인하는 메서드
     가장자리인 경우 상 우 하 좌 방향을 판별하여 각각 'U', 'R', 'D', 'L' 코드를 반환하고
     가장자리가 아닌 경우 -1을 반환한다. */
  char isExitGateAtEdge()
  {
    // 왼쪽 벽인 경우
    if (gateExitX == 0)
      return 'L';
    else if (gateExitX == (SIZE_X - 1))
      return 'R';
    else if (gateExitY == 0)
      return 'U';
    else if (gateExitY == (SIZE_Y - 1))
      return 'D';
    else
      return -1;
  }

  /* Snake Head가 아이템의 좌표와 일치하는지 확인하고 사용하는 메서드
      (Snake.h에 있던 useItem() 함수 기능을 합쳤습니다!)*/
  bool useItem(Snake &s)
  {
    Body snake_head = *(s).getHead();
    for (int i = 0; i < SIZE_Y; i++)
    {
      for (int j = 0; j < SIZE_X; j++)
      {
        if ((mapCodes[i][j] == GROWTH_CODE) && (snake_head.get_currentx() == j + 1) && (snake_head.get_currenty() == i + 1))
        {
          s.lengthen();
          int use_growth = s.getGrowthCnt();
          s.setGrowhCnt(use_growth + 1);
          removeGrowth(1);
          return true;
        }
        else if ((mapCodes[i][j] == POISON_CODE) && (snake_head.get_currentx() == j + 1) && (snake_head.get_currenty() == i + 1))
        {
          if (s.shorten())
          {
            int use_poison = s.getPoisonCnt();
            s.setPoisonCnt(use_poison + 1);
            removePoison(1);
            return true;
          }
          else
          {
            int use_poison = s.getPoisonCnt();
            s.setPoisonCnt(use_poison + 1);
            removePoison(1);
            return false;
          }
        }
      }
    }
    return true;
  }
};

#endif
