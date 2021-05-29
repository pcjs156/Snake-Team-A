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

  // char형 매개 변수(이하 symbol)를 받아 Direction 객체를 만들어 반환함
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

  // 전달된 symbol의 반대 방향 Direction 객체를 만들어 반환함
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
  void set_direction(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  // 해당 객체의 X 방향을 반환
  int getXDirection()
  {
    return x;
  }

  // 해당 객체의 Y 방향을 반환
  int getYDirection()
  {
    return y;
  }

  // 해당 객체의 시계 방향으로 회전하는 방향을 반환
  Direction getClockwise()
  {
    switch (getSymbol())
    {
    // 오른쪽 -> 아래
    case 'R':
      return Direction::getDirectionBySymbol('D');
    // 아래 -> 왼쪽
    case 'D':
      return Direction::getDirectionBySymbol('L');
    // 왼쪽 -> 위쪽
    case 'L':
      return Direction::getDirectionBySymbol('U');
    // 위쪽 -> 오른쪽
    case 'U':
      return Direction::getDirectionBySymbol('R');
    default:
      cout << "ERROR: getClockwise() got invalid direction" << endl;
      return Direction(0, 0);
    }
  }

  // 해당 객체의 반시계방향으로 회전하는 방향을 반환
  Direction getCounterClockwise()
  {
    switch (getSymbol())
    {
    // 오른쪽 -> 위
    case 'R':
      return Direction::getDirectionBySymbol('U');
    // 위 -> 왼쪽
    case 'U':
      return Direction::getDirectionBySymbol('L');
    // 왼쪽 -> 아래쪽
    case 'L':
      return Direction::getDirectionBySymbol('D');
    // 아래쪽 -> 오른쪽
    case 'D':
      return Direction::getDirectionBySymbol('R');
    default:
      cout << "ERROR: getCounterClockwise() got invalid direction" << endl;
      return Direction(0, 0);
    }
  }

  Direction &operator=(const Direction &s)
  {
    this->x = s.x;
    this->y = s.y;
    return *this;
  }
};

/* Snake를 구성하는 몸통(+머리) */
class Body
{
private:
  // Body의 현재 위치
  Pos currentPos;
  /* Body가 앞으로 이동해야 할 좌표들의 모음
       update를 수행할 때마다 현재 좌표를 맨 앞의 요소로 갱신하고,
       해당 스케줄을 삭제한다. */
  vector<Pos> scheduleQueue;

public:
  Body() {}

  // 항상 최초 위치 바로 이후의 스케줄까지 입력받아야 한다.
  Body(int initX, int initY, int nextX, int nextY)
  {
    currentPos = Pos(initX, initY);
    scheduleQueue.push_back(Pos(nextX, nextY));
  }

  Pos getPos()
  {
    return currentPos;
  }

  /* Body의 현재 위치를 다음 위치로 갱신하고, 수행된 스케줄을 삭제한다.
       수행 결과에 따라 다음의 값을 반환한다.
       true: 정상 수행
       false: scheduleQueue가 비어 있는 경우 */
  bool current_update()
  {
    if (scheduleQueue.size() == 0)
      return false;
    // 현재 위치 갱신
    currentPos = scheduleQueue[0];
    // 맨 앞의 스케줄 삭제
    scheduleQueue.erase(scheduleQueue.begin());
    return true;
  }
  //snake head 방향에 따라 snake head의 이동 스케줄을 새롭게 추가한다.
  void head_schedule_update(char symbol)
  {
    if (symbol == 'L')
    {
      scheduleQueue.push_back(Pos(currentPos.x - 1, currentPos.y));
    }
    else if (symbol == 'R')
    {
      scheduleQueue.push_back(Pos(currentPos.x + 1, currentPos.y));
    }
    else if (symbol == 'U')
    {
      scheduleQueue.push_back(Pos(currentPos.x, currentPos.y - 1));
    }
    else if (symbol == 'D')
    {
      scheduleQueue.push_back(Pos(currentPos.x, currentPos.y + 1));
    }
  }
  //snake head를 제외한 나머지 몸통의 이동 스케줄을 새롭게 추가한다
  void schedule_update(Pos next_pos)
  {
    scheduleQueue.push_back(next_pos);
  }
  //snake 몸통의 현재 좌표를 반환
  int get_currentx() { return currentPos.x; }
  int get_currenty() { return currentPos.y; }

  // 좌표 갱신
  void setPos(Pos p)
  {
    currentPos = Pos(p.x, p.y);
  }

  // 마지막 스케줄을 해당 좌표로 바꿔줌
  void setLastSchedule(Pos p)
  {
    scheduleQueue[scheduleQueue.size() - 1] = p;
  }

