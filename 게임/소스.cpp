﻿#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<conio.h>
#include<Windows.h>
#include<string.h>
#include<string>

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
#define MAZE_WIDTH 15
#define MAZE_HEIGHT 14
#define BATTLE_UI_WIDTH 31
#define BATTLE_UI_HEIGHT 40
#define INFO_UI_WIDTH 21
#define INFO_UI_HEIGHT 8
#define SELECT_UI_WIDTH 11
#define SELECT_UI_HEIGHT 7
#define DIALOS_DISTANCE 2
#define PLAYER_MAX_HP 100
#define PLAYER_MAX_MP 10
#define PLAYER_DAMAGE 5
#define MONSTER_MAX_HP 20
#define MONSTER_DAMAGE 5
#define BOSS_MAX_HP 100
#define BOSS_DAMAGE 13
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
    MONSTER,
    BOSS,
    SECRET_DOOR,
    ESCAPE
};

enum Square
{
    L_TOP = '7',
    R_TOP,
    L_BOTTOM,
    R_BOTTOM,
    WIDTH,
    LENGTH
};

enum PlayerAction
{
    ATTACK,
    SKILL,
    RUN,
    CHOICES
};

enum MainTitle
{
    START,
    END
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
    const char* shape;
};

struct Enemy
{
    int maxHP;
    int hp;
    int damage;
};

//미로 맵 데이터
char maze[MAZE_HEIGHT][MAZE_WIDTH];
char front_maze[MAZE_HEIGHT][MAZE_WIDTH];
char buf[256];
bool gameClear = false;
bool gameStart = false;
bool bossLive = true;
bool battle = false;

Player player;
Enemy enemy;
int item[2];
int secretDoor[2];
int escape[2];


