#include "Manager.h"

// Constructor/Destructor begins ==================================
Manager::Manager(int stageNumber) : STAGE_NUM(stageNumber)
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
    ifstream mapFile("./maps/map" + to_string(stageNumber) + ".txt");
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
    mapFile.close();

    // 규칙 파일을 여는데 실패하면 예외 발생(처리 안함)
    ifstream ruleFile("./rules/rule" + to_string(stageNumber) + ".txt");
    if (!ruleFile.is_open())
        throw RuleFileOpenFailedException();
    // 규칙 파일을 열었으면 스테이지 클리어 규칙에 해당하는 변수를 초기화
    else
    {
        int *conditionRefs[5] = {&maxBodyLength, &bodyLengthCondition, &growthCntCondition, &poisonCntCondition, &gateCntCondition};
        string buffer;
        // 최대 몸길이 / 몸길이 조건, Growth, Poison, Gate 사용 횟수 조건 5개의 규칙을 저장함
        for (int i = 0; i < 5; i++)
        {
            getline(ruleFile, buffer);
            *conditionRefs[i] = stoi(buffer);
        }
    }
    ruleFile.close();

    // 맵 파일 속 초기 Snake 위치/인스턴스 초기화
    int initSnakeX, initSnakeY;
    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++)
        {
            if (mapCodes[i][j] == INIT_snakeHead_CODE)
            {
                initSnakeY = i + 1;
                initSnakeX = j + 1;
                mapCodes[i][j] = 0; // 초기화에만 Snake Head 정보가 필요하므로 Empty로 놓음
            }
        }
    }
    initPos = Pos(initSnakeX, initSnakeY);
}

Manager::~Manager()
{
    // 맵 정보를 담은 동적 배열 해제
    for (int i = 0; i < SIZE_Y; i++)
        delete[] mapCodes[i];
    delete[] mapCodes;

    for (int i = 0; i < SIZE_Y; i++)
        delete[] mapStatus[i];
    delete[] mapStatus;
}

// Constructor/Destructor finished ================================
// GameOver Condition begins ======================================
bool Manager::isGameCleared(Snake &s)
{
    return (
        isBodyConditionCleared(s) &&
        isGrowthConditionCleared(s) &&
        isPoisonConditionCleared(s) &&
        isGateConditionCleared(s));
}

bool Manager::isMaxLength(Snake &s)
{
    return (s.getLength() == maxBodyLength);
}

bool Manager::isBodyConditionCleared(Snake &s)
{
    return (s.getLength() >= bodyLengthCondition);
}

bool Manager::isGrowthConditionCleared(Snake &s)
{
    return (s.getGrowthCnt() >= growthCntCondition);
}

bool Manager::isPoisonConditionCleared(Snake &s)
{
    return (s.getPoisonCnt() >= poisonCntCondition);
}

bool Manager::isGateConditionCleared(Snake &s)
{
    return (s.getGateCnt() >= gateCntCondition);
}

// GameOver Condition finished ====================================
// items begins ===================================================
/*이미 Growth 아이템이 생성되어 있으면 fasle 반환
  아이템이 생성되어 있지 않으면 아이템 생성 및 생성 시간 기록, 게임 맵에서 벽을 제외한 빈공간 중 랜덤한 좌표를 아이템 좌표로 설정
  아이템 생성 여부를 true로 표시 후 true 반환
  설정된 최대 몸길이에 도달했을 경우 아이템 생성 X */
bool Manager::createGrowth(Snake &s)
{
    if (isGrowthCreated || isMaxLength(s)) //이미 아이템이 생성되어 있을 때
    {
        return false;
    }
    else
    {
        Growth growthItem;
        growthCreatedTime = growthItem.time_check();
        srand((unsigned int)time(NULL));

        int y = growthPos.y, x = growthPos.x;
        while (mapCodes[y][x] != 0)
        {
            y = rand() % SIZE_Y;
            x = rand() % SIZE_X;
        }
        growthPos = Pos(x, y);
        mapStatus[y][x] = growthItem;
        mapCodes[y][x] = GROWTH_CODE;
        isGrowthCreated = true;

        return true;
    }
}
/*Growth 아이템이 생성되어 있는 상태에서 현재 시간을 측정,
  만약 snake head가 Growth 아이템을 접촉하지 않았고 Growth 아이템 생성 시간과 현재 시간이 5초 차이가 날 때 게임 맵에서 아이템을 삭제한 뒤 아이템 생성 여부를 false로 표시한 후, true 반환
  만약 snake head가 Growth 아이템을 접촉했을 때 즉시 게임 맵에서 아이템을 삭제한 뒤 아이템 생성 여부를 false로 표시한 후, true 반환
  두 조건에 해당되지 않으면 false를 반환*/
