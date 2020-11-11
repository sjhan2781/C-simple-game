#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <MMSystem.h>
//#include <fmod.h>


#pragma comment (lib, "winmm.lib")
//=========================================================================
typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
//커서 타입 정의

#define TIME 60
#define PRINTSPEED 45 // 오르마이티 출력 속도
#define ESC 27 // ESC 의 아스키 코드
#define printxy(x,y,str) {gotoxy(x,y); printf(#str"");} //printxy 함수 정의
#define map_YMAX 39 // 최대 이동가능 y 축
#define MAXBULLET 5 // 최대 총알 갯수
#define EXTENDASCII -32 // 확장 아스키 코드
#define MAXENEMY 5 // 최대 적 생성 카운트
#define MAXENEMYBULLET 50 //최대 적군 총알개수
#define randomize() srand((unsigned)time(NULL)) //난수 발생기 초기화
#define random(n) (rand() % (n)) // 0~n 까지의 난수 발생
#define LIST 30 //랭킹 최대 인원
#define GAMESPEED 16 //게임속도
#define WAITTIME 50//게임제목창글자속도

char *VERSION = "1.3.0";
// 패치노트
//1.0.1 BGM삽입. 사소한 버그 수정
//1.0.2 피격 판정 전반적 수정, 캐릭터 다양성 증가
//1.0.3 게임 반복 구현. 사소한 버그 수정
//1.0.4 랭킹 구현. 사소한 버그 수정
//1.0.5 그래픽 겹침 문제 해결
//1.0.6 게임중 플레이타임 출력 구현
//1.1.0 제목, 로고 삽입
//1.1.1 체력오류 수정. 사소한 버그 수정
//1.1.2 콤보구현
//1.1.3 무작위 총알 생성 위치 버그 수정
//1.2.0 피격 판정 버그 전면 수정
//1.2.1 콤보 보너스 점수 추가
//1.2.2 게임 밸런스 패치. 사소한 버그 수정
//1.2.3 두번째 스테이지 구현
//1.2.4 세번째 스테이지, 무한모드 구현
//1.2.5 스테이지별 난이도 밸런스 패치
//1.3.0 게임 BGM 재생중 효과음 구현
//=========================================================================
// 변수 선언 구역
int diff = 3;
//난이도
int bulletspeed = 2;
// 상대 총알 속도
int combo = 0;
// 콤보
int kill = 0;
// 킬 수
int comboBonusSave;
// 콤보, 콤보 보너스 점수 기억
int clearScore = 200;
//클리어 점수
int stageNumber = 1;
//스테이지 수
int playerHealth = 4;
//플레이어 체력
int px = 30;
int py = 39;
// 플레이어 위치
int bx = -1, by;
// 총알 위치
int score = 0;
// 점수
char *arEnemy[] = { " ◀◆◇◆▶ ", " ◁-※-▷ ", " ◆M▣M◆ ", " ◐〓■〓◑ " };
// 적군 모양 
clock_t start, finish;

double playtime;

int bossHealth = 10;

/*FMOD_SYSTEM *g_System;
FMOD_SOUND  *g_Sound;
FMOD_CHANNEL *channel = NULL;*/

//=========================================================================
// 데이터 선언 구역
enum
{
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKYBLUE,
	DARK_RED,
	DARK_VIOLET,
	DARK_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
};
//색 데이터 선언
struct
{
	BOOL exist;
	int x, y;
}Bullet[MAXBULLET];
// 구조체 [플레이어 총알] 
typedef struct enemy
{
	BOOL exist;
	int type;
	int x, y;
	int dest;
	int nFrame;
	int nCount;

}ENEMY;
// 구조체 [적군]

typedef struct enemybullet
{
	BOOL exist;
	int x, y;
	int nFrame;
	int nStay;
}ENEMYBULLET;
// 구조체 [적군 총알]
typedef struct players {
	char name[100];
	int score;
	double time;
}PLAYER;

typedef struct boss
{
	BOOL exist;
	int x, y;
	int dest;
	int ydest;
	int nFrame;
	int nCount;
	int yFrame;
	int yCount;
}BOSS;
// 구조체 [보스]
typedef struct bossbullet
{
	BOOL exist;
	int x, y;
	int nFrame;
	int nStay;
}BOSSBULLET;
// 구조체 [보스 총알]

ENEMY Enemy1[MAXENEMY], Enemy2[MAXENEMY];

ENEMYBULLET Enemy1Bullet[MAXENEMYBULLET], Enemy2Bullet[MAXENEMYBULLET];

BOSS boss;
BOSSBULLET bossBullet1[MAXENEMYBULLET], bossBullet2[MAXENEMYBULLET];

PLAYER list[LIST] = { "AAA", 0, 0 };
//=========================================================================
// 함수 선언 구역

void gotoxy(int x, int y); // 커서 이동
void setcursortype(CURSOR_TYPE c); //커서 타입 지정
void setColor(int color); //색깔 넣기
void loadingStage(); //게임 시작화면
void drawPlayer(); //플레이어 출력
void erasePlayer(); // 지나간 플레이어 지우기
void playerMoveAction(); // 플레이어 이동 
void statusBar(); // 상태바 구성
void drawBullet(int i); //총알 출력
void eraseBullet(int i); // 지나간 총알 지우기
void playerBulletAction(); // 총알 움직임 구현
void drawEnemy1(); // 적군 출력
void drawEnemy2();
void enemy1MoveAction(); //적군 움직임 구현
void enemy2MoveAction();
void enemy1BulletAction(int i); // 적군 총알 (속도/좌표)
void enemy2BulletAction(int i);
void eraseEnemy1Bullet(int i); // 지나간 적군 총알 지움
void eraseEnemy2Bullet(int i);
void moveEnemy1Bullet(); // 적군 총알 움직임
void moveEnemy2Bullet();
void enemy1Crash(); //적군 피격 판정
void enemy2Crash();
void playerCrash(); // 플레이어 피격 판정
void reset(); //상태 초기화
void printGameTitle(int x, int y);//게임 제목 출력
void rankPrint(struct player list[]); // 랭킹 출력
void GAMEOVER(); //게임오버 메세지 출력
int comboBonus(int combo); //콤보보너스
//void InIt();  //BGM삽입
//void Release(); //BGM메모리 해제
void drawBoss(); // 보스 생성 / bossmoveaction 에서
void bossMoveAction(); // 보스 움직임
void bossPattern1(); //보스 총알 패턴1
void bossPattern2();
void drawBossBullet1(int i); //보스총알 출력
void drawBossBullet2(int i);
void eraseBossBullet1(int i); //보스총알 지움
void eraseBossBullet2(int i);
void moveBossBullet1(); //보스 총알 움직임
void moveBossBullet2();
void bossCrash(); //보스 피격판정
//=====================================================================

