#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Manager;
class Item;

/* 좌표를 직관적으로 표시하기 위한 클래스 */
class Pos
{
public:
  int x;
  int y;

public:
  Pos() {}

  Pos(int x, int y) : x(x), y(y) {}

  //=연산자
  Pos &operator=(const Pos &s)
  {
    this->x = s.x;
    this->y = s.y;
    return *this;
  }

  friend bool operator==(const Pos &p1, const Pos &p2)
  {
    return (p1.x == p2.x) && (p1.y == p2.y);
  }

  friend bool operator!=(const Pos &p1, const Pos &p2)
  {
    return !(p1 == p2);
  }

  friend ostream &operator<<(ostream &os, const Pos &p)
  {
    cout << "(" << p.x << ", " << p.y << ")";
    return os;
  }
};

/* 방향을 표시하기 위한 클래스. 좌표와 심볼로 표현할 수 있음. */
class Direction
{
private:
  int x;
  int y;

public:
  Direction() : x(0), y(0) {}

  Direction(int x, int y) : x(x), y(y) {}

  // char형 매개 변수(이하 symbol)를 받아 Direction 객체를 만들어 반환
  static Direction getDirectionBySymbol(char symbol)
  {
    switch (symbol)
    {
    case 'L':
      return Direction(-1, 0);
    case 'R':
      return Direction(1, 0);
    case 'U':
      return Direction(0, -1);
    case 'D':
      return Direction(0, 1);
    }

    cout << "ERROR: Invalid Direction Symbol '" << symbol << "'\n";
    return Direction(0, 0);
  }

  // 전달된 symbol의 반대 방향 Direction 객체를 만들어 반환
  static Direction getOppositeDirection(char symbol)
  {
    switch (symbol)
    {
    case 'L':
      return Direction::getDirectionBySymbol('R');
    case 'R':
      return Direction::getDirectionBySymbol('L');
    case 'U':
      return Direction::getDirectionBySymbol('D');
    case 'D':
      return Direction::getDirectionBySymbol('U');
    }

    cout << "ERROR: On getOppositeDirection, Invalid Direction Symbol '" << symbol << "'\n";
    return Direction(0, 0);
  }

  // 해당 객체의 방향을 symbol로 표현해 반환
  char getSymbol()
  {
    // 왼쪽
    if (x == -1 && y == 0)
      return 'L';
    // 오른쪽
    else if (x == 1 && y == 0)
      return 'R';
    // 위쪽
    else if (x == 0 && y == -1)
      return 'U';
    // 아래쪽
    else if (x == 0 && y == 1)
      return 'D';
    // 알 수 없는 경우
    else
      return 'X';
  }

  //해당 객체의 방향을 변경
  void setDirection(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  // 해당 객체의 좌표 성분을 반환
  int getXDirection() { return x; }
  int getYDirection() { return y; }

  Direction &operator=(const Direction &s)
  {
    this->x = s.x;
    this->y = s.y;
    return *this;
  }

  bool operator==(const Direction &d)
  {
    return (this->x == d.x) && (this->y == d.y);
  }

  bool operator!=(const Direction &d)
  {
    return !(operator==(d));
  }

  // 해당 객체의 시계 방향으로 회전하는 방향을 반환
  Direction getClockwise();

  // 해당 객체의 반시계방향으로 회전하는 방향을 반환
  Direction getCounterClockwise();

  // 전달된 방향이 메서드 호출 방향과 정반대라면 true, 아니라면 false를 반환
  bool isOppositeWith(Direction &d);
};

/* Snake를 구성하는 몸통(+머리) */
class Body
{
private:
  // Body의 현재 위치
  Pos currentPos;
  // Body가 다음으로 이동할 좌표
  Pos nextPos;

public:
  Body() {}

  // 항상 최초 위치 바로 이후의 스케줄까지 입력받아야 한다.
  Body(int initX, int initY, int nextX, int nextY)
  {
    currentPos = Pos(initX, initY);
    nextPos = Pos(nextX, nextY);
  }

  Pos getPos()
  {
    return currentPos;
  }

  // Body의 현재 위치를 다음 위치로 갱신한다.
  void updateCurrentPos()
  {
    currentPos = nextPos;
  }

