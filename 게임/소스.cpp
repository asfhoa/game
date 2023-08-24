#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<conio.h>
#include<Windows.h>
#include<string.h>

#define col GetStdHandle(STD_OUTPUT_HANDLE) 
#define BLOOD SetConsoleTextAttribute( col,0x0004 );
#define ORIGINAL SetConsoleTextAttribute( col,0x0007 );
#define BLUE SetConsoleTextAttribute( col,0x0009 );
#define YELLOW SetConsoleTextAttribute( col,0x000e );
#define GOLD SetConsoleTextAttribute( col,0x0006 );	
#define GRAY SetConsoleTextAttribute( col,0x0008 );

#define MAZE_WIDTH 15
#define MEZE_HEIGHT 14
#define BATTLE_UI_WIDTH 31
#define BATTLE_UI_HEIGHT 30
#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

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

struct Player
{
    int x;
    int y;
    int hp;
    int mp;
    int damage;
    const char* shape;
};

struct Enemy
{
    int x;
    int y;
    int hp;
    int damage;
    const char* shape;
};

//미로 맵 데이터
char maze[MEZE_HEIGHT][MAZE_WIDTH];
char battle[BATTLE_UI_HEIGHT][BATTLE_UI_WIDTH];
bool gameClear = false;
bool gameOver = false;
bool bossLive = true;

Player player = { 2,1,100,20,5,"♠" };
Enemy monster[7] = { 0,0,15,5,"a" };
Enemy boss = { 0,0,100,10,"b" };
int item[2];
int secretDoor[2];
int escape[2];

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

//좌표 이동 함수
void gotoxy(int x, int y)
{
    //x, y 좌표 설정
    COORD position = { x,y };

    //커서 이동
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void Renderer()
{
    for (int i = 0; i < MEZE_HEIGHT; i++)
    {
        for (int j = 0; j < MAZE_WIDTH; j++)
        {
            switch (maze[i][j])
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
            }
        }
        printf("\n");
    }
    
    printf("HP : %d  ", player.hp);
    printf("MP : %d\n", player.mp);
    printf("Damage : %d ", player.damage);

    gotoxy(player.x, player.y);
    printf("%s", player.shape);
}

void Square(int x, int y, int hSize, int vSize)
{
    gotoxy(x, y);

    for (int i = y; i < vSize; i++)
    {
        for (int j = x; j < hSize-1; j++)
        {
            if (i == y && j == x)
                printf("┌");
            else if (i == y && j == hSize - 2)
                printf("┐");
            else if (i == vSize - 1 && j == x)
                printf("└");
            else if (i == vSize - 1 && j == hSize - 2)
                printf("┘");
            else if ((i != y || i != vSize - 1) && (j == x || j == hSize - 2))
                printf("│");
            else if ((i == y || i == vSize - 1) && (j != x || j != hSize - 2))
                printf("─");
            else
                printf("  ");
        }
        printf("\n");
    }

}

void BattleUIDraw()
{
    Square(0, 0, BATTLE_UI_HEIGHT, BATTLE_UI_WIDTH);
}

void Battle()
{
    while (true)
    {
        BattleUIDraw();

        if (player.hp <= 0)
        {
            gameOver = true;
            break;
        }

        Sleep(200);
        system("cls");
    }
}

void GetItem()
{
    player.hp += 20;
    player.mp += 5;
    player.damage += 1;
}

bool MoveCheck(int x, int y)
{
    switch (maze[player.y + y][player.x / 2 + x])
    {
    case WALL:
        return 1;
    case ITEM:
        GetItem();
        maze[player.y + y][player.x / 2 + x] = EMPTY;
        return 0;
    case MONSTER:
    case BOSS:
        Battle();
        maze[player.y + y][player.x / 2 + x] = EMPTY;
        return 0;
    case SECRET_DOOR:
        if (bossLive)
            return 1;

        maze[player.y + y][player.x / 2 + x] = EMPTY;
        return 0;
    case ESCAPE:
        gameClear = true;
        return 1;
    }
    return 0;
}

//키 입력 함수
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
            if (MoveCheck(0, -1))
                break;
            
            player.y--;
            break;
        case DOWN:
            if (MoveCheck(0, 1))
                break;
          
            player.y++;
            break;
        case LEFT:
            if (MoveCheck(-1, 0))
                break;

            player.x -= 2;
            break;
        case RIGHT:
            if (MoveCheck(1, 0))
                break;

            player.x += 2;
            break;
        }
    }
}

int main()
{

    CreateMaze();

    while (true)
    {
        
        Keyboard();
        Renderer();

        if (gameOver == true || gameClear == true)
            break;

        Sleep(100);
        system("cls");
    }
	return 0;
}