// 함수 정의 구역
void title_a() {
	int i;

	Sleep(800);
	setColor(YELLOW);

	for (i = 0; i < 42; i += 2) {
		gotoxy(i, 8); printf("━"); gotoxy(80 - i, 8); printf("━");
		gotoxy(i, 9); printf("━"); gotoxy(80 - i, 9); printf("━");
		gotoxy(i, 39); printf("━"); gotoxy(80 - i, 39); printf("━");
		gotoxy(i, 40); printf("━"); gotoxy(80 - i, 40); printf("━");
		Sleep(20);
	}

	setColor(WHITE);
	PlaySound(TEXT("Almighty.wav"), NULL, SND_ASYNC);

	gotoxy(1, 16); printf("        ■");
	gotoxy(1, 17); printf("      ■  ");
	gotoxy(1, 18); printf("          ");
	gotoxy(1, 19); printf("          ");
	gotoxy(1, 20); printf("        ");
	gotoxy(1, 21); printf(" ");
	gotoxy(1, 22); printf("  ");
	gotoxy(1, 25); printf("  ");
	gotoxy(1, 26); printf("");
	gotoxy(1, 27); printf("");
	gotoxy(1, 28); printf("");
	gotoxy(1, 29); printf("");
	gotoxy(1, 30); printf("");
	gotoxy(1, 31); printf("");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■ ");
	gotoxy(1, 17); printf("      ■     ");
	gotoxy(1, 18); printf("      ■     ");
	gotoxy(1, 19); printf("       ");
	gotoxy(1, 20); printf("      ");
	gotoxy(1, 21); printf("      ");
	gotoxy(1, 22); printf("      ");
	gotoxy(1, 25); printf("       ");
	gotoxy(1, 26); printf("      ");
	gotoxy(1, 27); printf("       ");
	gotoxy(1, 28); printf("       ");
	gotoxy(1, 29); printf("       ");
	gotoxy(1, 30); printf("        ");
	gotoxy(1, 31); printf("        ");
	Sleep(TIME);


	gotoxy(1, 16); printf("        ■■■ ");
	gotoxy(1, 17); printf("      ■     ");
	gotoxy(1, 18); printf("      ■     ");
	gotoxy(1, 19); printf("      ■");
	gotoxy(1, 20); printf("  ");
	gotoxy(1, 21); printf("");
	gotoxy(1, 22); printf(" ");
	gotoxy(1, 25); printf(" ");
	gotoxy(1, 26); printf(" ");
	gotoxy(1, 27); printf("");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf("  ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■");
	gotoxy(1, 17); printf("      ■      ■");
	gotoxy(1, 18); printf("      ■      ");
	gotoxy(1, 19); printf("      ■");
	gotoxy(1, 20); printf("");
	gotoxy(1, 21); printf("");
	gotoxy(1, 22); printf(" ");
	gotoxy(1, 25); printf(" ");
	gotoxy(1, 26); printf(" ");
	gotoxy(1, 27); printf("");
	gotoxy(1, 28); printf("");
	gotoxy(1, 29); printf("");
	gotoxy(1, 30); printf("");
	gotoxy(1, 31); printf("");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■ ");
	gotoxy(1, 17); printf("      ■      ■    ■ ");
	gotoxy(1, 18); printf("      ■      ■");
	gotoxy(1, 19); printf("      ■■ ");
	gotoxy(1, 20); printf("      ");
	gotoxy(1, 21); printf(" ");
	gotoxy(1, 22); printf(" ");
	gotoxy(1, 25); printf("   ");
	gotoxy(1, 26); printf("  ");
	gotoxy(1, 27); printf(" ");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf("  ");
	gotoxy(1, 30); printf("  ");
	gotoxy(1, 31); printf("  ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■ ");
	gotoxy(1, 18); printf("      ■      ■    ■   ");
	gotoxy(1, 19); printf("      ■■■ ");
	gotoxy(1, 20); printf("      ■ ");
	gotoxy(1, 21); printf(" ");
	gotoxy(1, 22); printf(" ");
	gotoxy(1, 25); printf("  ");
	gotoxy(1, 26); printf("  ");
	gotoxy(1, 27); printf(" ");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf("  ");
	gotoxy(1, 31); printf("  ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ");
	gotoxy(1, 19); printf("      ■■■■      ■ ");
	gotoxy(1, 20); printf("      ■      ■");
	gotoxy(1, 21); printf("      ■ ");
	gotoxy(1, 22); printf(" ");
	gotoxy(1, 25); printf(" ");
	gotoxy(1, 26); printf("");
	gotoxy(1, 27); printf("  ");
	gotoxy(1, 28); printf("  ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf("  ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■  ");
	gotoxy(1, 20); printf("      ■      ■    ■");
	gotoxy(1, 21); printf("      ■      ■ ");
	gotoxy(1, 22); printf("      ■     ");
	gotoxy(1, 25); printf(" ");
	gotoxy(1, 26); printf("  ");
	gotoxy(1, 27); printf(" ");
	gotoxy(1, 28); printf("");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf("");
	gotoxy(1, 31); printf("");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■ ");
	gotoxy(1, 20); printf("      ■      ■    ■ ");
	gotoxy(1, 21); printf("      ■      ■    ■ ");
	gotoxy(1, 22); printf("      ■      ■    ");
	gotoxy(1, 25); printf("  ");
	gotoxy(1, 26); printf(" ");
	gotoxy(1, 27); printf(" ");
	gotoxy(1, 28); printf("  ");
	gotoxy(1, 29); printf("");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf("");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■  ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■ ");
	gotoxy(1, 22); printf("      ■      ■    ■  ");
	gotoxy(1, 25); printf("             ■■ ");
	gotoxy(1, 26); printf("             ■");
	gotoxy(1, 27); printf("                ");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■       ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■         ");
	gotoxy(1, 25); printf("             ■■    ■");
	gotoxy(1, 26); printf("             ■ ■  ");
	gotoxy(1, 27); printf("             ■   ");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■     ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■     ■");
	gotoxy(1, 25); printf("             ■■    ■■ ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■");
	gotoxy(1, 27); printf("             ■   ■   ");
	gotoxy(1, 28); printf("             ■ ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■   ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■  ");
	gotoxy(1, 25); printf("             ■■    ■■       ■");
	gotoxy(1, 26); printf("             ■ ■  ■ ■  ");
	gotoxy(1, 27); printf("             ■   ■   ■");
	gotoxy(1, 28); printf("             ■  ");
	gotoxy(1, 29); printf("             ■ ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■ ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■");
	gotoxy(1, 25); printf("             ■■    ■■       ■■        ■ ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■  ");
	gotoxy(1, 27); printf("             ■   ■   ■  ");
	gotoxy(1, 28); printf("             ■        ■  ");
	gotoxy(1, 29); printf("             ■  ");
	gotoxy(1, 30); printf("             ■  ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■        ■■   ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■   ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ");
	gotoxy(1, 28); printf("             ■        ■ ");
	gotoxy(1, 29); printf("             ■        ■  ");
	gotoxy(1, 30); printf("             ■        ■ ");
	gotoxy(1, 31); printf("             ■        ");
	Sleep(TIME);


	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■ ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■          ■");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■  ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■                      ");
	gotoxy(1, 28); printf("             ■        ■          ■ ");
	gotoxy(1, 29); printf("             ■        ■  ");
	gotoxy(1, 30); printf("             ■        ■ ");
	gotoxy(1, 31); printf("             ■        ■  ");
	Sleep(TIME);


	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■ ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■      ■■ ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■            ■         ■  ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■ ");
	gotoxy(1, 28); printf("             ■        ■          ■ ");
	gotoxy(1, 29); printf("             ■        ■          ■ ");
	gotoxy(1, 30); printf("             ■        ■  ");
	gotoxy(1, 31); printf("             ■        ■    ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■ ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■  ");
	gotoxy(1, 29); printf("             ■        ■          ■   ");
	gotoxy(1, 30); printf("             ■        ■          ■  ");
	gotoxy(1, 31); printf("             ■        ■   ■ ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■                             ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■   ");
	gotoxy(1, 29); printf("             ■        ■          ■             ■    ");
	gotoxy(1, 30); printf("             ■        ■          ■  ");
	gotoxy(1, 31); printf("             ■        ■   ■  ■");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■                             ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■          ■■");
	gotoxy(1, 29); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 30); printf("             ■        ■          ■             ■  ");
	gotoxy(1, 31); printf("             ■        ■   ■  ■■");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■                             ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■          ■■■ ");
	gotoxy(1, 29); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 30); printf("             ■        ■          ■             ■                    ");
	gotoxy(1, 31); printf("             ■        ■   ■  ■■■■          ■ ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■                             ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■          ■■■■    ");
	gotoxy(1, 29); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 30); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 31); printf("             ■        ■   ■  ■■■■    ■    ■    ■    ■");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■                             ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■          ■■■■    ");
	gotoxy(1, 29); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 30); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 31); printf("             ■        ■   ■  ■■■■    ■    ■    ■    ■■  ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ■■■      ■         ■               ");
	gotoxy(1, 17); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 18); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 19); printf("      ■■■■■    ■         ■           ■■■■■■                             ");
	gotoxy(1, 20); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 21); printf("      ■      ■    ■         ■                      ");
	gotoxy(1, 22); printf("      ■      ■    ■■■■   ■■■■         ");
	gotoxy(1, 25); printf("             ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
	gotoxy(1, 26); printf("             ■ ■  ■ ■          ■             ■          ■          ");
	gotoxy(1, 27); printf("             ■   ■   ■          ■             ■          ■          ");
	gotoxy(1, 28); printf("             ■        ■          ■             ■          ■■■■    ");
	gotoxy(1, 29); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 30); printf("             ■        ■          ■             ■          ■          ");
	gotoxy(1, 31); printf("             ■        ■   ■  ■■■■    ■    ■    ■    ■■■■  ");
	Sleep(TIME);

	Sleep(500);
	printxy(52, 41, BY JSM & HSJ & LJH & LMS);
	PlaySound(NULL, 0, 0);
	Sleep(2200);
}
//타이틀 a
/*void title_b() {
int i;

Sleep(800);
setColor(YELLOW);

for (i = 0; i < 42; i++) {
gotoxy(i, 8); printf("━"); gotoxy(80-i, 8); printf("━");
gotoxy(i, 9); printf("━"); gotoxy(80-i,9); printf("━");
gotoxy(i, 39); printf("━"); gotoxy(80-i, 39); printf("━");
gotoxy(i, 40); printf("━"); gotoxy(80-i, 40); printf("━");
Sleep(22);
}
setColor(WHITE);
PlaySound(TEXT("Almighty.wav"), NULL, SND_ASYNC | SND_LOOP);
gotoxy(1,16); printf("  ■           ");
gotoxy(1,17); printf("■            ");
gotoxy(1,18); printf("■         ");
gotoxy(1,19); printf("■■                             ");
gotoxy(1,20); printf("■         ");
gotoxy(1,21); printf("■                ");
gotoxy(1,22); printf("■          ");
gotoxy(1,25); printf("                                                                      ■■");
gotoxy(1,26); printf("                   ");
gotoxy(1,27); printf("                      ");
gotoxy(1,28); printf("                                                                      ■■ ");
gotoxy(1,29); printf("                        ");
gotoxy(1,30); printf("                      ");
gotoxy(1,31); printf("                                                                      ■■ ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■              ");
gotoxy(1, 17); printf("■                         ");
gotoxy(1, 18); printf("■                   ");
gotoxy(1, 19); printf("■■■                                ");
gotoxy(1, 20); printf("■                         ");
gotoxy(1, 21); printf("■                          ");
gotoxy(1, 22); printf("■                   ");
gotoxy(1, 25); printf("                                                                 ■■■■ ");
gotoxy(1, 26); printf("                                                                 ■");
gotoxy(1, 27); printf("                                                                 ■");
gotoxy(1, 28); printf("                                                                 ■■■■  ");
gotoxy(1, 29); printf("                                                                 ■ ");
gotoxy(1, 30); printf("                                                                 ■  ");
gotoxy(1, 31); printf("                                                                 ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■                              ");
gotoxy(1, 17); printf("■      ■                                   ");
gotoxy(1, 18); printf("■      ■                                  ");
gotoxy(1, 19); printf("■■■■■                                         ");
gotoxy(1, 20); printf("■      ■                                 ");
gotoxy(1, 21); printf("■      ■                                 ");
gotoxy(1, 22); printf("■      ■           ");
gotoxy(1, 25); printf("                                                                 ■■■■ ");
gotoxy(1, 26); printf("                                                                 ■");
gotoxy(1, 27); printf("                                                                 ■");
gotoxy(1, 28); printf("                                                                 ■■■■  ");
gotoxy(1, 29); printf("                                                                 ■ ");
gotoxy(1, 30); printf("                                                                 ■  ");
gotoxy(1, 31); printf("                                                           ■    ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■                       ");
gotoxy(1, 17); printf("■      ■    ■                              ");
gotoxy(1, 18); printf("■      ■    ■                             ");
gotoxy(1, 19); printf("■■■■■    ■                                   ");
gotoxy(1, 20); printf("■      ■    ■                               ");
gotoxy(1, 21); printf("■      ■    ■                          ");
gotoxy(1, 22); printf("■      ■    ■        ");
gotoxy(1, 25); printf("                                                       ■■      ■■■■ ");
gotoxy(1, 26); printf("                                                                 ■");
gotoxy(1, 27); printf("                                                                 ■");
gotoxy(1, 28); printf("                                                                 ■■■■  ");
gotoxy(1, 29); printf("                                                                 ■ ");
gotoxy(1, 30); printf("                                                                 ■  ");
gotoxy(1, 31); printf("                                                           ■    ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■                        ");
gotoxy(1, 17); printf("■      ■    ■                               ");
gotoxy(1, 18); printf("■      ■    ■                               ");
gotoxy(1, 19); printf("■■■■■    ■                                       ");
gotoxy(1, 20); printf("■      ■    ■                              ");
gotoxy(1, 21); printf("■      ■    ■                   ");
gotoxy(1, 22); printf("■      ■    ■■■■       ");
gotoxy(1, 25); printf("                                                  ■■■■■     ■■■■ ");
gotoxy(1, 26); printf("                                                      ■         ■");
gotoxy(1, 27); printf("                                                      ■         ■");
gotoxy(1, 28); printf("                                                      ■         ■■■■  ");
gotoxy(1, 29); printf("                                                      ■         ■ ");
gotoxy(1, 30); printf("                                                      ■         ■  ");
gotoxy(1, 31); printf("                                                      ■   ■    ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■                        ");
gotoxy(1, 17); printf("■      ■    ■                               ");
gotoxy(1, 18); printf("■      ■    ■                               ");
gotoxy(1, 19); printf("■■■■■    ■                                       ");
gotoxy(1, 20); printf("■      ■    ■                              ");
gotoxy(1, 21); printf("■      ■    ■                   ");
gotoxy(1, 22); printf("■      ■    ■■■■       ");
gotoxy(1, 25); printf("                                                  ■■■■■     ■■■■ ");
gotoxy(1, 26); printf("                                                      ■         ■");
gotoxy(1, 27); printf("                                                      ■         ■");
gotoxy(1, 28); printf("                                                      ■         ■■■■  ");
gotoxy(1, 29); printf("                                                      ■         ■ ");
gotoxy(1, 30); printf("                                                      ■         ■  ");
gotoxy(1, 31); printf("                                               ■     ■   ■    ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                      ");
gotoxy(1, 19); printf("■■■■■    ■         ■                                      ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■         ");
gotoxy(1, 25); printf("                                         ■■     ■■■■■     ■■■■ ");
gotoxy(1, 26); printf("                                                      ■         ■");
gotoxy(1, 27); printf("                                                      ■         ■");
gotoxy(1, 28); printf("                                                      ■         ■■■■  ");
gotoxy(1, 29); printf("                                                      ■         ■ ");
gotoxy(1, 30); printf("                                                      ■         ■  ");
gotoxy(1, 31); printf("                                         ■■  ■     ■   ■    ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                      ");
gotoxy(1, 19); printf("■■■■■    ■         ■                                      ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■■■■  ");
gotoxy(1, 25); printf("                                     ■■■■     ■■■■■     ■■■■ ");
gotoxy(1, 26); printf("                                        ■            ■         ■");
gotoxy(1, 27); printf("                                        ■            ■         ■");
gotoxy(1, 28); printf("                                        ■            ■         ■■■■  ");
gotoxy(1, 29); printf("                                        ■            ■         ■ ");
gotoxy(1, 30); printf("                                        ■            ■         ■  ");
gotoxy(1, 31); printf("                                     ■■■■  ■     ■   ■    ■■■■");
Sleep(PRINTSPEED);


gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                      ");
gotoxy(1, 19); printf("■■■■■    ■         ■                                      ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■■■■   ");
gotoxy(1, 25); printf("                                     ■■■■     ■■■■■     ■■■■ ");
gotoxy(1, 26); printf("                                        ■            ■         ■");
gotoxy(1, 27); printf("                                        ■            ■         ■");
gotoxy(1, 28); printf("                                        ■            ■         ■■■■  ");
gotoxy(1, 29); printf("                                        ■            ■         ■ ");
gotoxy(1, 30); printf("                                        ■            ■         ■  ");
gotoxy(1, 31); printf("                               ■    ■■■■  ■     ■   ■    ■■■■");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                       ");
gotoxy(1, 19); printf("■■■■■    ■         ■           ■■                            ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■■■■         ");
gotoxy(1, 25); printf("                          ■       ■■■■      ■■■■■      ■■■■  ");
gotoxy(1, 26); printf("                          ■          ■             ■          ■          ");
gotoxy(1, 27); printf("                          ■          ■             ■          ■          ");
gotoxy(1, 28); printf("                          ■          ■             ■          ■■■■    ");
gotoxy(1, 29); printf("                          ■          ■             ■          ■          ");
gotoxy(1, 30); printf("                          ■          ■             ■          ■          ");
gotoxy(1, 31); printf("                          ■   ■  ■■■■    ■    ■    ■    ■■■■  ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                      ");
gotoxy(1, 19); printf("■■■■■    ■         ■           ■■■■                             ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■■■■         ");
gotoxy(1, 25); printf("                  ■    ■■       ■■■■      ■■■■■      ■■■■  ");
gotoxy(1, 26); printf("                   ■  ■ ■          ■             ■          ■          ");
gotoxy(1, 27); printf("                     ■   ■          ■             ■          ■          ");
gotoxy(1, 28); printf("                          ■          ■             ■          ■■■■    ");
gotoxy(1, 29); printf("                          ■          ■             ■          ■          ");
gotoxy(1, 30); printf("                          ■          ■             ■          ■          ");
gotoxy(1, 31); printf("                          ■   ■  ■■■■    ■    ■    ■    ■■■■  ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                      ");
gotoxy(1, 19); printf("■■■■■    ■         ■           ■■■■■■                             ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■■■■         ");
gotoxy(1, 25); printf("                ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
gotoxy(1, 26); printf("                ■ ■  ■ ■          ■             ■          ■          ");
gotoxy(1, 27); printf("                ■   ■   ■          ■             ■          ■          ");
gotoxy(1, 28); printf("                ■        ■          ■             ■          ■■■■    ");
gotoxy(1, 29); printf("                ■        ■          ■             ■          ■          ");
gotoxy(1, 30); printf("                ■        ■          ■             ■          ■          ");
gotoxy(1, 31); printf("                ■        ■   ■  ■■■■    ■    ■    ■    ■■■■  ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ■■■      ■         ■               ");
gotoxy(1, 17); printf("■      ■    ■         ■                      ");
gotoxy(1, 18); printf("■      ■    ■         ■                      ");
gotoxy(1, 19); printf("■■■■■    ■         ■           ■■■■■■                             ");
gotoxy(1, 20); printf("■      ■    ■         ■                      ");
gotoxy(1, 21); printf("■      ■    ■         ■                      ");
gotoxy(1, 22); printf("■      ■    ■■■■   ■■■■         ");
gotoxy(1, 25); printf("                ■■    ■■       ■■■■      ■■■■■      ■■■■  ");
gotoxy(1, 26); printf("                ■ ■  ■ ■          ■             ■          ■          ");
gotoxy(1, 27); printf("                ■   ■   ■          ■             ■          ■          ");
gotoxy(1, 28); printf("                ■        ■          ■             ■          ■■■■    ");
gotoxy(1, 29); printf("                ■        ■          ■             ■          ■          ");
gotoxy(1, 30); printf("                ■        ■          ■             ■          ■          ");
gotoxy(1, 31); printf("                ■        ■   ■  ■■■■    ■    ■    ■    ■■■■  ");
Sleep(PRINTSPEED);

Sleep(500);
printxy(52,41,BY JSM & HSJ & LJH & LMS);
PlaySound(NULL, 0, 0);
Sleep(2200);

}
// 타이틀 b*/
void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
// 커서 이동
void setcursortype(CURSOR_TYPE c)
{
	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}