  //snake head 방향에 따라 snake head의 이동 스케줄을 새롭게 추가한다.
  void updateHeadSchedule(char symbol);

  //snake 몸통의 현재 좌표를 반환
  int getCurrentX() { return currentPos.x; }
  int getCurrentY() { return currentPos.y; }

  // 현재 좌표 정보 갱신
  void setPos(Pos p)
  {
    currentPos = Pos(p.x, p.y);
  }

  // 다음 위치를 해당 좌표로 바꿔줌
  void setNextPos(Pos p)
  {
    nextPos = p;
  }

  // 다음 위치를 반환
  Pos getNextPos()
  {
    return nextPos;
  }
};

/* Snake의 상태를 저장/갱신하기 위한 클래스 */
class Snake
{
private:
  const static int MIN_LENGTH = 3; // 뱀의 최소 몸길이
  int length;                      // 뱀의 몸 길이
  int growthCnt;                   // Growth 획득 횟수
  int poisonCnt;                   // Posion 획득 횟수
  int gateCnt;                     // Gate 이용 횟수
  Direction lastDirection;         // Snake의 마지막 이동 방향
  vector<Body> bodies;             // Snake의 머리와 몸통들을 요소로 하는 벡터

  // *= 연산자 오버로딩에 사용됨)
  int initX, initY;

public:
  // Constructor
  Snake() {}
  Snake(int initX, int initY);
  Snake(Pos initPos);

  Snake &operator=(const Snake &s);

  // Getter ============================================
  // 몸 길이를 반환
  int getLength();
  // 획득한 growth의 개수를 반환
  int getGrowthCnt();
  // 획득한 poison의 개수를 반환
  int getPoisonCnt();
  // 게이트 통과 횟수를 반환
  int getGateCnt();
  // body 객체를 담은 vector를 반환
  vector<Body> getBodies();
  // 입력된 마지막 입력 방향을 반환
  Direction getlastdirection();

  /* body의 맨 앞 레퍼런스(머리)를 반환
       Precondition: bodies가 비어 있으면 안됨 */
  Body *getHead();
  // head의 좌표를 Pos로 반환
  Pos getHeadPos();
  // 맨 마지막 Body의 레퍼런스를 반환
  Body getLastBody();

  // Setter =============================================
  // 획득한 growth의 개수를 갱신
  void setGrowhCnt(int cnt);
  // 획득한 poison의 개수를 갱신
  void setPoisonCnt(int cnt);
  // 게이트 통과 횟수를 갱신
  void setGateCnt(int cnt);
  // snake의 머리 위치를 갱신
  void setHeadPos(Pos p);
  // 마지막으로 입력된 방향을 갱신
  void setLastDirection(Direction d);

  // 몸 길이 관련 ========================================
  /* Snake의 길이를 1만큼 줄이는 함수
     길이를 더 줄일 수 없다면(줄이면 뱀이 죽는다면) 길이를 갱신하지 않고 false를 반환
     길이를 더 줄일 수 있다면 길이와 Body 정보를 갱신하고 true를 반환 */
  bool shorten();

  // Snake의 길이를 1만큼 늘이고 Body 정보를 갱신하는 함수
  void lengthen();

  // 아이템 관련 =========================================
  // 게이트를 사용해 Body의 위치와 이동 경로를 수정함.일관성을 위해 항상 true를 반환하도록 설정 bool
  bool useGate(Manager &m, Direction direction);

  // 이동 관련 ============================================
  /* newDirection 방향으로 Snake를 이동시키고, lastDirection을 갱신함
       만약 newDirection이 lastDirection과 반대 방향이라면, false를 반환해 게임을 종료할 수 있도록 하고
       그렇지 않은 경우 true를 반환하여 메서드가 제대로 실행 되었음을 외부에 알림
       단, 아이템이나 벽 충돌, 몸통 충돌 등으로 인한 게임 종료는 함수 외부에서 판단함 */
  bool changeHeadDirection(Direction newDirection);

  //snake의 몸통 이동 데이터 갱신
  void moveTo();

  // 상태 점검 =============================================
  /* 머리가 자신의 몸통과 부딪혔는지 확인하는 메서드
       Precondition: moveTo 메서드에 의해 이동이 완전히 끝난 후에 호출해야 함*/
  bool isBumpedToBody();
};

#endif
