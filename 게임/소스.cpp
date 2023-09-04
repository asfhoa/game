#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<conio.h>
#include<Windows.h>
#include<string.h>
#include<string>
#include<time.h>

#define col GetStdHandle(STD_OUTPUT_HANDLE) 
#define BLOOD SetConsoleTextAttribute( col,0x0004 );
#define ORIGINAL SetConsoleTextAttribute( col,0x0007 );
#define BLUE SetConsoleTextAttribute( col,0x0009 );
#define YELLOW SetConsoleTextAttribute( col,0x000e );
#define GOLD SetConsoleTextAttribute( col,0x0006 );	
#define GRAY SetConsoleTextAttribute( col,0x0008 );
#define RED SetConsoleTextAttribute( col,0x000c );

#define TITLE_WIDTH 35
#define TITLE_HEIGHT 35
#define DUNGEON_WIDTH 15
#define DUNGEON_HEIGHT 14
#define BATTLE_UI_WIDTH 31
#define BATTLE_UI_HEIGHT 40
#define INFO_UI_WIDTH 21
#define INFO_UI_HEIGHT 9
#define SELECT_UI_WIDTH 11
#define SELECT_UI_HEIGHT 7
#define TITLE 11

enum KeyInput
{
    UP = 72,
    LEFT = 75,
    RIGHT = 77,
    DOWN = 80,
    SPACEBAR = 32
};

enum Object
{
    EMPTY = '0',
    WALL,
    ITEM,
    ALIEN,
    DINOSAUR,
    TUX,
    BOSS,
    SECRET_DOOR,
    ESCAPE
};

enum Square
{
    L_TOP = 'A',
    R_TOP,
    L_BOTTOM,
    R_BOTTOM,
    WIDTH,
    LENGTH
};

enum PlayerAction
{
    ATTACK,
    RUN,
    SKILL,
    USE_ITEM,
    CHOICES
};

enum MainTitle
{
    START,
    END
};

enum MonsterInfo
{
    ALIEN_MAX_HP = 20,
    ALIEN_DAMAGE = 5,
    ALIEN_SPEED = 5,
    DINOSAUR_MAX_HP = 40,
    DINOSAUR_DAMAGE = 2,
    DINOSAUR_SPEED = 1,
    TUX_MAX_HP = 15,
    TUX_DAMAGE = 8,
    TUX_SPEED = 15,
    BOSS_MAX_HP = 100,
    BOSS_DAMAGE = 18,
    DRAGON_SPEED = 12
};

enum AttackTurn
{
    PLAYER_TURN,
    MONSTER_TURN
};

enum PlayerInfo
{
    PLAYER_MAX_HP = 100,
    PLAYER_MAX_MP = 10,
    PLAYER_DAMAGE = 5,
    PLAYER_SPEED = 5
};

struct Player
{
    int x;
    int y;
    int maxHP;
    int hp;
    int maxMP;
    int mp;
    int damage;
    int speed;
    const char* shape;
};

struct Monster
{
    int maxHP;
    int hp;
    int damage;
    int speed;
};

//미로 맵 데이터
char dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH];
char front_maze[DUNGEON_HEIGHT][DUNGEON_WIDTH];
char buf[256];
bool gameClear = false;
bool gameStart = false;
bool bossLive = true;
bool battle = false;

Player player;
Player front_player;
Monster monster;
int itemCount;
char s[20];