//커서 타입 지정
void setColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
//색깔 넣기
void loadingStage(){
	int i;
	int	move = 4;
	int move2 = 2;

	for (i = 6 + move; i < 68 + move; i += 2)
	{
		setColor(RED);
		printxy(i, 9, ━);
		Sleep(15);
	}

	printxy(66 + move, 9, ┓);
	for (i = 10; i < 38; i++)
	{
		setColor(RED);
		printxy(66 + move, i, ┃);
		Sleep(25);
	}

	printxy(66 + move, 37, ┛);
	for (i = 64 + move; i > 4 + move; i -= 2)
	{
		setColor(VIOLET);
		printxy(i, 37, ━);
		Sleep(15);
	}

	printxy(4 + move, 37, ┗);
	for (i = 36; i > 9; i--)
	{
		setColor(VIOLET);
		printxy(4 + move, i, ┃);
		Sleep(25);
	}
	setColor(RED);
	printxy(4 + move, 9, ┏);

	Sleep(500);

	setColor(WHITE);
	printxy(29 + move, 4, A) Sleep(100);
	printxy(31 + move, 4, L) Sleep(100);
	printxy(33 + move, 4, L) Sleep(100);
	printxy(35 + move, 4, -) Sleep(100);
	printxy(37 + move, 4, M) Sleep(100);
	printxy(39 + move, 4, I) Sleep(100);
	printxy(41 + move, 4, T) Sleep(100);
	printxy(43 + move, 4, E) Sleep(100);

	setColor(WHITE);
	printxy(24 + move, 6, P) Sleep(70);
	printxy(28 + move, 6, R) Sleep(70);
	printxy(32 + move, 6, E) Sleep(70);
	printxy(36 + move, 6, S) Sleep(70);
	printxy(40 + move, 6, E) Sleep(70);
	printxy(44 + move, 6, N) Sleep(70);
	printxy(48 + move, 6, T) Sleep(70);




	setColor(WHITE);
	printGameTitle(11, 10);

	setColor(YELLOW);
	gotoxy(51, 8); // 버전관리
	printf("SHOOT & DODGE v%s", VERSION);

	printxy(14, 38, Copyrightⓒ 2015.All - M.I.T.E All Rights Reserved.);

	setColor(YELLOW);


	while (1){
		printxy(29, 45, PRESS ANY KEY TO START);
		Sleep(600);
		printxy(29, 45, 　　　　　　　　　　　);
		Sleep(600);

		if (kbhit())
			break;
	}

}
//게임 시작화면   및 버전 관리
void drawPlayer()
{
	setColor(WHITE);
	gotoxy(px + 2, py - 1);
	printf("▲");
	gotoxy(px, py);
	printf("◀■▶");
}
//플레이어 출력
void erasePlayer(){
	printxy(px + 2, py - 1, 　);
	printxy(px, py, 　　　);
}
// 지나간 플레이어 지우기
void playerMoveAction(){
	int i, ch;

	if (GetAsyncKeyState(VK_LEFT)<0 && px>1){
		erasePlayer();
		px--;
		drawPlayer();
	}//왼쪽이동
	else if (GetAsyncKeyState(VK_RIGHT)<0 && px<70){
		erasePlayer();
		px++;
		drawPlayer();
	}//오른쪽이동
	else if (GetAsyncKeyState(VK_UP)<0 && py > 28){
		erasePlayer();
		py--;
		drawPlayer();
	}//위로이동
	else if (GetAsyncKeyState(VK_DOWN)<0 && py < 41){
		erasePlayer();
		py++;
		drawPlayer();
	}//아래로이동
	if (kbhit())
	{
		ch = getch();
		switch (ch)
		{
		case ' ':
			for (i = 0; i<MAXBULLET && Bullet[i].exist == TRUE; i++);
			if (i != MAXBULLET)
			{
				Bullet[i].x = px + 3;
				Bullet[i].y = py - 1;
				Bullet[i].exist = TRUE;
			}
			break;
		case ESC:
			exit(0);
		}
	}//if
}
// 플레이어 이동 
void statusBar(){
	int i, count;

	setColor(WHITE);
	printxy(0, 42, ▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣);
	printxy(5, 46, HEALTH:);

	setColor(WHITE);

	for (i = 0; i < playerHealth; i++){
		gotoxy(13 + 2 * i, 46);
		printf("■");
	}
	count = 4 - playerHealth;
	for (i = 0; i < count; i++){
		gotoxy(19 - 2 * i, 46);
		printf("　");
	}

	if (combo >= 10){
		gotoxy(50, 44);
		setColor(YELLOW);
		printf("%d ", combo);
		setColor(WHITE);
		printf("콤보!  BONUS + %d 점", comboBonus(combo));
	}
	else if (combo == 0){
		gotoxy(50, 44);
		printf("　　　                         ");
	}

	gotoxy(50, 46);
	printf("점수 : %d 점", score);

	finish = clock();
	playtime = (finish - start) / CLOCKS_PER_SEC;
	gotoxy(50, 48);
	printf("Play Time : %d 분 %d 초", (int)playtime / 60, (int)playtime % 60);
}
// 상태바 구성
void drawBullet(int i){
	setColor(WHITE);
	printxy(Bullet[i].x, Bullet[i].y, *);
}
// 플레이어 총알 출력
void eraseBullet(int i){
	printxy(Bullet[i].x, Bullet[i].y, 　);
}
// 지나간 플레이어 총알 지우기
void playerBulletAction(){
	int i;

	for (i = 0; i<MAXBULLET; i++)
	{
		if (Bullet[i].exist == TRUE)
		{
			eraseBullet(i);
			if ((Bullet[i].y) == 0){
				Bullet[i].exist = FALSE;
			}//if
			else{
				Bullet[i].y--;
				drawBullet(i);
			}//else
		}// large if
	} //  for
}
// 플레이어 총알 움직임 구현
void drawEnemy1()
{
	int i, j, check;

	if (random(15) == 0)
	{

		for (i = 0; i< MAXENEMY && Enemy1[i].exist == TRUE; i++);

		if (i != MAXENEMY)
		{
			if (random(2) == 0)
			{
				Enemy1[i].x = 5;
				Enemy1[i].dest = 1;
			}
			else
			{
				Enemy1[i].x = 65;
				Enemy1[i].dest = -1;
			}

			while (1){
				check = random(5) + 1;
				for (j = 1; j<MAXENEMY; j++){
					if (check == Enemy1[j].y)
						break;
				}
				if (j == MAXENEMY){
					Enemy1[i].y = check;
					break;
				}
			}

			Enemy1[i].nFrame = Enemy1[i].nCount = random(2) + 1; // 기본 속도 1 추가속도 (0~5)
			Enemy1[i].type = random(sizeof(arEnemy) / sizeof(arEnemy[0]));
			Enemy1[i].exist = TRUE;
		}
	}
}
void drawEnemy2()
{
	int i, j, check;

	if (random(15) == 0)
	{

		for (i = 0; i< MAXENEMY && Enemy2[i].exist == TRUE; i++);

		if (i != MAXENEMY)
		{
			if (random(2) == 0)
			{
				Enemy2[i].x = 5;
				Enemy2[i].dest = 1;
			}
			else
			{
				Enemy2[i].x = 65;
				Enemy2[i].dest = -1;
			}

			while (1){
				check = random(5) + 6;
				for (j = 0; j<MAXENEMY; j++){
					if (check == Enemy2[j].y)
						break;
				}
				if (j == MAXENEMY){
					Enemy2[i].y = check;
					break;
				}
			}

			Enemy2[i].nFrame = Enemy2[i].nCount = random(2) + 1; // 기본 속도 4 추가속도 (0~2)
			Enemy2[i].type = random(sizeof(arEnemy) / sizeof(arEnemy[0]));
			Enemy2[i].exist = TRUE;
		}
	}
}
// 적군 생성 / enemymoveaction 에서 
void enemy1MoveAction(){
	int i;

	for (i = 0; i< MAXENEMY; i++)
	{
		if (Enemy1[i].exist == FALSE)
			continue;

		if (--Enemy1[i].nCount == 0)
		{
			if (Enemy1[i].type == -1){
				printxy(Enemy1[i].x - 3, Enemy1[i].y, );
				printxy(Enemy1[i].x - 3, Enemy1[i].y - 1, );
				printxy(Enemy1[i].x - 3, Enemy1[i].y + 1, );
				Enemy1[i].y = 0;
				Enemy1[i].exist = FALSE;
				continue;
			}

			Enemy1[i].nCount = Enemy1[i].nFrame;

			if (Enemy1[i].x > 70 || Enemy1[i].x < 4)
			{
				Enemy1[i].dest = (-1)*Enemy1[i].dest;
				Enemy1[i].x += Enemy1[i].dest;
			} //inner if
			else
			{
				Enemy1[i].x += Enemy1[i].dest;
				setColor(RED);
				gotoxy(Enemy1[i].x - 3, Enemy1[i].y);
				printf(arEnemy[Enemy1[i].type]);

				enemy1BulletAction(i);
			}//inner else
		}//outer if
	}//for
}
void enemy2MoveAction(){
	int i;

	for (i = 0; i< MAXENEMY; i++)
	{
		if (Enemy2[i].exist == FALSE)
			continue;

		if (--Enemy2[i].nCount == 0)
		{
			if (Enemy2[i].type == -1){
				printxy(Enemy2[i].x - 3, Enemy2[i].y, );
				printxy(Enemy2[i].x - 3, Enemy2[i].y - 1, );
				printxy(Enemy2[i].x - 3, Enemy2[i].y + 1, );
				Enemy2[i].y = 0;
				Enemy2[i].exist = FALSE;
				continue;
			}

			Enemy2[i].nCount = Enemy2[i].nFrame;

			if (Enemy2[i].x > 70 || Enemy2[i].x < 4)
			{
				Enemy2[i].dest = (-1)*Enemy2[i].dest;
				Enemy2[i].x += Enemy2[i].dest;
			} //inner if
			else
			{
				Enemy2[i].x += Enemy2[i].dest;
				setColor(VIOLET);
				gotoxy(Enemy2[i].x - 3, Enemy2[i].y);
				printf(arEnemy[Enemy2[i].type]);

				enemy2BulletAction(i);
			}//inner else
		}//outer if
	}//for
}
// 적군 출력 및 움직임
void enemy1BulletAction(int i){
	int j;

	if (random(5) == 0)
	{
		for (j = 0; j < MAXENEMYBULLET && Enemy1Bullet[j].exist == TRUE; j++);
		if (j != MAXENEMY)
		{
			Enemy1Bullet[j].x = Enemy1[i].x + 2;
			Enemy1Bullet[j].y = Enemy1[i].y + 1;
			Enemy1Bullet[j].nFrame = Enemy1Bullet[j].nStay = diff + random(3);
			Enemy1Bullet[j].exist = TRUE;
		}
	}
}
void enemy2BulletAction(int i){
	int j;

	if (random(5) == 0)
	{
		for (j = 0; j < MAXENEMYBULLET && Enemy2Bullet[j].exist == TRUE; j++);
		if (j != MAXENEMY)
		{
			Enemy2Bullet[j].x = Enemy2[i].x + 2;
			Enemy2Bullet[j].y = Enemy2[i].y + 1;
			Enemy2Bullet[j].nFrame = Enemy2Bullet[j].nStay = 2 + random(3);
			Enemy2Bullet[j].exist = TRUE;
		}
	}
}
// 적군 총알 (속도/좌표)
void drawEnemy1Bullet(int i)
{
	setColor(YELLOW);
	printxy(Enemy1Bullet[i].x, Enemy1Bullet[i].y, 。);
}
void drawEnemy2Bullet(int i)
{
	setColor(YELLOW);
	printxy(Enemy2Bullet[i].x, Enemy2Bullet[i].y, 。);
}
// 적군 총알 출력
void eraseEnemy1Bullet(int i)
{
	printxy(Enemy1Bullet[i].x, Enemy1Bullet[i].y, 　);
}
void eraseEnemy2Bullet(int i)
{
	printxy(Enemy2Bullet[i].x, Enemy2Bullet[i].y, 　);
}
// 지나간 적군 총알 지움
void moveEnemy1Bullet()
{
	int i;

	for (i = 0; i< MAXENEMYBULLET; i++){
		if (Enemy1Bullet[i].exist == FALSE)
			continue;

		if (--Enemy1Bullet[i].nStay == 0){
			Enemy1Bullet[i].nStay = Enemy1Bullet[i].nFrame;
			eraseEnemy1Bullet(i);

			if (Enemy1Bullet[i].y > 40)
				Enemy1Bullet[i].exist = FALSE;

			else{
				if (random(2)){
					if (Enemy1Bullet[i].x < px)
						Enemy1Bullet[i].x++;
					else
						Enemy1Bullet[i].x--;
				}// if

				Enemy1Bullet[i].y++;

				drawEnemy1Bullet(i);
			} //else
		}//if
	}//for
}
void moveEnemy2Bullet()
{
	int i;

	for (i = 0; i< MAXENEMYBULLET; i++){
		if (Enemy2Bullet[i].exist == FALSE)
			continue;

		if (--Enemy2Bullet[i].nStay == 0){
			Enemy2Bullet[i].nStay = Enemy2Bullet[i].nFrame;
			eraseEnemy2Bullet(i);

			if (Enemy2Bullet[i].y > 40)
				Enemy2Bullet[i].exist = FALSE;

			else{
				if (random(2)){
					if (Enemy2Bullet[i].x < px)
						Enemy2Bullet[i].x++;
					else
						Enemy2Bullet[i].x--;
				}// if

				Enemy2Bullet[i].y++;

				drawEnemy2Bullet(i);
			} //else
		}//if
	}//for
}
// 적군 총알 움직임
void enemy1Crash()
{
	int i;

	for (i = 0; i < MAXENEMY; i++){
		if ((Enemy1[i].exist == FALSE) || (Enemy1[i].type == -1))
			continue;


		if ((Enemy1[i].y == Bullet[i].y) && (abs(Enemy1[i].x + 3 - Bullet[i].x) <= 7)){
			kill++;
			combo++;
			printxy(Bullet[i].x, Bullet[i].y, );
			Bullet[i].exist = FALSE;
			Enemy1[i].type = -1;

			setColor(YELLOW);
			printxy(Enemy1[i].x - 1, Enemy1[i].y - 1, ++++ + );
			printxy(Enemy1[i].x - 1, Enemy1[i].y, +10점 + );
			printxy(Enemy1[i].x - 1, Enemy1[i].y + 1, ++++ + );
			score += 10;
			Enemy1[i].nFrame = Enemy1[i].nCount = 50;
			break;
		}

	}
}
void enemy2Crash()
{
	int i;

	for (i = 0; i < MAXENEMY; i++){
		if ((Enemy2[i].exist == FALSE) || (Enemy2[i].type == -1))
			continue;


		if ((Enemy2[i].y == Bullet[i].y) && (abs(Enemy2[i].x + 3 - Bullet[i].x) <= 7)){
			kill++;
			combo++;
			printxy(Bullet[i].x, Bullet[i].y, );
			Bullet[i].exist = FALSE;
			Enemy2[i].type = -1;

			setColor(YELLOW);
			printxy(Enemy2[i].x - 1, Enemy2[i].y - 1, ++++ + );
			printxy(Enemy2[i].x - 1, Enemy2[i].y, +10점 + );
			printxy(Enemy2[i].x - 1, Enemy2[i].y + 1, ++++ + );
			score += 10;
			Enemy2[i].nFrame = Enemy2[i].nCount = 50;
			break;
		}

	}
}
// 적군 피격판정
void playerCrash()
{
	int i;

	for (i = 0; i < MAXENEMYBULLET; i++){
		if (Enemy1Bullet[i].exist == FALSE)
			continue;

		if (Enemy1Bullet[i].y == py && abs(Enemy1Bullet[i].x - (px + 3)) <= 3){
			score += comboBonus(combo);
			combo = 0;
			printxy(Enemy1Bullet[i].x, Enemy1Bullet[i].y, );
			Enemy1Bullet[i].exist = FALSE;
			playerHealth--;
			printf("\a");
		}// outer if
	}// for

	for (i = 0; i < MAXENEMYBULLET; i++){
		if (Enemy2Bullet[i].exist == FALSE)
			continue;

		if (Enemy2Bullet[i].y == py && abs(Enemy2Bullet[i].x - (px + 3)) <= 3){
			score += comboBonus(combo);
			combo = 0;
			printxy(Enemy2Bullet[i].x, Enemy2Bullet[i].y, );
			Enemy2Bullet[i].exist = FALSE;
			playerHealth--;
			printf("\a");
		}// outer if
	}// for

	for (i = 0; i < MAXENEMYBULLET; i++){
		if (bossBullet1[i].exist == FALSE)
			continue;

		if (bossBullet1[i].y == py && abs(bossBullet1[i].x - (px + 3)) <= 3){
			score += comboBonus(combo);
			combo = 0;
			printxy(bossBullet1[i].x, bossBullet1[i].y, );
			bossBullet1[i].exist = FALSE;
			playerHealth--;
			printf("\a");
		}// outer if
	}// for

	for (i = 0; i < MAXENEMYBULLET; i++){
		if (bossBullet2[i].exist == FALSE)
			continue;

		if (bossBullet2[i].y == py && abs(bossBullet2[i].x - (px + 3)) <= 3){
			score += comboBonus(combo);
			combo = 0;
			printxy(bossBullet2[i].x, bossBullet2[i].y, );
			bossBullet2[i].exist = FALSE;
			playerHealth--;
			printf("\a");
		}// outer if
	}// for
}
// 플레이어 피격판정
void reset(){
	int i;

	for (i = 0; i<MAXENEMYBULLET; i++){
		Enemy1Bullet[i].exist = FALSE;
		Enemy2Bullet[i].exist = FALSE;
	}
	for (i = 0; i<MAXBULLET; i++)
		Bullet[i].exist = FALSE;
	for (i = 0; i<MAXENEMYBULLET; i++){
		Enemy1[i].exist = FALSE;
		Enemy2[i].exist = FALSE;
	}
	playerHealth = 4;
	score = 0;
	px = 30;
	py = 39;
	kill = 0;
	diff = 3;
}
//플레이어 총알, 적군 총알, 적군 초기화 (모든 초기 변수 초기화)
void semiReset(){
	int i;

	for (i = 0; i<MAXENEMYBULLET; i++){
		Enemy1Bullet[i].exist = FALSE;
		Enemy2Bullet[i].exist = FALSE;
	}
	for (i = 0; i<MAXBULLET; i++)
		Bullet[i].exist = FALSE;
	for (i = 0; i<MAXENEMYBULLET; i++){
		Enemy1[i].exist = FALSE;
		Enemy2[i].exist = FALSE;
		Enemy1[i].y = 0;
		Enemy2[i].y = 0;
	}
	px = 30;
	py = 39;
}
//적군, 총알, 플레이어만 리셋
void printGameTitle(int x, int y){
	int i;
	x = x + 1;
	i = y;
	setColor(WHITE);
	printxy(x, i, )Sleep(WAITTIME);
	printxy(x, i + 1, )Sleep(WAITTIME);
	printxy(x, i + 2,　■■■　　■　　■　　■■　　　■■　　■■■■■　　)Sleep(WAITTIME);
	printxy(x, i + 3,■　　　■　■　　■　■　　■　■　　■　　　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 4,■　　　　　■■■■　■　　■　■　　■　　　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 5,　■■■　　■　　■　■　　■　■　　■　　　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 6,　　　　■　■　　■　　■■　　　■■　　　　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 7,　　　　■　■　　■　　　　　  　　　　　　　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 8,　■■■　　　　　　　　　　　　　　　　　　　■　　　　)Sleep(WAITTIME);
	setColor(YELLOW);
	printxy(x, i + 9, 　　　　　　　　　　■■■　　　　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 10,　　　　　　　　　■　　　■　　　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 11,　　　　　　　　　■　　　■　　　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 12,　　　　　　　　　 ■■■　 ■　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 13,　　　　　　　　　■　　 ■ ■　　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 14,　　　　　　　　　■　　　■　　　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 15,　　　　　　　　　　■■■　■　　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 16,　　　　　　　　　　　　　　　■　　　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 17, )Sleep(WAITTIME);

	setColor(WHITE);

	printxy(x, i + 18, ■■■　　　　　　　　　　　　　　■■■　　　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 19, ■　　■　　　■■　　■■■　　■　　　■　　■■■　　)Sleep(WAITTIME);
	printxy(x, i + 20, ■　　　■　■　　■　■　　■　■　　　■　　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 21, ■　　　■　■　　■　■　　■　■　　　　　　■■■　　)Sleep(WAITTIME);
	printxy(x, i + 22, ■　　　■　■　　■　■　　■　■　■■■■　■　　　　)Sleep(WAITTIME);
	printxy(x, i + 23, ■　　■　　　■■　　■■■　　■　　　■■　■■■　　)Sleep(WAITTIME);
	printxy(x, i + 24, ■■■　　　　　　　　　　　　　　■■■　■　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 25, 　　　　　　)Sleep(WAITTIME);
	printxy(x, i + 26, )Sleep(WAITTIME);
}
//게임제목표시
void playerDie() {
	//FMOD_Channel_Stop( channel );
	PlaySound(TEXT("Boom.wav"), NULL, SND_ASYNC);

	setColor(RED);
	printxy(px, py, @@@@@@);
	printxy(px, py + 1, @BANG@);
	printxy(px, py + 2, @@@@@@);
	Sleep(300);

	setColor(YELLOW);
	printxy(px - 2, py - 1, @@@@@@@@@@);
	printxy(px - 2, py, @　　　　@);
	printxy(px - 2, py + 1, @　　　　@);
	printxy(px - 2, py + 2, @　　　　@);
	printxy(px - 2, py + 3, @@@@@@@@@@);
	Sleep(2000);

	system("cls");
}//사망시 액션
//플레이어 사망 액션
void GAMEOVER() {
	system("cls");
	setColor(WHITE);
	printxy(0, 12, ================================== GAME OVER =================================== );
	setColor(SKYBLUE);
	gotoxy(32, 14);
	printf("Score = %d점", score);
	gotoxy(28, 16);
	printf("Play Time = %2d 분 %2d초", (int)playtime / 60, (int)playtime % 60);
	setColor(WHITE);
	printxy(0, 32, ================================================================================ );
}
//게임오버 메세지	
void rankPrint(struct players list[]) {

	int j;
	int k;
	char ctmp[20];
	int tmp;
	double dtmp;

	for (j = 0; j < LIST; j++) {
		for (k = 0; k < LIST - j; k++) {
			if (list[k].score < list[k + 1].score) {

				strcpy(ctmp, list[k].name);
				strcpy(*list[k].name, list[k + 1].name);
				strcpy(list[k + 1].name, ctmp);

				tmp = list[k].score;
				list[k].score = list[k + 1].score;
				list[k + 1].score = tmp;

				dtmp = list[k].time;
				list[k].time = list[k + 1].time;
				list[k + 1].time = dtmp;
			}
		}
	}

	for (j = 0; j < 3; j++) {
		gotoxy(20, 18);
		printf("점수 기록");
		Sleep(200);
		gotoxy(20, 18);
		printf("        ");
		Sleep(200);
	}

	gotoxy(20, 18);
	printf("점수 기록");

	Sleep(250);

	for (j = 0; j < 10; j++) {
		gotoxy(10, 20 + j);
		printf("%2d위 : %s ", j + 1, list[j].name);
		Sleep(250);
	}

	for (j = 0; j < 10; j++) {
		gotoxy(30, 20 + j);
		printf("%3d 점", list[j].score);
		Sleep(250);
	}

	for (j = 0; j < LIST; j++) {
		for (k = 0; k < LIST - j; k++) {
			if (list[k].time < list[k + 1].time) {
				strcpy(ctmp, list[k].name);
				strcpy(list[k].name, list[k + 1].name);
				strcpy(list[k + 1].name, ctmp);

				tmp = list[k].score;
				list[k].score = list[k + 1].score;
				list[k + 1].score = tmp;

				dtmp = list[k].time;
				list[k].time = list[k + 1].time;
				list[k + 1].time = dtmp;
			}
		}
	}

	for (j = 0; j < 3; j++) {
		gotoxy(50, 18);
		printf("시간 기록");
		Sleep(200);
		gotoxy(50, 18);
		printf("        ");
		Sleep(200);
	}
	gotoxy(50, 18);
	printf("시간 기록");
	Sleep(250);

	for (j = 0; j < 10; j++) {
		gotoxy(40, 20 + j);
		printf("%2d위 : %s ", j + 1, list[j].name);
		Sleep(250);
	}

	for (j = 0; j < 10; j++) {
		gotoxy(60, 20 + j);
		printf("%2d분 %2d초", (int)list[j].time / 60, (int)list[j].time % 60);
		Sleep(250);
	}
}
//랭킹 출력
int comboBonus(int combo){
	int i;
	int bonus;
	int combo1 = 0;
	int combo2 = 0;
	int combo3 = 0;
	int combo4 = 0;
	int combo5 = 0;

	for (i = 0; i<combo; i++){
		if (i >= 9 && i<29)
			combo1 += 5;
		else if (i >= 29 && i<49)
			combo2 += 10;
		else if (i >= 49 && i<79)
			combo3 += 15;
		else if (i >= 79 && i<109)
			combo4 += 20;
		else if (i >= 109)
			combo5 += 25;
	}

	bonus = combo1 + combo2 + combo3 + combo4 + combo5;

	return bonus;
}
//콤보보너스
/*void Init()
{
FMOD_System_Create(&g_System);
FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL,  NULL);
FMOD_System_CreateSound( g_System, "BGM.wav", FMOD_DEFAULT, 0, &g_Sound);
}
//BGM삽입
void Release()
{
FMOD_Sound_Release(g_Sound);
FMOD_System_Close(g_System);
FMOD_System_Release(g_System);
}
//BGM메모리 해제*/
void drawBoss(){
	int i;

	while (boss.exist != TRUE){

		boss.x = 38;
		boss.y = 6;
		if (random(2) == 0)
			boss.dest = 1;
		else
			boss.dest = -1;

		boss.ydest = 1;

		boss.nFrame = boss.nCount = 2; // 기본 속도 4 추가속도 (0~2)
		boss.yFrame = boss.yCount = 100;
		boss.exist = TRUE;
	}
}
// 보스 생성 / bossmoveaction 에서 
void bossMoveAction(){

	if (--boss.nCount == 0)
	{
		boss.nCount = boss.nFrame;

		if (boss.x > 60 || boss.x < 20)
		{
			boss.dest = (-1)*boss.dest;
			boss.x += boss.dest;
		} //inner if
		else
		{
			boss.x += boss.dest;
			setColor(DARK_YELLOW);
			gotoxy(boss.x - 3, boss.y - 1);   printf("  ■■■■■■  ");
			gotoxy(boss.x - 3, boss.y);       printf("  ■  ■■  ■  ");
			gotoxy(boss.x - 3, boss.y + 1);   printf("    ■■■■  ");

		}//inner else
	}//outer if

	if (--boss.yCount == 0)
	{
		boss.yCount = boss.yFrame;

		if (boss.x > 60 || boss.x < 20)
		{
			boss.ydest = 0;
		} //inner if
		else
		{
			boss.y += boss.ydest;
			setColor(DARK_YELLOW);
			gotoxy(boss.x - 3, boss.y - 2);   printf("                ");
			gotoxy(boss.x - 3, boss.y - 1);   printf("  ■■■■■■  ");
			gotoxy(boss.x - 3, boss.y);       printf("  ■  ■■  ■  ");
			gotoxy(boss.x - 3, boss.y + 1);   printf("    ■■■■  ");

		}//inner else
	}//outer if
	bossPattern1();
	bossPattern2();

}
// 보스 움직임
void bossPattern1(){
	int j;

	if (random(30) == 0)
	{
		for (j = 0; j < MAXENEMYBULLET && bossBullet1[j].exist == TRUE; j++);
		if (j != MAXENEMYBULLET)
		{
			bossBullet1[j].x = boss.x + 5 + (j - 10);
			bossBullet1[j].y = boss.y + 1;
			bossBullet1[j].nFrame = bossBullet1[j].nStay = 2 + random(3);
			bossBullet1[j].exist = TRUE;
		}
	}
}
void bossPattern2(){
	int j;

	if (random(30) == 0)
	{
		for (j = 0; j < MAXENEMYBULLET && bossBullet2[j].exist == TRUE; j++);
		if (j != MAXENEMYBULLET)
		{
			bossBullet2[j].x = boss.x + 5 + (j - 10);
			bossBullet2[j].y = boss.y + 1;
			bossBullet2[j].nFrame = bossBullet2[j].nStay = 2 + random(3);
			bossBullet2[j].exist = TRUE;
		}
	}
}
//보스 총알 패턴
void drawBossBullet1(int i)
{
	setColor(YELLOW);
	printxy(bossBullet1[i].x - 5, bossBullet1[i].y, 。);

}
void drawBossBullet2(int i)
{
	setColor(YELLOW);
	printxy(bossBullet2[i].x + 5, bossBullet2[i].y, 。);

}
//보스총알 출력
void eraseBossBullet1(int i)
{
	printxy(bossBullet1[i].x - 5, bossBullet1[i].y, 　);
}
void eraseBossBullet2(int i)
{
	printxy(bossBullet2[i].x + 5, bossBullet2[i].y, 　);
}
//보스총알 지움
void moveBossBullet1()
{
	int i;

	for (i = 0; i< MAXENEMYBULLET; i++){
		if (bossBullet1[i].exist == FALSE)
			continue;

		if (--bossBullet1[i].nStay == 0){
			bossBullet1[i].nStay = bossBullet1[i].nFrame;

			eraseBossBullet1(i);

			if (bossBullet1[i].y > 40)
				bossBullet1[i].exist = FALSE;

			else{
				if (random(2)){
					if (bossBullet1[i].x < px)
						bossBullet1[i].x++;
					else
						bossBullet1[i].x--;
				}// if

				bossBullet1[i].y++;

				drawBossBullet1(i);
			} //else
		}//if
	}//for
}
void moveBossBullet2()
{
	int i;

	for (i = 0; i< MAXENEMYBULLET; i++){
		if (bossBullet2[i].exist == FALSE)
			continue;

		if (--bossBullet2[i].nStay == 0){
			bossBullet2[i].nStay = bossBullet2[i].nFrame;

			eraseBossBullet2(i);

			if (bossBullet2[i].y > 40)
				bossBullet2[i].exist = FALSE;

			else{
				if (random(2)){
					if (bossBullet2[i].x < px)
						bossBullet2[i].x++;
					else
						bossBullet2[i].x--;
				}// if

				bossBullet2[i].y++;

				drawBossBullet2(i);
			} //else
		}//if
	}//for
}
//보스 총알 움직임
void bossCrash(){
	int i;

	for (i = 0; i<MAXBULLET; i++){
		if (boss.y == Bullet[i].y && abs(boss.x + 3 - Bullet[i].x) <= 7){
			combo++;
			printxy(Bullet[i].x, Bullet[i].y, );
			Bullet[i].exist = FALSE;
			Bullet[i].y = 40;
			setColor(YELLOW);
			score += 10;

			bossHealth--;
		}
	}
}
//보스 피격판정
//========================================================================
void main() {
	int i = 0;

	setcursortype(NOCURSOR);  //커서 없앰
	system("mode con: cols=80 lines=50"); //콘솔창 크기 지정 x = 80, y = 50
	//Init();
	
	
	title_a();
	
	
	while (1) { //게임 반복

		system("cls");
		//FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound,0, &channel);
		PlaySound(TEXT("BGM.wav"), NULL, SND_ASYNC || SND_LOOP);
		
		setColor(WHITE);
		loadingStage();		//로딩스테이지 불러오기

		system("cls");
		randomize();				// 난수 발생기 초기화
		start = clock();
		reset();

		
		while (1) { //스테이지1
			statusBar();				// 상태바 출력

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}
			if (kill >= 10){
				system("cls");
				gotoxy(40, 25); printf("스테이지2");
				Sleep(2000);
				break;
			}
			drawPlayer();
			playerMoveAction();
			playerBulletAction();

			drawEnemy1();
			drawEnemy2();
			moveEnemy1Bullet();
			moveEnemy2Bullet();
			enemy1MoveAction();
			enemy2MoveAction();

			playerCrash();
			enemy1Crash();
			enemy2Crash();

			statusBar();

			Sleep(GAMESPEED);//게임속도 조절
		}// 스테이지1

		if (playerHealth>0)
			semiReset();

		diff = 2;  //난이도 설정

		while (1) {//스테이지2

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}

			if (kill >= 20){
				system("cls");
				gotoxy(40, 25); printf("스테이지3");
				Sleep(2000);
				break;
			}

			statusBar();
			drawPlayer();
			playerMoveAction();
			playerBulletAction();

			drawEnemy1();
			drawEnemy2();
			moveEnemy1Bullet();
			moveEnemy2Bullet();
			enemy1MoveAction();
			enemy2MoveAction();

			playerCrash();
			enemy1Crash();
			enemy2Crash();

			statusBar();

			Sleep(GAMESPEED);//게임속도 조절
		}// 스테이지 2

		if (playerHealth>0)
			semiReset();
		diff = 1;  //난이도 설정

		while (1) {//스테이지 3

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}

			if (kill >= 30){
				system("cls");
				gotoxy(40, 25); printf("보스");
				Sleep(2000);
				break;
			}

			statusBar();
			drawPlayer();
			playerMoveAction();
			playerBulletAction();

			drawEnemy1();
			drawEnemy2();
			moveEnemy1Bullet();
			moveEnemy2Bullet();
			enemy1MoveAction();
			enemy2MoveAction();

			playerCrash();
			enemy1Crash();
			enemy2Crash();

			statusBar();

			Sleep(GAMESPEED);//게임속도 조절
		}// 스테이지 3

		if (playerHealth>0)
			semiReset();

		while (1){//보스
			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}

			statusBar();
			drawPlayer();
			playerMoveAction();
			playerBulletAction();

			drawBoss();
			bossMoveAction();

			bossPattern1();
			bossPattern2();

			moveBossBullet1();
			moveBossBullet2();

			bossCrash();
			playerCrash();

			gotoxy(2, 4);
			printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");


			if (bossHealth <= 0){
				system("cls");
				gotoxy(40, 25); printf("무한모드");
				Sleep(2000);
				break;
			}

			statusBar();

			Sleep(GAMESPEED);//게임속도 조절
		}

		while (1) {//무한모드

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}

			statusBar();
			drawPlayer();
			playerMoveAction();
			playerBulletAction();

			drawEnemy1();
			drawEnemy2();
			moveEnemy1Bullet();
			moveEnemy2Bullet();
			enemy1MoveAction();
			enemy2MoveAction();

			playerCrash();
			enemy1Crash();
			enemy2Crash();

			statusBar();

			Sleep(GAMESPEED);//게임속도 조절
		}//무한모드

		playerDie();

		fflush(stdin);
		setColor(WHITE);
		printxy(28, 12, 이름을 입력하세요 : );
		gets(list[i].name);

		list[i].score = score;
		list[i].time = playtime;

		GAMEOVER();

		rankPrint(list);

		i++;


		getch();
		system("cls");
		system("pause > NULL");
	}//while
	//Release();
} //main