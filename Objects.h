#include "Snake.h"

class Object
{
};

class Wall : public Object
{
};

class ImmuneWall : public Wall
{
};

class Gate : public Wall
{
};

class Item : public Object
{
protected:
    const int LEN_AFFECT = 1;     // 길이 변경을 얼마만큼 할지
    const int MISSION_AFFECT = 1; // 미션 점수는 얼마만큼 변경할지
    const int SCORE_AFFECT = 1;   // 종합 점수는 얼마만큼 변경할지

public:
    // Snake의 상태를 갱신함
    // Item을 상속받는 Growth, Poison에서 재정의해야함
    virtual bool affect(Snake &s)
    {
        return false;
    }
};

class Growth : public Item
{
public:
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
    // Snake의 상태를 갱신
    // 길이 -1
    // Poison Mission + 1
    // Poison Score - 1
    // Body 상태 갱신
    // 뱀이 죽으면 false, 죽지 않았으면 true
    bool affect(Snake &s);
};