bool Manager::removeGrowth(int check)
{
    time_t end = time(NULL);
    int y = growthPos.y, x = growthPos.x;

    if ((check == 0) && isGrowthCreated && (end - growthCreatedTime > GROWTH_DURATION)) //snake head가 Growth 아이템을 접촉하지 않았고 Growth 아이템 생성 시간과 현재 시간이 5초 차이가 날 때
    {
        mapStatus[y][x] = Empty();
        mapCodes[y][x] = 0;
        isGrowthCreated = false;
        growthPos = Pos(0, 0);
        return true;
    }
    else if (check == 1) //snake head가 Growth 아이템을 접촉했을 때
    {
        mapStatus[y][x] = Empty();
        mapCodes[y][x] = 0;
        isGrowthCreated = false;
        growthPos = Pos(0, 0);
        return true;
    }
    else
        return false;
}
/*이미 Poison 아이템이 생성되어 있으면 fasle 반환
  아이템이 생성되어 있지 않으면 아이템 생성 및 생성 시간 기록, 벽을 제외한 빈공간 중 랜덤한 좌표를 아이템 좌표로 설정
  아이템이 생성되어 있다고 표시 후 true 반환*/
bool Manager::createPoison()
{
    if (isPoisonCreated) //이미 아이템이 생성되어 있을 때
    {
        return false;
    }
    else
    {
        Poison poisonItem;
        poisonCreatedTime = poisonItem.time_check();
        srand((unsigned int)time(NULL));

        int y = poisonPos.y, x = poisonPos.x;
        while (mapCodes[y][x] != 0)
        {
            y = rand() % SIZE_Y;
            x = rand() % SIZE_X;
        }
        poisonPos = Pos(x, y);
        mapStatus[y][x] = poisonItem;
        mapCodes[y][x] = POISON_CODE;
        isPoisonCreated = true;

        return true;
    }
}
/*Poison 아이템이 생성되어 있는 상태에서 현재 시간을 측정,
  만약 snake head가 Poison 아이템을 접촉하지 않았고 Poison 아이템 생성 시간과 현재 시간이 5초 차이가 날 때 게임 맵에서 아이템을 삭제한 뒤 아이템 생성 여부를 false로 표시한 후, true 반환
  만약 snake head가 Poison 아이템을 접촉했을 때 즉시 게임 맵에서 아이템을 삭제한 뒤 아이템 생성 여부를 false로 표시한 후, true 반환
  두 조건에 해당되지 않으면 false를 반환*/
bool Manager::removePoison(int check)
{
    time_t end = time(NULL);
    int y = poisonPos.y, x = poisonPos.x;

    if ((check == 0) && isPoisonCreated && (end - poisonCreatedTime > POISON_DURATION)) //snake head가 Poison 아이템을 접촉하지 않았고 Poison 아이템 생성 시간과 현재 시간이 5초 차이가 날 때
    {
        mapStatus[y][x] = Empty();
        mapCodes[y][x] = 0;
        isPoisonCreated = false;
        poisonPos = Pos(0, 0);
        return true;
    }
    else if (check == 1) //snake head가 Poison 아이템을 접촉했을 때
    {
        mapStatus[y][x] = Empty();
        mapCodes[y][x] = 0;
        isPoisonCreated = false;
        poisonPos = Pos(0, 0);
        return true;
    }
    else
        return false;
}
/*snake head의 currentpos값과 Growth / Poison 아이템 객체의 위치 좌표값을 비교
  만약 snake head가 Growth 아이템을 접촉했을 때 snake의 몸 길이를 늘리는 lengthen()함수 소환, snake의 Growth 아이템 이용 횟수에 +1, Growth 아이템 제거 및 true 반환
  만약 snake head가 Poison 아이템을 접촉했을 때 snake의 몸 길이를 줄이는 shorten()함수 소환, 몸 길이를 줄인 결과 길이가 3이상일 때 snake의 Poison 아이템 이용 횟수에 +1, Poison 아이템 제거 및 true 반환
  몸 길이를 줄인 결과 길이가 2이하일 때 snake의 Poison 아이템 이용 횟수에 +1, Poison 아이템 제거 및 false 반환 (useItem()반환 결과 false일 경우, 게임 오버)
  아무런 조건에 해당되지 않아도 true 반환*/
