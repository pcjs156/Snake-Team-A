#include "Snake.h"

// Direction begins =============================================
// 해당 객체의 시계 방향으로 회전하는 방향을 반환
Direction Direction::getClockwise()
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
Direction Direction::getCounterClockwise()
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

// 전달된 방향이 메서드 호출 방향과 정반대라면 true, 아니라면 false를 반환
bool Direction::isOppositeWith(Direction &d)
{
    return Direction::getOppositeDirection(d.getSymbol()) == *this;
}
// Direction finished ===========================================

// Body begins ==================================================
void Body::updateHeadSchedule(char symbol)
{
    if (symbol == 'L')
        nextPos = Pos(currentPos.x - 1, currentPos.y);
    else if (symbol == 'R')
        nextPos = Pos(currentPos.x + 1, currentPos.y);
    else if (symbol == 'U')
        nextPos = Pos(currentPos.x, currentPos.y - 1);
    else if (symbol == 'D')
        nextPos = Pos(currentPos.x, currentPos.y + 1);
}
// Body finished ================================================

// Snake begins =================================================
Snake::Snake(int initX, int initY)
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

Snake &Snake::operator=(const Snake &s)
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
int Snake::getLength() { return length; }
int Snake::getGrowthCnt() { return growthCnt; }
int Snake::getPoisonCnt() { return poisonCnt; }
int Snake::getGateCnt() { return gateCnt; }
vector<Body> Snake::getBodies() { return bodies; }
Direction Snake::getlastdirection() { return lastDirection; }

Body *Snake::getHead()
{
    return &bodies[0];
}
Pos Snake::getHeadPos()
{
    return bodies[0].getPos();
}
Body Snake::getLastBody()
{
    return bodies[length - 1];
}

// Setter =============================================
void Snake::setGrowhCnt(int cnt) { this->growthCnt = cnt; };
void Snake::setPoisonCnt(int cnt) { this->poisonCnt = cnt; };
void Snake::setGateCnt(int cnt) { this->gateCnt; };
void Snake::setHeadPos(Pos p)
{
    bodies[0].setPos(p);
}
void Snake::setLastDirection(Direction d)
{
    lastDirection = d;
}

// 몸 길이 관련 ========================================
//snake의 몸 길이를 1만큼 줄이기
bool Snake::shorten()
{
    if (length == MIN_LENGTH)  //현재 몸 길이가 3인 경우 줄일 수가 없기 때문에 false 반환
        return false;
    else   //현재 몸 길이가 3보다 큰 경우
    {
        length -= 1;
        bodies.pop_back();
        return true;
    }
}
/*snake의 몸 길이를 1만큼 늘리기
  snake의 lastDirection의 값에 따라 새로운 body객체 생성 및 bodies 벡터에 추가
  몸 길이를 나타내는 변수에 -1*/
void Snake::lengthen()
{
    char symbol = this->lastDirection.getSymbol();
    if (symbol == 'L')
        bodies.push_back(Body(bodies[length - 1].getCurrentX() + 1, bodies[length - 1].getCurrentY(), bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));
    if (symbol == 'R')
        bodies.push_back(Body(bodies[length - 1].getCurrentX() - 1, bodies[length - 1].getCurrentY(), bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));
    if (symbol == 'U')
        bodies.push_back(Body(bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY() + 1, bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));
    if (symbol == 'D')
        bodies.push_back(Body(bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY() - 1, bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));

    length += 1;
}

// 이동 관련 ============================================
bool Snake::changeHeadDirection(Direction newDirection)
{
    char symbol = this->lastDirection.getSymbol();

    // 새 방향이 기존 방향과 정반대인 경우 게임 오버
    if (newDirection.isOppositeWith(lastDirection))
        return false;
    // 새로 입력된 방향이 유효한 경우
    else if (newDirection.getSymbol() != 'X')
    {
        this->lastDirection = newDirection;
        return true;
    }
    // 알 수 없는 심볼인 경우
    else
        return false;
}

void Snake::moveTo()
{
    // 마지막으로 입력된 방향의 심볼
    char symbol = this->lastDirection.getSymbol();

    // 머리 위치 갱신
    bodies[0].updateCurrentPos();
    bodies[0].updateHeadSchedule(symbol);

    // 몸통 위치 갱신
    for (int i = 1; i < bodies.size(); i++)
    {
        bodies[i].updateCurrentPos();
        bodies[i].setNextPos(Pos(bodies[i - 1].getCurrentX(), bodies[i - 1].getCurrentY()));
    }
}

// 상태 점검 =============================================
bool Snake::isBumpedToBody()
{
    for (int i = 1; i < bodies.size(); i++)
    {
        if ((bodies[0].getCurrentX() == bodies[i].getCurrentX()) && (bodies[0].getCurrentY() == bodies[i].getCurrentY()))
        {
            return true;
        }
    }
    return false;
}
// Snake finished ===============================================