void gotoxy(int x, int y)
{
    //x, y 좌표 설정
    COORD position = { x,y };

    //커서 이동
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void CreateMaze()
{
    //0 : 빈 공간  ("  ")
    //1 : 벽       ("■")
    //2 : 아이템   ("♥")
    //3 : 몬스터   ("㉾")
    //4 : 보스     ("㈜")
    //5 : 비밀문   ("□")
    //6 : 탈출구   ("◎")

    strcpy(maze[0],  "11111111111111");
    strcpy(maze[1],  "10001111121021");
    strcpy(maze[2],  "11100030103001");
    strcpy(maze[3],  "12101010121031");
    strcpy(maze[4],  "10101010111101");
    strcpy(maze[5],  "10101030000001");
    strcpy(maze[6],  "10301011111131");
    strcpy(maze[7],  "11111011141101");
    strcpy(maze[8],  "11111011101101");
    strcpy(maze[9],  "11111000001121");
    strcpy(maze[10], "11111011111101");
    strcpy(maze[11], "10001011111101");
    strcpy(maze[12], "16005000000301");
    strcpy(maze[13], "11111111111111");
}

void Init()
{
    player = { 2,1,PLAYER_MAX_HP,PLAYER_MAX_HP,
PLAYER_MAX_MP,PLAYER_MAX_MP,PLAYER_DAMAGE,"♠" };

    CreateMaze();
    memset(front_maze, 0, sizeof(front_maze));

    gameStart = true;
    gameClear = false;
    bossLive = true;
}

void ErasePoint(int x, int y)
{
    gotoxy(x * 2, y);
    printf("  ");
    gotoxy(-1, -1);
}

void DrawPoint(const char* c, int x, int y)
{
    gotoxy(x * 2, y);
    printf("%s", c);
    gotoxy(-1, -1);
}

void MazeDraw(int x, int y, char c)
{
    gotoxy(x, y);
    switch (c)
    {
    case EMPTY:
        printf("  ");
        break;
    case WALL:
        GRAY
            printf("■");
        ORIGINAL
            break;
    case ITEM:
        GOLD
            printf("♥");
        ORIGINAL
            break;
    case MONSTER:
        BLOOD
            printf("♣");
        ORIGINAL
            break;
    case BOSS:
        BLOOD
            printf("㈜");
        ORIGINAL
            break;
    case SECRET_DOOR:
        YELLOW
            printf("□");
        ORIGINAL
            break;
    case ESCAPE:
        BLUE
            printf("◎");
        ORIGINAL
            break;
    case L_TOP:
        printf("┌");
        break;
    case L_BOTTOM:
        printf("└");
        break;
    case R_TOP:
        printf("┐");
        break;
    case R_BOTTOM:
        printf("┘");
        break;
    case WIDTH:
        printf("─");
        break;
    case LENGTH:
        printf("│");
        break;
    }
    gotoxy(MAZE_WIDTH, MAZE_HEIGHT);
}

void DrawAll()
{
    for (int i = 0; i < MAZE_HEIGHT; ++i)
    {
        for (int j1 = 0, j2 = 0; j1 < MAZE_WIDTH; j1++)
        {

            MazeDraw(j2, i, maze[i][j1]);

            j2 += 2;
        }
    }
}

void MazeBuffer()
{
    static Player front_player = player;

    for (int i = 0; i < MAZE_HEIGHT; ++i)
    {
        for (int j1 = 0, j2 = 0; j1 < MAZE_WIDTH; j1++)
        {
            if (front_maze[i][j1] != maze[i][j1])
            {
                MazeDraw(j2, i, maze[i][j1]);
                front_maze[i][j1] = maze[i][j1];
            }
            j2 += 2;
        }
    }

    if (front_player.x != player.x || front_player.y != player.y)
    {
        gotoxy(front_player.x, front_player.y);
        printf("%s", "  ");
        front_player = player;
    }

    gotoxy(0, MAZE_HEIGHT + 1);
    printf("HP : %d  ", player.hp);
    printf("MP : %d\n", player.mp);
    printf("Damage : %d ", player.damage);

    gotoxy(player.x, player.y);
    printf("%s", player.shape);
}

void SquareDraw(int minX, int minY, int maxX, int maxY)
{
    gotoxy(minX, minY);

    for (int i = minY; i < maxY; i++)
    {
        for (int j = minX, j2 = minX; j < maxX - 1; j++)
        {
            if (i == minY && j == minX)
                MazeDraw(j2, i, L_TOP);
            else if (i == minY && j == maxX - 2)
                MazeDraw(j2, i, R_TOP);
            else if (i == maxY - 1 && j == minX)
                MazeDraw(j2, i, L_BOTTOM);
            else if (i == maxY - 1 && j == maxX - 2)
                MazeDraw(j2, i, R_BOTTOM);
            else if ((i != minY || i != maxY - 1) &&
                (j == minX || j == maxX - 2))
                MazeDraw(j2, i, LENGTH);
            else if ((i == minY || i == maxY - 1) &&
                (j != minX || j != maxX - 2))
                MazeDraw(j2, i, WIDTH);
            j2 += 2;
        }
    }
}

void MonsterDraw(char monster,int y)
{
    FILE* readfile = NULL;
    if (monster == MONSTER)
        readfile = fopen("alien.TXT", "r");
    else if (monster == BOSS)
        readfile = fopen("dragon.TXT", "r");
    else if(monster==TITLE)
        readfile = fopen("title.TXT", "r"); 

    char buffer[100000] = { 0, };

    if (readfile != NULL)
    {
        gotoxy(0, y);
        fread(buffer, 1, 100000, readfile);
        printf("%s", buffer);
    }

    fclose(readfile);
}

int SelectCursor(int dialogs, int distance, int x, int y)
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
                if (Select - 1 >= 0)
                {
                    ErasePoint(x, y);
                    y -= distance;
                    Select--;
                }
                break;
            case DOWN:
                if (Select + 1 < dialogs)
                {
                    ErasePoint(x, y);
                    y += distance;
                    Select++;
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

void Attack()
{
    enemy.hp -= player.damage;

    if (enemy.hp > 0)
        player.hp -= enemy.damage;
}

void Skill()
{
    enemy.hp -= player.damage * 2;
    player.mp -= 5;
}

void BattleInfoDraw()
{
    DrawPoint("플레이어 정보", 5, 2);
    gotoxy(5 * 2, 4);
    printf("              ", player.maxHP, player.hp);
    gotoxy(5 * 2, 4);
    printf("HP : %d/%d", player.maxHP, player.hp);
    gotoxy(5 * 2, 5);
    printf("              ", player.maxMP, player.mp);
    gotoxy(5 * 2, 5);
    printf("MP : %d/%d", player.maxMP, player.mp);
    gotoxy(5 * 2, 6);
    printf("              ", player.damage);
    gotoxy(5 * 2, 6);
    printf("Damage : %d", player.damage);

    DrawPoint("몬스터 정보", 19, 2);
    gotoxy(19 * 2, 4);
    printf("              ", enemy.maxHP, enemy.hp);
    gotoxy(19 * 2, 4);
    printf("HP : %d/%d", enemy.maxHP, enemy.hp);
    gotoxy(19 * 2, 5);
    printf("              ", enemy.damage);
    gotoxy(19 * 2, 5);
    printf("Damage : %d", enemy.damage);
}

bool Battle(char monster)
{
    sprintf(buf, "mode con: lines=%d cols=%d", BATTLE_UI_HEIGHT, BATTLE_UI_WIDTH * 2);
    system(buf);
    system("cls");
    MonsterDraw(monster,10);
    SquareDraw(0, 0, BATTLE_UI_WIDTH, BATTLE_UI_HEIGHT); //전체 창
    SquareDraw(3*2, 1, INFO_UI_WIDTH+10, INFO_UI_HEIGHT); //플레이어와 몬스터 정보 창
    SquareDraw(10*2, 32, SELECT_UI_WIDTH+20, SELECT_UI_HEIGHT+32); //플레이어 선택지 창
    DrawPoint("공격!!", 13, 33);
    DrawPoint("스킬 발동!!", 13, 35);
    DrawPoint("도망쳐!!", 13, 37);

    battle = true;

    if (monster == MONSTER)
        enemy = { MONSTER_MAX_HP, MONSTER_MAX_HP,MONSTER_DAMAGE };
    else
        enemy = { BOSS_MAX_HP, BOSS_MAX_HP,BOSS_DAMAGE };

    while (true)
    {
        BattleInfoDraw();

        switch (SelectCursor(CHOICES, DIALOS_DISTANCE, 11, 33))
        {
        case ATTACK:
            Attack();
            break;
        case SKILL:
            if (player.mp > 0)
                Skill();
            break;
        case RUN:
            system("cls");
            DrawAll();
            return 0;
        }

        if (enemy.hp <= 0)
        {
            if (monster == BOSS)
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

    battle = false;
    system("cls");
    DrawAll();

    return 1;
}

void GetItem()
{
    if (player.hp + 20 < player.maxHP)
        player.hp += 20;
    else
        player.hp = player.maxHP;

    if (player.mp + 5 < player.maxMP)
        player.mp += 5;
    else
        player.mp = player.maxMP;

    player.damage += 1;
}

bool MoveCheck(int x,int y,char object)
{
    switch (object)
    {
    case WALL:
        return 1;
    case ITEM:
        GetItem();
        maze[player.y + y][player.x / 2 + x] = EMPTY;
        return 0;
    case MONSTER:
    case BOSS:
        if (Battle(object))
        {
            maze[player.y + y][player.x / 2 + x] = EMPTY;
            return 0;
        }
        
        return 1;
    case SECRET_DOOR:
        if (bossLive)
            return 1;

        maze[player.y + y][player.x / 2 + x] = EMPTY;
        return 0;
    case ESCAPE:
        gameClear = true;
        return 0;
    }
    return 0;
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
            if (MoveCheck(0, -1, maze[player.y - 1][player.x / 2]))
                break;

            player.y--;
            break;
        case DOWN:
            if (MoveCheck(0, 1, maze[player.y + 1][player.x / 2]))
                break;

            player.y++;
            break;
        case LEFT:
            if (MoveCheck(-1, 0, maze[player.y][player.x / 2 - 1]))
                break;

            player.x -= 2;
            break;
        case RIGHT:
            if (MoveCheck(1, 0, maze[player.y][player.x / 2 + 1]))
                break;

            player.x += 2;
            break;
        }
        sprintf(buf, "mode con: lines=%d cols=%d", MAZE_HEIGHT, MAZE_WIDTH * 2);
    }
}

int main()
{
    
    sprintf(buf, "mode con: lines=%d cols=%d", TITLE_HEIGHT, TITLE_WIDTH * 2);
    system(buf);

    while (true)
    {
        MonsterDraw(TITLE,0);
        DrawPoint("던전 RPG", TITLE_WIDTH-20 + 10, 4);
        DrawPoint("게임 시작", TITLE_WIDTH/2+10, TITLE_HEIGHT/2+8);
        DrawPoint("게임 종료", TITLE_WIDTH/2+10, TITLE_HEIGHT/2+11);
        switch (SelectCursor(END+1,3,
            TITLE_WIDTH / 2 + 8, TITLE_HEIGHT / 2 + 8))
        {
        case START:
            sprintf(buf, "mode con: lines=%d cols=%d", MAZE_HEIGHT + 4, MAZE_WIDTH * 2);
            system(buf);

            Init();
            
            while (true)
            {
                Keyboard();
                MazeBuffer();

                if (gameStart == false || gameClear == true)
                    break;

                Sleep(100);
            }
            if (gameClear)
            {
                gotoxy(MAZE_WIDTH / 2 - 3, MAZE_HEIGHT / 2);
                printf("게임 클리어!!");
            }

            if (!gameStart)
            {
                gotoxy(MAZE_WIDTH / 2 - 3, MAZE_HEIGHT / 2);
                printf("게임 오버 :( ");
            }
            _getch();
            sprintf(buf, "mode con: lines=%d cols=%d", TITLE_HEIGHT, TITLE_WIDTH * 2);
            system(buf);
            break;
        case END:
            return 0;
        }
    }
	return 0;
}