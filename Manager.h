#ifndef __MANAGER_H__
#define __MANAGER_H__

/* !!! Gate, Item쪽 구현은 Snake쪽 틀이 좀 잡힌 다음 구체화하는게 좋을 것 같음. 변경 가능성 높음. !!!*/
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "Snake.h"
#include "Objects.h"

// 맵을 구성하는 요소들의 코드
const int EMPTY_CODE = 0;           // 빈 공간
const int WALL_CODE = 1;            // 벽
const int IMMUNE_WALL_CODE = 2;     // 게이트가 생기지 않는 벽
const int INIT_SNAKE_HEAD_CODE = 3; // Snake의 머리 부분

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
  // 현재 게이트가 생성되어 있는지 여부를 기록하는 변수
  bool isGateCreated;
  // 게이트가 열려 있는지(뱀이 통과하는 중인지) 기록하는 변수
  bool isGateOpened;
  // 들어가는 게이트와 나오는 게이트의 좌표
  // 안정성을 위해 아이템이 생성되어 있지 않을 때에는 -1로 할당해 놓을 것!
  int gateEntranceX, gateEntranceY;
  int gateExitX, gateExitY;
  //

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
        growth_item_y = rand() % 26;
        growth_item_x = rand() % 56;
      }
      mapStatus[growth_item_y][growth_item_x] = growth_item;
      mapCodes[growth_item_y][growth_item_x] = 4;
      growthCnt = 1;
    }
  }
  /* Growth를 삭제하는 메서드
    Growth가 생성되어 있다면 Growth 자리를 nullptr로 두고 좌표를 -1로 변경한 뒤 true를 반환한다.
    Growth가 생성되어 있지 않다면 false를 반환한다. */
  bool removeGrowth(int check)
  {
    time_t end = time(NULL);
    if ((check == 0) && (growthCnt == 1) && (end - init_growth > 5))
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
        poison_item_y = rand() % 26;
        poison_item_x = rand() % 56;
      }
      mapStatus[poison_item_y][poison_item_x] = poison_item;
      mapCodes[poison_item_y][poison_item_x] = 5;
      poisonCnt = 1;
    }
  }
  /* Poison을 삭제하는 메서드
    Poison이 생성되어 있다면 Poison 자리를 nullptr로 두고 좌표를 -1로 변경한 뒤 true를 반환한다.
    Poison이 생성되어 있지 않다면 false를 반환한다. */
  bool removePoison(int check)
  {
    time_t end = time(NULL);
    if ((check == 0) && (poisonCnt == 1) && (end - init_poison > 5))
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

  /* 게이트를 생성하는 메서드
       게이트가 생성되어 있거나 게이트를 통과하고 있는 경우 false를 반환하고,
       게이트가 생성되어 있지 않은 경우 게이트를 생성한 후 true를 반환함*/
  bool createGate()
  {
  }

  /* 게이트를 삭제하는 메서드
       게이트가 생성되어 있거나 게이트를 통과하고 있는 경우 false를 반환하고,
       게이트가 생성되어 있지 않은 경우 게이트를 생성한 후 true를 반환함*/
  bool removeGate() { return true; }

  /* Snake Head가 아이템의 좌표와 일치하는지 확인하고 사용하는 메서드
      (Snake.h에 있던 useItem() 함수 기능을 합쳤습니다!)*/
  bool useItem(Snake &s)
  {
    Body snake_head = s.getHead();
    for (int i = 0; i < SIZE_Y; i++)
    {
      for (int j = 0; j < SIZE_X; j++)
      {
        if ((mapCodes[i][j] == 4) && (snake_head.get_currentx() == j + 1) && (snake_head.get_currenty() == i + 1))
        {
          s.lengthen();
          int use_growth = s.getGrowthCnt();
          s.setGrowhCnt(use_growth + 1);
          removeGrowth(1);
          return true;
        }
        else if ((mapCodes[i][j] == 5) && (snake_head.get_currentx() == j + 1) && (snake_head.get_currenty() == i + 1))
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

  /* Snake Head가 게이트 좌표와 일치하는지 확인하는 메서드 */
  bool isAtGate() { return true; }
};

#endif