bool Manager::useItem(Snake &s)
{
    Body snakeHead = *(s).getHead();
    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++)
        {
            if ((mapCodes[i][j] == GROWTH_CODE) && snakeHead.getPos() == Pos(j + 1, i + 1)) //snake head가 Growth 아이템을 접촉했을 때
            {
                s.lengthen(); //snake 몸 길이 늘리기
                int growthCnt = s.getGrowthCnt();
                s.setGrowhCnt(growthCnt + 1); //snake의 Growth 아이템 이용 횟수에 +1
                removeGrowth(1);
                return true;
            }
            else if ((mapCodes[i][j] == POISON_CODE) && snakeHead.getPos() == Pos(j + 1, i + 1)) //snake head가 Poison 아이템을 접촉했을 때
            {
                if (s.shorten()) //snake의 몸 길이를 줄였을 때 snake 몸 길이가 3이상일 때
                {
                    int poisonCnt = s.getPoisonCnt();
                    s.setPoisonCnt(poisonCnt + 1);
                    removePoison(1);
                    return true;
                }
                else ////snake의 몸 길이를 줄였을 때 snake 몸 길이가 2이하일 때
                {
                    int poisonCnt = s.getPoisonCnt();
                    s.setPoisonCnt(poisonCnt + 1);
                    removePoison(1);
                    return false;
                }
            }
        }
    }
    return true;
}

// items finished =================================================
// gate begins ====================================================
bool Manager::isGateActivated()
{
    return gateActivationLeft > 0;
}

bool Manager::createGate(Snake &s)
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
        int entranceX, entranceY;
        while (true)
        {
            entranceX = rand() % SIZE_X;
            entranceY = rand() % SIZE_Y;
            if (mapCodes[entranceY][entranceX] == WALL_CODE)
                break;
        }
        gateEntrancePos = Pos(entranceX, entranceY);

        // 나오는 게이트
        int exitX, exitY;
        while (true)
        {
            exitX = rand() % SIZE_X;
            exitY = rand() % SIZE_Y;
            gateExitPos = Pos(exitX, exitY);

            if ((mapCodes[exitY][exitX] == WALL_CODE) && (gateEntrancePos != gateExitPos))
                break;
        }

        mapStatus[entranceY][entranceX] = Gate();
        mapStatus[exitY][exitX] = Gate();
        mapCodes[entranceY][entranceX] = GATE_CODE;
        mapCodes[exitY][exitX] = GATE_CODE;

        return true;
    }
}

void Manager::updateGateActivationLeft()
{
    gateActivationLeft -= 1;
}

void Manager::removeGate()
{
    if (isGateCreated && (!isGateActivated()) && (time(NULL) - gateGenerated > GATE_DURATION_SEC))
    {
        int entranceY = gateEntrancePos.y, entranceX = gateEntrancePos.x;
        int exitY = gateExitPos.y, exitX = gateExitPos.x;

        // 맵에 기록되어 있는 게이트 정보 삭제
        mapCodes[entranceY][entranceX] = WALL_CODE;
        mapStatus[entranceY][entranceX] = Wall();
        mapCodes[exitY][exitX] = WALL_CODE;
        mapStatus[exitY][exitX] = Wall();

        // 활성화 표시 해제
        isGateCreated = false;
    }
}