  vector<Pos> getSchedule()
  {
    return scheduleQueue;
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
  Snake() {}
  // Constructor
  Snake(int initX, int initY)
  {
    // ()= 연산자 오버로딩에 사용됨)
    this->initX = initX;
    this->initY = initY;

    // 몸 길이 초기화
    length = 3;
    // 아이템 획득/게이트 통과 횟수 초기화
    growthCnt = 0;
    poisonCnt = 0;
    gateCnt = 0;

    // 왼쪽 방향을 기본 방향으로 함
    lastDirection = Direction::getDirectionBySymbol('L');

    // 맨 처음에는 ">~~" 이렇게 왼쪽을 보고 움직이도록 설정
    bodies.push_back(Body(initX, initY, initX - 1, initY));
    bodies.push_back(Body(initX + 1, initY, initX, initY));
    bodies.push_back(Body(initX + 2, initY, initX + 1, initY));
  }

  Snake &operator=(const Snake &s)
  {
    // ()= 연산자 오버로딩에 사용됨)
    this->initX = s.initX;
    this->initY = s.initY;

    // 몸 길이 초기화
    length = 3;
    // 아이템 획득/게이트 통과 횟수 초기화
    growthCnt = 0;
    poisonCnt = 0;
    gateCnt = 0;

    // 왼쪽 방향을 기본 방향으로 함
    lastDirection = Direction::getDirectionBySymbol('L');

    // 맨 처음에는 ">~~" 이렇게 왼쪽을 보고 움직이도록 설정
    bodies.push_back(Body(initX, initY, initX - 1, initY));
    bodies.push_back(Body(initX + 1, initY, initX, initY));
    bodies.push_back(Body(initX + 2, initY, initX + 1, initY));

    return *this;
  }

  // Getter ============================================
  int getLength() { return length; }
  int getGrowthCnt() { return growthCnt; }
  int getPoisonCnt() { return poisonCnt; }
  int getGateCnt() { return gateCnt; }
  vector<Body> getBodies() { return bodies; }
  Direction getlastdirection() { return lastDirection; }
  /* body의 맨 앞 레퍼런스를 반환함
       Precondition: bodies가 비어 있으면 안됨 */
  Body *getHead()
  {
    return &bodies[0];
  }
  /* head의 좌표를 Pos로 반환함 */
  Pos getHeadPos()
  {
    return bodies[0].getPos();
  }
  /* 맨 마지막 Body의 레퍼런스를 반환함 */
  Body getLastBody()
  {
    return bodies[length - 1];
  }

  // Setter =============================================
  void setLength(int length) { this->length = length; };
  void setGrowhCnt(int cnt) { this->growthCnt = cnt; };
  void setPoisonCnt(int cnt) { this->poisonCnt = cnt; };
  void setGateCnt(int cnt) { this->gateCnt; };
  void setHeadPos(Pos p)
  {
    bodies[0].setPos(p);
  }
  void setLastDirection(Direction d)
  {
    lastDirection = d;
  }

  // 몸 길이 관련 ========================================
  /* Snake의 길이를 1만큼 줄이는 함수
     길이를 더 줄일 수 없다면(줄이면 뱀이 죽는다면) 길이를 갱신하지 않고 false를 반환
     길이를 더 줄일 수 있다면 길이와 Body 정보를 갱신하고 true를 반환 */
  bool shorten()
  {
    if (length == MIN_LENGTH)
      return false;
    else
    {
      length -= 1;
      bodies.pop_back();
      return true;
    }
  }
  /* Snake의 길이를 1만큼 늘이고 Body 정보를 갱신하는 함수 */
  void lengthen()
  {
    char symbol = this->lastDirection.getSymbol();
    if (symbol == 'L')
    {
      bodies.push_back(Body(bodies[length - 1].get_currentx() + 1, bodies[length - 1].get_currenty(), bodies[length - 1].get_currentx(), bodies[length - 1].get_currenty()));
      length += 1;
    }
    if (symbol == 'R')
    {
      bodies.push_back(Body(bodies[length - 1].get_currentx() - 1, bodies[length - 1].get_currenty(), bodies[length - 1].get_currentx(), bodies[length - 1].get_currenty()));
      length += 1;
    }
    if (symbol == 'U')
    {
      bodies.push_back(Body(bodies[length - 1].get_currentx(), bodies[length - 1].get_currenty() + 1, bodies[length - 1].get_currentx(), bodies[length - 1].get_currenty()));
      length += 1;
    }
    if (symbol == 'D')
    {
      bodies.push_back(Body(bodies[length - 1].get_currentx(), bodies[length - 1].get_currenty() - 1, bodies[length - 1].get_currentx(), bodies[length - 1].get_currenty()));
      length += 1;
    }
  }

  // 아이템 관련 =========================================
  /* 게이트를 사용해 Body의 위치와 이동 경로를 수정함. 일관성을 위해 항상 true를 반환하도록 설정 */
  bool useGate(Manager &m, Direction direction);

  // 이동 관련 ============================================
  /* newDirection 방향으로 Snake를 이동시키고, lastDirection을 갱신함
       만약 newDirection이 lastDirection과 반대 방향이라면, false를 반환해 게임을 종료할 수 있도록 하고
       그렇지 않은 경우 true를 반환하여 메서드가 제대로 실행 되었음을 외부에 알림
       단, 아이템이나 벽 충돌, 몸통 충돌 등으로 인한 게임 종료는 함수 외부에서 판단함 */

  //snake의 lastDirection을 newDirection 방향으로 갱신함
  bool change_head_direction(Direction newDirection)
  {
    char symbol = this->lastDirection.getSymbol();
    if (symbol == 'L')
    {
      //newDirection 방향이 기존 방향과 반대될 때
      if (newDirection.getSymbol() == 'R')
        return false;
      //newDirection 방향이 반대 방향은 제외한 기존 방향과 다를 때
      else if (newDirection.getSymbol() != 'X')
      {
        this->lastDirection = newDirection;
        return true;
      }
      //newDirection 방향이 기존 방향과 동일할 때
      else
      {
        return true;
      }
    }
    if (symbol == 'R')
    {
      if (newDirection.getSymbol() == 'L')
        return false;
      else if (newDirection.getSymbol() != 'X')
      {
        this->lastDirection = newDirection;
        return true;
      }
      else
      {
        return true;
      }
    }
    if (symbol == 'U')
    {
      if (newDirection.getSymbol() == 'D')
        return false;
      else if (newDirection.getSymbol() != 'X')
      {
        this->lastDirection = newDirection;
        return true;
      }
      else
      {
        return true;
      }
    }
    if (symbol == 'D')
    {
      if (newDirection.getSymbol() == 'U')
        return false;
      else if (newDirection.getSymbol() != 'X')
      {
        this->lastDirection = newDirection;
        return true;
      }
      else
      {
        return true;
      }
    }
  }
  //snake의 몸통 이동 데이터 갱신
  void moveTo()
  {
    char symbol = this->lastDirection.getSymbol();
    if (symbol == 'L')
    {
      //snake head의 현재 위치를 다음 위치로 갱신
      if (bodies[0].current_update())
      {
        //snake 방향에 따른 snake head의 이동 스케줄을 새롭게 추가
        bodies[0].head_schedule_update(symbol);
        //snake head를 제외한 몸통들의 현재 위치를 다음 위치로 갱신
        for (int i = 1; i < bodies.size(); i++)
        {
          if (bodies[i].current_update())
          {
            //snake head를 제외한 몸통들이 각각 자신의 앞에 있는 몸통의 현재 좌표값을 다음 이동 스케줄에 새롭게 추가
            bodies[i].schedule_update(Pos(bodies[i - 1].get_currentx(), bodies[i - 1].get_currenty()));
          }
        }
      }
    }
    if (symbol == 'R')
    {
      if (bodies[0].current_update())
      {
        bodies[0].head_schedule_update(symbol);
        for (int i = 1; i < bodies.size(); i++)
        {
          if (bodies[i].current_update())
          {
            bodies[i].schedule_update(Pos(bodies[i - 1].get_currentx(), bodies[i - 1].get_currenty()));
          }
        }
      }
    }
    if (symbol == 'U')
    {
      if (bodies[0].current_update())
      {
        bodies[0].head_schedule_update(symbol);
        for (int i = 1; i < bodies.size(); i++)
        {
          if (bodies[i].current_update())
          {
            bodies[i].schedule_update(Pos(bodies[i - 1].get_currentx(), bodies[i - 1].get_currenty()));
          }
        }
      }
    }
    if (symbol == 'D')
    {
      if (bodies[0].current_update())
      {
        bodies[0].head_schedule_update(symbol);
        for (int i = 1; i < bodies.size(); i++)
        {
          if (bodies[i].current_update())
          {
            bodies[i].schedule_update(Pos(bodies[i - 1].get_currentx(), bodies[i - 1].get_currenty()));
          }
        }
      }
    }
  }
  // 상태 점검 =============================================
  /* 머리가 자신의 몸통과 부딪혔는지 확인하는 메서드
       Precondition: moveTo 메서드에 의해 이동이 완전히 끝난 후에 호출해야 함*/
  bool isBumpedToBody()
  {
    for (int i = 1; i < bodies.size(); i++)
    {
      if ((bodies[0].get_currentx() == bodies[i].get_currentx()) && (bodies[0].get_currenty() == bodies[i].get_currenty()))
      {
        return true;
      }
    }
    return false;
  }
};

#endif
