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
#define SQUARE_WIDTH 31
#define SQUARE_HEIGHT 40
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

enum Square
{
    L_TOP = '7',
    R_TOP,
    L_BOTTOM,
    R_BOTTOM,
    WIDTH,
    LENGTH
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
    int hp;
    int damage;
    const char* shape;
};

//미로 맵 데이터
char maze[MEZE_HEIGHT][MAZE_WIDTH];
char battleUI[1][1];
bool gameClear = false;
bool gameOver = false;
bool bossLive = true;

Player player = { 2,1,100,20,5,"♠" };
char monster[7][1] = { 0,0 };
char boss;
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

void MazeDraw(int x,int y,char c)
{
    gotoxy(x,y );
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
    gotoxy(MAZE_WIDTH, MEZE_HEIGHT);
}

void MazeBuffer()
{
    static char front_buffer[MEZE_HEIGHT][MAZE_WIDTH] = { ' ' };
    static Player front_player = player;

    //현재 미로와 front_buffer(이전 미로)에 있는 미로 비교
    for (int i = 0; i < MEZE_HEIGHT; ++i)
        for (int j1 = 0, j2 = 0; j1 < MAZE_WIDTH; j1++)
        {
            if (front_buffer[i][j1] != maze[i][j1])
            {
                MazeDraw(j2, i, maze[i][j1]);
                // 바뀐 부분 화면에 출력
                front_buffer[i][j1] = maze[i][j1];
                // 바뀐 부분은 출력 후 front_buffer에 저장
            }
            j2 += 2;
        }

    if (front_player.x != player.x|| front_player.y != player.y)
    {
        gotoxy(front_player.x, front_player.y);
        printf("%s", "  ");
        front_player = player;
    }

    gotoxy(0, MEZE_HEIGHT+1);
    printf("HP : %d  ", player.hp);
    printf("MP : %d\n", player.mp);
    printf("Damage : %d ", player.damage);

    gotoxy(player.x, player.y);
    printf("%s", player.shape);
}

void SquareDraw(int x, int y)
{
    gotoxy(x, y);

    for (int i = y; i < SQUARE_HEIGHT; i++)
    {
        for (int j = x, j2 = x; j < SQUARE_WIDTH - 1; j++)
        {
            if (i == y && j == x)
                MazeDraw(j2, i, L_TOP);
            else if (i == y && j == SQUARE_WIDTH - 2)
                MazeDraw(j2, i, R_TOP);
            else if (i == SQUARE_HEIGHT - 1 && j == x)
                MazeDraw(j2, i, L_BOTTOM);
            else if (i == SQUARE_HEIGHT - 1 && j == SQUARE_WIDTH - 2)
                MazeDraw(j2, i, R_BOTTOM);
            else if ((i != y || i != SQUARE_HEIGHT - 1) &&
                (j == x || j == SQUARE_WIDTH - 2))
                MazeDraw(j2, i, LENGTH);
            else if ((i == y || i == SQUARE_HEIGHT - 1) &&
                (j != x || j != SQUARE_WIDTH - 2))
                MazeDraw(j2, i, WIDTH);
            j2 += 2;
        }
    }

}


void BattleUIDraw()
{

}

void MonsterDraw(char monster)
{
    FILE* readfile = NULL;
    if (monster == MONSTER)
        readfile = fopen("alien.TXT", "r");
    else if (monster == BOSS)
        readfile = fopen("dragon.TXT", "r");

    char buffer[10000] = { 0, };

    if (readfile != NULL)
    {
        gotoxy(0, 8);
        fread(buffer, 1, 10000, readfile);
        printf("%s", buffer);
    }

    fclose(readfile);
}
void Battle(char monster)
{
    system("cls");
    MonsterDraw(monster);
    SquareDraw(0, 0);

    while (true)
    {
        BattleUIDraw();

        if (player.hp <= 0)
        {
            gameOver = true;
            break;
        }

        Sleep(100);
    }
}

void GetItem()
{
    player.hp += 20;
    player.mp += 5;
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
        Battle(object);
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
    }
}

int main()
{
    CreateMaze();
    MonsterDraw(BOSS);
    SquareDraw(0, 0);
    while (true)
    {
        
        Keyboard();
        MazeBuffer();

        if (gameOver == true || gameClear == true)
            break;

        Sleep(100);
    }
	return 0;
}