void gotoxy(int x, int y)
{
    //x, y 좌표 설정
    COORD position = { x,y };

    //커서 이동
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void ChangeCon(int width,int height)
{
    sprintf(buf, "mode con: lines=%d cols=%d", height, width * 2);
    system(buf);
    system("cls");
}

void CreateMaze()
{
    //0 : 빈 공간  ("  ")
    //1 : 벽       ("■")
    //2 : 아이템   ("♥")
    //3 : 몬스터1  ("♣")
    //4 : 몬스터2  ("★")
    //5 : 몬스터3  ("●")
    //6 : 보스     ("㈜")
    //7 : 비밀문   ("□")
    //8 : 탈출구   ("◎")

    strcpy(dungeon[0],  "11111111111111");
    strcpy(dungeon[1],  "10001111121021");
    strcpy(dungeon[2],  "11100000105001");
    strcpy(dungeon[3],  "12101010121001");
    strcpy(dungeon[4],  "10101010111101");
    strcpy(dungeon[5],  "10101000000001");
    strcpy(dungeon[6],  "10301011111101");
    strcpy(dungeon[7],  "11111011161101");
    strcpy(dungeon[8],  "11111011101101");
    strcpy(dungeon[9],  "11111040001121");
    strcpy(dungeon[10], "11111011111101");
    strcpy(dungeon[11], "10001011111101");
    strcpy(dungeon[12], "18007000000001");
    strcpy(dungeon[13], "11111111111111");

    memset(front_maze, 0, sizeof(front_maze));
}

void Init()
{
    player = { 2,1,PLAYER_MAX_HP,PLAYER_MAX_HP,
PLAYER_MAX_MP,PLAYER_MAX_MP,PLAYER_DAMAGE,PLAYER_SPEED,"♠" };
    front_player = { 0,0,0,0,0,0,0,0," " };

    CreateMaze();

    gameStart = true;
    gameClear = false;
    bossLive = true;

    itemCount = 0;
}

void ErasePoint(int x, int y)
{
    gotoxy(x * 2, y);
    printf("  ");
}

void DrawPoint(const char* c, int x, int y)
{
    gotoxy(x * 2, y);
    printf("%s", c);
}

void TileDraw(int x, int y, char c)
{
    switch (c)
    {
    case WALL:
        GRAY
            DrawPoint("■", x/2, y);
        ORIGINAL
            break;
    case ITEM:
        GOLD
            DrawPoint("♥", x / 2, y);
        ORIGINAL
            break;
    case ALIEN:
        BLOOD
            DrawPoint("♣", x / 2, y);
        ORIGINAL
            break;
    case DINOSAUR:
        BLOOD
            DrawPoint("★", x / 2, y);
        ORIGINAL
            break;
    case TUX:
        BLOOD
            DrawPoint("●", x / 2, y);
        ORIGINAL
            break;
    case BOSS:
        BLOOD
            DrawPoint("㈜", x / 2, y);
        ORIGINAL
            break;
    case SECRET_DOOR:
        YELLOW
            DrawPoint("□", x / 2, y);
        ORIGINAL
            break;
    case ESCAPE:
        BLUE
            DrawPoint("◎", x / 2, y);
        ORIGINAL
            break;
    case L_TOP:
        DrawPoint("┌", x / 2, y);
        break;
    case L_BOTTOM:
        DrawPoint("└", x / 2, y);
        break;
    case R_TOP:
        DrawPoint("┐", x / 2, y);
        break;
    case R_BOTTOM:
        DrawPoint("┘", x / 2, y);
        break;
    case WIDTH:
        DrawPoint("─", x / 2, y);
        break;
    case LENGTH:
        DrawPoint("│", x / 2, y);
        break;
    }
}

void DungeonDraw()
{
    ChangeCon(DUNGEON_WIDTH, DUNGEON_HEIGHT + 4);
    for (int i = 0; i < DUNGEON_HEIGHT; ++i)
    {
        for (int j1 = 0, j2 = 0; j1 < DUNGEON_WIDTH; j1++)
        {
            TileDraw(j2, i, dungeon[i][j1]);
            j2 += 2;
        }
    }
    DrawPoint(player.shape, player.x / 2, player.y);
}

void UpdateDraw()
{
    for (int i = 0; i < DUNGEON_HEIGHT; ++i)
    {
        for (int j1 = 0, j2 = 0; j1 < DUNGEON_WIDTH; j1++)
        {
            if (front_maze[i][j1] != dungeon[i][j1])
            {
                TileDraw(j2, i, dungeon[i][j1]);
                front_maze[i][j1] = dungeon[i][j1];
            }
            j2 += 2;
        }
    }

    if (front_player.x != player.x || front_player.y != player.y)
    {
        ErasePoint(front_player.x/2, front_player.y);
        DrawPoint(player.shape, player.x / 2, player.y);
        front_player = player;
    }

    sprintf(s, "HP : %d", player.hp);
    DrawPoint(s,0, DUNGEON_HEIGHT + 1);
    sprintf(s, "MP : %d\n", player.mp);
    DrawPoint(s, 6, DUNGEON_HEIGHT + 1);
    sprintf(s, "Damage : %d", player.damage);
    DrawPoint(s, 0, DUNGEON_HEIGHT + 2);
}

void SquareDraw(int minX, int minY, int maxX, int maxY)
{
    gotoxy(minX, minY);

    for (int i = minY; i < maxY; i++)
    {
        for (int j = minX, j2 = minX; j < maxX - 1; j++)
        {
            if (i == minY && j == minX)
                TileDraw(j2, i, L_TOP);
            else if (i == minY && j == maxX - 2)
                TileDraw(j2, i, R_TOP);
            else if (i == maxY - 1 && j == minX)
                TileDraw(j2, i, L_BOTTOM);
            else if (i == maxY - 1 && j == maxX - 2)
                TileDraw(j2, i, R_BOTTOM);
            else if ((i != minY || i != maxY - 1) &&
                (j == minX || j == maxX - 2))
                TileDraw(j2, i, LENGTH);
            else if ((i == minY || i == maxY - 1) &&
                (j != minX || j != maxX - 2))
                TileDraw(j2, i, WIDTH);
            j2 += 2;
        }
    }
}

void MonsterDraw(char monsterType, int y)
{
    FILE* readfile = NULL;
    switch (monsterType)
    {
    case TITLE:
        readfile = fopen("title.TXT", "r");
        break;
    case ALIEN:
        readfile = fopen("alien.TXT", "r");
        break;
    case DINOSAUR:
        readfile = fopen("dinosaur.TXT", "r");
        break;
    case TUX:
        readfile = fopen("tux.TXT", "r");
        break;
    case BOSS:
        readfile = fopen("dragon.TXT", "r");
        break;
    }

    char buffer[100000] = { 0, };

    if (readfile != NULL)
    {
        gotoxy(0, y);
        fread(buffer, 1, 100000, readfile);
        printf("%s", buffer);
    }

    fclose(readfile);
}

void CreateMonster(char monsterType)
{
    switch (monsterType)
    {
    case ALIEN:
        monster = { ALIEN_MAX_HP, ALIEN_MAX_HP,ALIEN_DAMAGE,ALIEN_SPEED };
        break;
    case DINOSAUR:
        monster = { DINOSAUR_MAX_HP, DINOSAUR_MAX_HP,DINOSAUR_DAMAGE ,DINOSAUR_SPEED };
        break;
    case TUX:
        monster = { TUX_MAX_HP, TUX_MAX_HP,TUX_DAMAGE,TUX_SPEED };
        break;
    case BOSS:
        monster = { BOSS_MAX_HP, BOSS_MAX_HP,BOSS_DAMAGE ,DRAGON_SPEED };
        break;
    }
}

int SelectCursor(int dialogs, int x, int y)
{
    int Select = 0;
    
    RED
        DrawPoint("▷", x, y);
    ORIGINAL
        while (1)
        {
            switch (_getch())
            {
            case UP:
                if (Select - 1 == 0 || Select - 1 == 2)
                {
                    ErasePoint(x, y);
                    y -= 3;
                    Select--;
                }
                break;
            case DOWN:
                if (Select + 1 == 1 || Select + 1 == 3)
                {
                    ErasePoint(x, y);
                    y += 3;
                    Select++;
                }
                break;
            case LEFT:
                if (gameStart && (Select - 2 == 0 || Select - 2 == 1))
                {
                    ErasePoint(x, y);
                    x -= 6;
                    Select -= 2;
                }
                break;
            case RIGHT:
                if (gameStart && (Select + 2 == 2 || Select + 2 == 3))
                {
                    ErasePoint(x, y);
                    x += 6;
                    Select += 2;
                }
                break;
            case SPACEBAR:
                ErasePoint(x, y);
                return Select;
            }
            RED
                DrawPoint("▷", x, y);
            ORIGINAL
        }
}

void Attack(int playerTurn)
{
    if (playerTurn == PLAYER_TURN)
        monster.hp -= player.damage;
    else
        player.hp -= monster.damage;
}

void Skill()
{
    monster.hp -= player.damage * 2;
    player.mp -= 5;
}

void UseItem()
{
    if (player.hp + 20 < player.maxHP)
        player.hp += 20;
    else
        player.hp = player.maxHP;

    if (player.mp + 5 < player.maxMP)
        player.mp += 5;
    else
        player.mp = player.maxMP;
    player.speed += player.speed;
    itemCount--;
}

void GetItem()
{
    itemCount++;
    player.damage += 1;
}

void BattleInfoDraw()
{
    DrawPoint("플레이어 정보", 5, 2);

    sprintf(s, "남은 아이템 : %d", itemCount);
    DrawPoint(s, 5, 3);

    DrawPoint("              ", 5, 4);
    sprintf(s, "HP : %d/%d", player.maxHP, player.hp);
    DrawPoint(s, 5, 4);

    DrawPoint("              ", 5, 5);
    sprintf(s, "MP : %d/%d", player.maxMP, player.mp);
    DrawPoint(s, 5, 5);

    DrawPoint("              ", 5, 6);
    sprintf(s, "Damage : %d", player.damage);
    DrawPoint(s, 5, 6);

    sprintf(s, "Speed : %d", player.speed);
    DrawPoint(s, 5, 7);

    DrawPoint("몬스터 정보", 19, 2);

    DrawPoint("              ", 19, 4);
    sprintf(s, "HP : %d/%d", monster.maxHP, monster.hp);
    DrawPoint(s, 19, 4);

    DrawPoint("              ", 19, 6);
    sprintf(s, "Damage : %d", monster.damage);
    DrawPoint(s, 19, 6);

    sprintf(s, "Speed : %d", monster.speed);
    DrawPoint(s, 19, 7);
}

void BattleUIDraw(char monsterType)
{
    MonsterDraw(monsterType, 10);
    SquareDraw(0, 0, BATTLE_UI_WIDTH, BATTLE_UI_HEIGHT); //전체 창
    SquareDraw(3 * 2, 1, INFO_UI_WIDTH + 10, INFO_UI_HEIGHT); //플레이어와 몬스터 정보 창
    SquareDraw(7 * 2, 31, SELECT_UI_WIDTH + 20, SELECT_UI_HEIGHT + 32); //플레이어 선택지 창
    DrawPoint("공격!!", 10, 33);
    DrawPoint("스킬 발동!!", 16, 33);
    DrawPoint("도망쳐!!", 10, 36);
    DrawPoint("아이템사용!!", 16, 36);
}

bool Battle(char monsterType)
{
    ChangeCon(BATTLE_UI_WIDTH, BATTLE_UI_HEIGHT);
    CreateMonster(monsterType);
    BattleUIDraw(monsterType);

    while (true)
    {
        BattleInfoDraw();

        switch (SelectCursor(CHOICES, 8, 33))
        {
        case ATTACK:
            if (player.speed >= monster.speed)
            {
                Attack(PLAYER_TURN);
                Attack(MONSTER_TURN);
            }
            else
            {
                Attack(PLAYER_TURN);
                Attack(MONSTER_TURN);
            }
            player.speed = PLAYER_SPEED;
            break;
        case RUN:
            Attack(MONSTER_TURN);
            system("cls");
            return 0;
        case SKILL:
            if (player.mp > 0)
            {
                Skill();
                Attack(MONSTER_TURN);
            }
            break;
        case USE_ITEM:
            if (itemCount > 0)
                UseItem();
            break;
        }

        if (monster.hp <= 0)
        {
            if (monsterType == BOSS)
                bossLive = false;
            break;
        }

        if (player.hp <= 0)
        {
            gameStart = false;
            break;
        }

        Sleep(100);
    }

    system("cls");

    return 1;
}

bool MoveCheck(int x,int y,char object)
{
    bool move = true;
    int num;
    switch (object)
    {
    case EMPTY:
        num = rand();
        if (num % 100 <= 10)
        {
            if (Battle((rand() % 3) + 51))
                move = true;
            else
                move = false;
            DungeonDraw();
        }
        else
            move = true;

        break;
    case WALL:
        move = false;
        break;
    case ITEM:
        GetItem();
        dungeon[player.y + y][player.x / 2 + x] = EMPTY;
        move = true;
        break;
    case ALIEN:
    case DINOSAUR:
    case TUX:
    case BOSS:
        if (Battle(object))
        {
            dungeon[player.y + y][player.x / 2 + x] = EMPTY;

            move = true;
        }
        else
            move = false;

        DungeonDraw();
        break;
    case SECRET_DOOR:
        if (bossLive)
            move = false;

        dungeon[player.y + y][player.x / 2 + x] = EMPTY;
        move = true;
        break;
    case ESCAPE:
        gameClear = false;
        move = true;
        break;
    }
    return move;
}

void Keyboard()
{
    char key = 0;

    if (_kbhit())
    {
        key = _getch();

        if (key == -32)
            key = _getch();

        switch (key)
        {
        case UP:
            if (!MoveCheck(0, -1, dungeon[player.y - 1][player.x / 2]))
                break;
            player.y--;
            break;
        case DOWN:
            if (!MoveCheck(0, 1, dungeon[player.y + 1][player.x / 2]))
                break;
            player.y++;
            break;
        case LEFT:
            if (!MoveCheck(-1, 0, dungeon[player.y][player.x / 2 - 1]))
                break;
            player.x -= 2;
            break;
        case RIGHT:
            if (!MoveCheck(1, 0, dungeon[player.y][player.x / 2 + 1]))
                break;
            player.x += 2;
            break;
        }
    }
}

int main()
{
    srand(time(NULL));
    ChangeCon(TITLE_WIDTH, TITLE_HEIGHT);

    while (true)
    {
        MonsterDraw(TITLE,0);
        DrawPoint("던전 RPG", TITLE_WIDTH-20 + 10, 4);
        DrawPoint("게임 시작", TITLE_WIDTH/2+10, TITLE_HEIGHT/2+8);
        DrawPoint("게임 종료", TITLE_WIDTH/2+10, TITLE_HEIGHT/2+11);
        switch (SelectCursor(END+1,
            TITLE_WIDTH / 2 + 8, TITLE_HEIGHT / 2 + 8))
        {
        case START:
            ChangeCon(DUNGEON_WIDTH, DUNGEON_HEIGHT + 4);

            Init();
            
            while (true) //게임중
            {
                Keyboard();
                UpdateDraw();

                if (gameStart == false || gameClear == true)
                    break;

                Sleep(100);
            }

            if (gameClear)
            {
                DrawPoint("게임 클리어!!", DUNGEON_WIDTH / 2 - 3, DUNGEON_HEIGHT / 2);
            }

            if (!gameStart)
            {
                DrawPoint("게임 오버 :( ", DUNGEON_WIDTH / 2 - 3, DUNGEON_HEIGHT / 2);
            }
            _getch();
            ChangeCon(TITLE_WIDTH, TITLE_HEIGHT);
            break;
        case END:
            return 0;
        }
    }
	return 0;
}