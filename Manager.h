/* !!! Gate, Item쪽 구현은 Snake쪽 틀이 좀 잡힌 다음 구체화하는게 좋을 것 같음. 변경 가능성 높음. !!!*/

/* 맵과 Snake의 정보를 저장할 매니저 클래스 */
class Manager
{
private:
    // 맵의 X-Y 방향 크기
    int SIZE;
    // 각 요소(Wall, Snake Head, ...) 등을 저장할 2차원 배열
    char **map;
    // Snake를 가리키고 있음 -> 매 스테이지가 끝날 때마다 새로 갱신해주어야 함
    Snake snake;

    // 현재 아이템이 생성되어 있는지 여부를 기록하는 변수
    bool isGrowthCreated;
    bool isPoisonCreated;
    // 생성된 아이템의 위치를 기록하는 변수
    // 안정성을 위해 아이템이 생성되어 있지 않을 때에는 -1로 할당해 놓을 것!
    int growthX, growthY;
    int poisonX, poisonY;

    // 현재 게이트가 생성되어 있는지 여부를 기록하는 변수
    bool isGateCreated;
    // 게이트가 열려 있는지(뱀이 통과하는 중인지) 기록하는 변수
    bool isGateOpened;
    // 들어가는 게이트와 나오는 게이트의 좌표
    // 안정성을 위해 아이템이 생성되어 있지 않을 때에는 -1로 할당해 놓을 것!
    int gateEntranceX, gateEntranceY;
    int gateExitX, gateExitY;

public:
    // filename을 입력받아 맵을 초기화함
    Manager(string filename, Snake &snake) : snake(snake)
    {
        /* !!! 생성자의 맨 앞에서 파일로부터 먼저 정보를 읽어와야 함 !!! */
        // 지금은 기본값 20으로 그냥 설정
        SIZE = 20;

        // 맵 정보를 담을 2차원 배열을 동적 할당함
        map = new char *[SIZE];
        for (int i = 0; i < SIZE; i++)
            map[i] = new char[SIZE];

        // 아이템 관련 변수 초기화
        isGrowthCreated = false;
        isPoisonCreated = false;
        growthX = growthY = -1;
        poisonX = poisonY = -1;

        // 게이트 관련 변수 초기화
        isGateCreated = false;
        isGateOpened = false;
        gateEntranceX = gateEntranceY = -1;
        gateExitX = gateExitY = -1;
    }

    ~Manager()
    {
        // 맵 정보를 담은 동적 배열 해제
        for (int i = 0; i < SIZE; i++)
            delete[] map[i];
        delete[] map;
    }

    /* Growth를 생성하는 메서드 
       Growth가 생성되어 있다면 Growth를 생성하지 않고 false를 반환한다. 
       Growth가 생성되어 있지 않다면 Growth를 생성하고 true를 반환한다. */
    bool createGrowth();

    /* Growth를 삭제하는 메서드 
    Growth가 생성되어 있다면 Growth 자리를 nullptr로 두고 좌표를 -1로 변경한 뒤 true를 반환한다. 
    Growth가 생성되어 있지 않다면 false를 반환한다. */
    bool removeGrowth();

    /* Poison을 생성하는 메서드 
       Poison이 생성되어 있다면 Poison을 생성하지 않고 false를 반환한다. 
       Poison이 생성되어 있지 않다면 Poison를 생성하고 true를 반환한다. */
    bool createPoison();

    /* Poison을 삭제하는 메서드 
    Poison이 생성되어 있다면 Poison 자리를 nullptr로 두고 좌표를 -1로 변경한 뒤 true를 반환한다. 
    Poison이 생성되어 있지 않다면 false를 반환한다. */
    bool removePoison();

    /* 게이트를 생성하는 메서드 
       게이트가 생성되어 있거나 게이트를 통과하고 있는 경우 false를 반환하고,
       게이트가 생성되어 있지 않은 경우 게이트를 생성한 후 true를 반환함*/
    bool createGate();

    /* 게이트를 삭제하는 메서드
       게이트가 생성되어 있거나 게이트를 통과하고 있는 경우 false를 반환하고,
       게이트가 생성되어 있지 않은 경우 게이트를 생성한 후 true를 반환함*/
    bool removeGate();

    /* Snake Head가 아이템의 좌표와 일치하는지 확인하는 메서드 */
    bool isAtItem();

    /* Snake Head가 게이트 좌표와 일치하는지 확인하는 메서드 */
    bool isAtGate();
};