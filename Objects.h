#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include <ctime>
#include "Snake.h"

class Object
{
protected:
    // 화면에 보이게 될 기호 문자(유니코드이므로 string으로 지정함)
    string symbol;

public:
    Object()
    {
        symbol = "?";
    }

    string getSymbol()
    {
        return symbol;
    }
};

class Empty : public Object
{
public:
    Empty()
    {
        symbol = " ";
    }
};

class Wall : public Object
{
public:
    Wall()
    {
        symbol = "#";
    }
};

class ImmuneWall : public Wall
{
public:
    ImmuneWall()
    {
        symbol = "E";
    }
};

class Gate : public Wall
{
    Gate()
    {
        symbol = "@";
    }
};

class Item : public Object
{
protected:
    // 아이템이 생성된 절대 초 -> 아이템이 사라지는 때를 계산하기 위한 것
    time_t CREATED_TIME;

public:
    Item()
    {
        symbol = "*";
        CREATED_TIME = time(NULL);
    }

    // Snake의 상태를 갱신함
    // Item을 상속받는 Growth, Poison에서 재정의해야함
    virtual bool
    affect(Snake &s)
    {
        return false;
    }
};

class Growth : public Item
{
public:
    Growth()
    {
        symbol = "+";
    }

    // Snake의 상태를 갱신
    // 길이 +1
    // Growth Mission + 1
    // Growth Score + 1
    // Body 상태 갱신
    // --> 항상 true를 반환
    bool affect(Snake &s);
};

class Poison : public Item
{
public:
    Poison()
    {

        symbol = "-";
    }

    // Snake의 상태를 갱신
    // 길이 -1
    // Poison Mission + 1
    // Poison Score - 1
    // Body 상태 갱신
    // 뱀이 죽으면 false, 죽지 않았으면 true
    bool affect(Snake &s);
};

#endif