void Manager::turnOnGate(Snake &s)
{
    Body head = *(s.getHead());
    Pos headPos = head.getPos();

    // gateAtivationLeft 기록을 시작함(snake의 몸 길이만큼 게이트 활성화를 유지함)
    // turnOnGate -> 해당 변수 감소 순서대로 수행되고, 머리의 위치를 미리 옮겨 놓으므로 몸 길이보다 1 작게 초기화 해주는 것이 맞음.
    gateActivationLeft = s.getLength() - 1;

    // 만약 head가 들어가는 게이트가 이전에 설정해놓은 진입 게이트가 아니라면,
    // 진입 게이트와 출구 게이트를 서로 바꿔 현재 접촉한 게이트가 진입 게이트임을 보장한다. (swap)
    int entranceY = gateEntrancePos.y, entranceX = gateEntrancePos.x;
    int exitY = gateExitPos.y, exitX = gateExitPos.x;

    if (headPos != Pos(entranceX + 1, entranceY + 1))
    {
        int tmp = entranceX;
        entranceX = exitX;
        exitX = tmp;

        tmp = entranceY;
        entranceY = exitY;
        exitY = tmp;

        Pos tmpPos = gateEntrancePos;
        gateEntrancePos = gateExitPos;
        gateExitPos = tmpPos;
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
        int exitY = gateExitPos.y, exitX = gateExitPos.x;
        int outX = exitX + (symbolExitGateAtEdge != 'R' ? 1 : 0) + (symbolExitGateAtEdge == 'L' ? 1 : 0);
        int outY = exitY + (symbolExitGateAtEdge != 'D' ? 1 : 0) + (symbolExitGateAtEdge == 'U' ? 1 : 0);

        (*(s.getHead())).setNextPos(Pos(outX, outY));
    }
    // 경우 2. 출구 게이트가 가장자리에 있지 않을 경우
    else
    {
        // 원래 진출하려던 방향의 상대좌표
        int directionX = s.getlastdirection().getXDirection();
        int directionY = s.getlastdirection().getYDirection();

        // 규칙 1. 원래 진출하려던 방향이 뚫려 있는 경우 그대로 진출
        if (mapCodes[exitY + directionY][exitX + directionX] == EMPTY_CODE)
        {
            // 실제 위치 정보와 nCurses를 통해 보여지는 화면이 다른 것을 보정하기 위한 코드
            int outX = exitX + directionX + 1;
            int outY = exitY + directionY + 1;

            (*(s.getHead())).setNextPos(Pos(outX, outY));
        }
        // 규칙 2. 원래 진출하려던 방향이 막혀 있는 경우
        else
        {
            // 빠져 나올 좌표
            int outX = exitX, outY = exitY;
            // 실제 위치 정보와 nCurses를 통해 보여지는 화면이 다른 것을 보정하기 위한 값
            int dx, dy;

            // 1. 시계 방향 시도
            Direction clockwiseDir = s.getlastdirection().getClockwise();
            int clockwiseX = clockwiseDir.getXDirection();
            int clockwiseY = clockwiseDir.getYDirection();

            // 2. 반시계 방향 시도
            Direction counterClockwiseDir = s.getlastdirection().getCounterClockwise();
            int counterX = counterClockwiseDir.getXDirection();
            int counterY = counterClockwiseDir.getYDirection();

            // 3. 반대 방향 시도
            Direction oppositeDir = Direction::getOppositeDirection(s.getlastdirection().getSymbol());
            int oppositeX = oppositeDir.getXDirection();
            int oppositeY = oppositeDir.getYDirection();

            Direction newDirection = Direction(0, 0);

            if (mapCodes[exitY + clockwiseY][exitX + clockwiseX] == EMPTY_CODE)
            {
                dx = clockwiseX;
                dy = clockwiseY;
                newDirection = clockwiseDir;
            }
            // 2. 반시계 방향 시도
            else if (mapCodes[exitY + counterY][exitX + counterX] == EMPTY_CODE)
            {
                dx = counterX;
                dy = counterY;
                newDirection = counterClockwiseDir;
            }
            // 3. 반대 방향 시도
            else if (mapCodes[exitY + oppositeY][exitX + oppositeX] == EMPTY_CODE)
            {
                dx = oppositeX;
                dy = oppositeY;
                newDirection = oppositeDir;
            }

            // 위치 보정
            outX += dx + 1;
            outY += dy + 1;

            (*(s.getHead())).setNextPos(Pos(outX, outY));
            s.setLastDirection(newDirection);
        }
    }
}

bool Manager::isHeadAtGate(Snake &s)
{
    Body head = *(s).getHead();
    Pos headPos = head.getPos();

    int entranceY = gateEntrancePos.y, entranceX = gateEntrancePos.x;
    int exitY = gateExitPos.y, exitX = gateExitPos.x;

    return ((headPos == Pos(exitX + 1, exitY + 1)) || (headPos == Pos(entranceX + 1, entranceY + 1)));
}

bool Manager::isTailAtEntranceGate(Snake &s)
{
    Body tail = s.getLastBody();
    Pos tailPos = tail.getPos();

    int entranceY = gateEntrancePos.y, entranceX = gateEntrancePos.x;

    return (tailPos == Pos(entranceX + 1, entranceY + 1));
}

char Manager::isExitGateAtEdge()
{
    int exitY = gateExitPos.y, exitX = gateExitPos.x;

    // 왼쪽 벽인 경우
    if (exitX == 0)
        return 'L';
    else if (exitX == (SIZE_X - 1))
        return 'R';
    else if (exitY == 0)
        return 'U';
    else if (exitY == (SIZE_Y - 1))
        return 'D';
    else
        return -1;
}

// gate finished ==================================================
