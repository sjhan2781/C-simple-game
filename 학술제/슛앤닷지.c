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
//Ŀ�� Ÿ�� ����

#define TIME 60
#define PRINTSPEED 45 // ��������Ƽ ��� �ӵ�
#define ESC 27 // ESC �� �ƽ�Ű �ڵ�
#define printxy(x,y,str) {gotoxy(x,y); printf(#str"");} //printxy �Լ� ����
#define map_YMAX 39 // �ִ� �̵����� y ��
#define MAXBULLET 5 // �ִ� �Ѿ� ����
#define EXTENDASCII -32 // Ȯ�� �ƽ�Ű �ڵ�
#define MAXENEMY 5 // �ִ� �� ���� ī��Ʈ
#define MAXENEMYBULLET 50 //�ִ� ���� �Ѿ˰���
#define randomize() srand((unsigned)time(NULL)) //���� �߻��� �ʱ�ȭ
#define random(n) (rand() % (n)) // 0~n ������ ���� �߻�
#define LIST 30 //��ŷ �ִ� �ο�
#define GAMESPEED 16 //���Ӽӵ�
#define WAITTIME 50//��������â���ڼӵ�

char *VERSION = "1.3.0";
// ��ġ��Ʈ
//1.0.1 BGM����. ����� ���� ����
//1.0.2 �ǰ� ���� ������ ����, ĳ���� �پ缺 ����
//1.0.3 ���� �ݺ� ����. ����� ���� ����
//1.0.4 ��ŷ ����. ����� ���� ����
//1.0.5 �׷��� ��ħ ���� �ذ�
//1.0.6 ������ �÷���Ÿ�� ��� ����
//1.1.0 ����, �ΰ� ����
//1.1.1 ü�¿��� ����. ����� ���� ����
//1.1.2 �޺�����
//1.1.3 ������ �Ѿ� ���� ��ġ ���� ����
//1.2.0 �ǰ� ���� ���� ���� ����
//1.2.1 �޺� ���ʽ� ���� �߰�
//1.2.2 ���� �뷱�� ��ġ. ����� ���� ����
//1.2.3 �ι�° �������� ����
//1.2.4 ����° ��������, ���Ѹ�� ����
//1.2.5 ���������� ���̵� �뷱�� ��ġ
//1.3.0 ���� BGM ����� ȿ���� ����
//=========================================================================
// ���� ���� ����
int diff = 3;
//���̵�
int bulletspeed = 2;
// ��� �Ѿ� �ӵ�
int combo = 0;
// �޺�
int kill = 0;
// ų ��
int comboBonusSave;
// �޺�, �޺� ���ʽ� ���� ���
int clearScore = 200;
//Ŭ���� ����
int stageNumber = 1;
//�������� ��
int playerHealth = 4;
//�÷��̾� ü��
int px = 30;
int py = 39;
// �÷��̾� ��ġ
int bx = -1, by;
// �Ѿ� ��ġ
int score = 0;
// ����
char *arEnemy[] = { " ���ߡޡߢ� ", " ��-��-�� ", " ��M��M�� ", " �ġ���� " };
// ���� ��� 
clock_t start, finish;

double playtime;

int bossHealth = 10;

/*FMOD_SYSTEM *g_System;
FMOD_SOUND  *g_Sound;
FMOD_CHANNEL *channel = NULL;*/

//=========================================================================
// ������ ���� ����
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
//�� ������ ����
struct
{
	BOOL exist;
	int x, y;
}Bullet[MAXBULLET];
// ����ü [�÷��̾� �Ѿ�] 
typedef struct enemy
{
	BOOL exist;
	int type;
	int x, y;
	int dest;
	int nFrame;
	int nCount;

}ENEMY;
// ����ü [����]

typedef struct enemybullet
{
	BOOL exist;
	int x, y;
	int nFrame;
	int nStay;
}ENEMYBULLET;
// ����ü [���� �Ѿ�]
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
// ����ü [����]
typedef struct bossbullet
{
	BOOL exist;
	int x, y;
	int nFrame;
	int nStay;
}BOSSBULLET;
// ����ü [���� �Ѿ�]

ENEMY Enemy1[MAXENEMY], Enemy2[MAXENEMY];

ENEMYBULLET Enemy1Bullet[MAXENEMYBULLET], Enemy2Bullet[MAXENEMYBULLET];

BOSS boss;
BOSSBULLET bossBullet1[MAXENEMYBULLET], bossBullet2[MAXENEMYBULLET];

PLAYER list[LIST] = { "AAA", 0, 0 };
//=========================================================================
// �Լ� ���� ����

void gotoxy(int x, int y); // Ŀ�� �̵�
void setcursortype(CURSOR_TYPE c); //Ŀ�� Ÿ�� ����
void setColor(int color); //���� �ֱ�
void loadingStage(); //���� ����ȭ��
void drawPlayer(); //�÷��̾� ���
void erasePlayer(); // ������ �÷��̾� �����
void playerMoveAction(); // �÷��̾� �̵� 
void statusBar(); // ���¹� ����
void drawBullet(int i); //�Ѿ� ���
void eraseBullet(int i); // ������ �Ѿ� �����
void playerBulletAction(); // �Ѿ� ������ ����
void drawEnemy1(); // ���� ���
void drawEnemy2();
void enemy1MoveAction(); //���� ������ ����
void enemy2MoveAction();
void enemy1BulletAction(int i); // ���� �Ѿ� (�ӵ�/��ǥ)
void enemy2BulletAction(int i);
void eraseEnemy1Bullet(int i); // ������ ���� �Ѿ� ����
void eraseEnemy2Bullet(int i);
void moveEnemy1Bullet(); // ���� �Ѿ� ������
void moveEnemy2Bullet();
void enemy1Crash(); //���� �ǰ� ����
void enemy2Crash();
void playerCrash(); // �÷��̾� �ǰ� ����
void reset(); //���� �ʱ�ȭ
void printGameTitle(int x, int y);//���� ���� ���
void rankPrint(struct player list[]); // ��ŷ ���
void GAMEOVER(); //���ӿ��� �޼��� ���
int comboBonus(int combo); //�޺����ʽ�
//void InIt();  //BGM����
//void Release(); //BGM�޸� ����
void drawBoss(); // ���� ���� / bossmoveaction ����
void bossMoveAction(); // ���� ������
void bossPattern1(); //���� �Ѿ� ����1
void bossPattern2();
void drawBossBullet1(int i); //�����Ѿ� ���
void drawBossBullet2(int i);
void eraseBossBullet1(int i); //�����Ѿ� ����
void eraseBossBullet2(int i);
void moveBossBullet1(); //���� �Ѿ� ������
void moveBossBullet2();
void bossCrash(); //���� �ǰ�����
//=====================================================================

// �Լ� ���� ����
void title_a() {
	int i;

	Sleep(800);
	setColor(YELLOW);

	for (i = 0; i < 42; i += 2) {
		gotoxy(i, 8); printf("��"); gotoxy(80 - i, 8); printf("��");
		gotoxy(i, 9); printf("��"); gotoxy(80 - i, 9); printf("��");
		gotoxy(i, 39); printf("��"); gotoxy(80 - i, 39); printf("��");
		gotoxy(i, 40); printf("��"); gotoxy(80 - i, 40); printf("��");
		Sleep(20);
	}

	setColor(WHITE);
	PlaySound(TEXT("Almighty.wav"), NULL, SND_ASYNC);

	gotoxy(1, 16); printf("        ��");
	gotoxy(1, 17); printf("      ��  ");
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

	gotoxy(1, 16); printf("        ��� ");
	gotoxy(1, 17); printf("      ��     ");
	gotoxy(1, 18); printf("      ��     ");
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


	gotoxy(1, 16); printf("        ���� ");
	gotoxy(1, 17); printf("      ��     ");
	gotoxy(1, 18); printf("      ��     ");
	gotoxy(1, 19); printf("      ��");
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

	gotoxy(1, 16); printf("        ����      ��");
	gotoxy(1, 17); printf("      ��      ��");
	gotoxy(1, 18); printf("      ��      ");
	gotoxy(1, 19); printf("      ��");
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

	gotoxy(1, 16); printf("        ����      �� ");
	gotoxy(1, 17); printf("      ��      ��    �� ");
	gotoxy(1, 18); printf("      ��      ��");
	gotoxy(1, 19); printf("      ��� ");
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

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    �� ");
	gotoxy(1, 18); printf("      ��      ��    ��   ");
	gotoxy(1, 19); printf("      ���� ");
	gotoxy(1, 20); printf("      �� ");
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

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ");
	gotoxy(1, 19); printf("      �����      �� ");
	gotoxy(1, 20); printf("      ��      ��");
	gotoxy(1, 21); printf("      �� ");
	gotoxy(1, 22); printf(" ");
	gotoxy(1, 25); printf(" ");
	gotoxy(1, 26); printf("");
	gotoxy(1, 27); printf("  ");
	gotoxy(1, 28); printf("  ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf("  ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��  ");
	gotoxy(1, 20); printf("      ��      ��    ��");
	gotoxy(1, 21); printf("      ��      �� ");
	gotoxy(1, 22); printf("      ��     ");
	gotoxy(1, 25); printf(" ");
	gotoxy(1, 26); printf("  ");
	gotoxy(1, 27); printf(" ");
	gotoxy(1, 28); printf("");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf("");
	gotoxy(1, 31); printf("");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         �� ");
	gotoxy(1, 20); printf("      ��      ��    �� ");
	gotoxy(1, 21); printf("      ��      ��    �� ");
	gotoxy(1, 22); printf("      ��      ��    ");
	gotoxy(1, 25); printf("  ");
	gotoxy(1, 26); printf(" ");
	gotoxy(1, 27); printf(" ");
	gotoxy(1, 28); printf("  ");
	gotoxy(1, 29); printf("");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf("");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��  ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    �� ");
	gotoxy(1, 22); printf("      ��      ��    ��  ");
	gotoxy(1, 25); printf("             ��� ");
	gotoxy(1, 26); printf("             ��");
	gotoxy(1, 27); printf("                ");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��       ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    ���         ");
	gotoxy(1, 25); printf("             ���    ��");
	gotoxy(1, 26); printf("             �� ��  ");
	gotoxy(1, 27); printf("             ��   ");
	gotoxy(1, 28); printf(" ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��     ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    ����     ��");
	gotoxy(1, 25); printf("             ���    ��� ");
	gotoxy(1, 26); printf("             �� ��  �� ��");
	gotoxy(1, 27); printf("             ��   ��   ");
	gotoxy(1, 28); printf("             �� ");
	gotoxy(1, 29); printf(" ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           ��   ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   ���  ");
	gotoxy(1, 25); printf("             ���    ���       ��");
	gotoxy(1, 26); printf("             �� ��  �� ��  ");
	gotoxy(1, 27); printf("             ��   ��   ��");
	gotoxy(1, 28); printf("             ��  ");
	gotoxy(1, 29); printf("             �� ");
	gotoxy(1, 30); printf(" ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           ��� ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   ����");
	gotoxy(1, 25); printf("             ���    ���       ���        �� ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��  ");
	gotoxy(1, 27); printf("             ��   ��   ��  ");
	gotoxy(1, 28); printf("             ��        ��  ");
	gotoxy(1, 29); printf("             ��  ");
	gotoxy(1, 30); printf("             ��  ");
	gotoxy(1, 31); printf(" ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           ����");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       ����        ���   ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��   ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ");
	gotoxy(1, 28); printf("             ��        �� ");
	gotoxy(1, 29); printf("             ��        ��  ");
	gotoxy(1, 30); printf("             ��        �� ");
	gotoxy(1, 31); printf("             ��        ");
	Sleep(TIME);


	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           ����� ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ����          ��");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��  ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��                      ");
	gotoxy(1, 28); printf("             ��        ��          �� ");
	gotoxy(1, 29); printf("             ��        ��  ");
	gotoxy(1, 30); printf("             ��        �� ");
	gotoxy(1, 31); printf("             ��        ��  ");
	Sleep(TIME);


	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           ������ ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      �����      ��� ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��            ��         ��  ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             �� ");
	gotoxy(1, 28); printf("             ��        ��          �� ");
	gotoxy(1, 29); printf("             ��        ��          �� ");
	gotoxy(1, 30); printf("             ��        ��  ");
	gotoxy(1, 31); printf("             ��        ��    ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           ������� ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      ����");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��  ");
	gotoxy(1, 29); printf("             ��        ��          ��   ");
	gotoxy(1, 30); printf("             ��        ��          ��  ");
	gotoxy(1, 31); printf("             ��        ��   �� ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           �������                             ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      �����  ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��   ");
	gotoxy(1, 29); printf("             ��        ��          ��             ��    ");
	gotoxy(1, 30); printf("             ��        ��          ��  ");
	gotoxy(1, 31); printf("             ��        ��   ��  ��");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           �������                             ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      �����  ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��          ���");
	gotoxy(1, 29); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 30); printf("             ��        ��          ��             ��  ");
	gotoxy(1, 31); printf("             ��        ��   ��  ���");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           �������                             ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      �����  ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��          ���� ");
	gotoxy(1, 29); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 30); printf("             ��        ��          ��             ��                    ");
	gotoxy(1, 31); printf("             ��        ��   ��  �����          �� ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           �������                             ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      �����  ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��          �����    ");
	gotoxy(1, 29); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 30); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 31); printf("             ��        ��   ��  �����    ��    ��    ��    ��");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           �������                             ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      �����  ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��          �����    ");
	gotoxy(1, 29); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 30); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 31); printf("             ��        ��   ��  �����    ��    ��    ��    ���  ");
	Sleep(TIME);

	gotoxy(1, 16); printf("        ����      ��         ��               ");
	gotoxy(1, 17); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 18); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 19); printf("      ������    ��         ��           �������                             ");
	gotoxy(1, 20); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 21); printf("      ��      ��    ��         ��                      ");
	gotoxy(1, 22); printf("      ��      ��    �����   �����         ");
	gotoxy(1, 25); printf("             ���    ���       �����      ������      �����  ");
	gotoxy(1, 26); printf("             �� ��  �� ��          ��             ��          ��          ");
	gotoxy(1, 27); printf("             ��   ��   ��          ��             ��          ��          ");
	gotoxy(1, 28); printf("             ��        ��          ��             ��          �����    ");
	gotoxy(1, 29); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 30); printf("             ��        ��          ��             ��          ��          ");
	gotoxy(1, 31); printf("             ��        ��   ��  �����    ��    ��    ��    �����  ");
	Sleep(TIME);

	Sleep(500);
	printxy(52, 41, BY JSM & HSJ & LJH & LMS);
	PlaySound(NULL, 0, 0);
	Sleep(2200);
}
//Ÿ��Ʋ a
/*void title_b() {
int i;

Sleep(800);
setColor(YELLOW);

for (i = 0; i < 42; i++) {
gotoxy(i, 8); printf("��"); gotoxy(80-i, 8); printf("��");
gotoxy(i, 9); printf("��"); gotoxy(80-i,9); printf("��");
gotoxy(i, 39); printf("��"); gotoxy(80-i, 39); printf("��");
gotoxy(i, 40); printf("��"); gotoxy(80-i, 40); printf("��");
Sleep(22);
}
setColor(WHITE);
PlaySound(TEXT("Almighty.wav"), NULL, SND_ASYNC | SND_LOOP);
gotoxy(1,16); printf("  ��           ");
gotoxy(1,17); printf("��            ");
gotoxy(1,18); printf("��         ");
gotoxy(1,19); printf("���                             ");
gotoxy(1,20); printf("��         ");
gotoxy(1,21); printf("��                ");
gotoxy(1,22); printf("��          ");
gotoxy(1,25); printf("                                                                      ���");
gotoxy(1,26); printf("                   ");
gotoxy(1,27); printf("                      ");
gotoxy(1,28); printf("                                                                      ��� ");
gotoxy(1,29); printf("                        ");
gotoxy(1,30); printf("                      ");
gotoxy(1,31); printf("                                                                      ��� ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����              ");
gotoxy(1, 17); printf("��                         ");
gotoxy(1, 18); printf("��                   ");
gotoxy(1, 19); printf("����                                ");
gotoxy(1, 20); printf("��                         ");
gotoxy(1, 21); printf("��                          ");
gotoxy(1, 22); printf("��                   ");
gotoxy(1, 25); printf("                                                                 ����� ");
gotoxy(1, 26); printf("                                                                 ��");
gotoxy(1, 27); printf("                                                                 ��");
gotoxy(1, 28); printf("                                                                 �����  ");
gotoxy(1, 29); printf("                                                                 �� ");
gotoxy(1, 30); printf("                                                                 ��  ");
gotoxy(1, 31); printf("                                                                 �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����                              ");
gotoxy(1, 17); printf("��      ��                                   ");
gotoxy(1, 18); printf("��      ��                                  ");
gotoxy(1, 19); printf("������                                         ");
gotoxy(1, 20); printf("��      ��                                 ");
gotoxy(1, 21); printf("��      ��                                 ");
gotoxy(1, 22); printf("��      ��           ");
gotoxy(1, 25); printf("                                                                 ����� ");
gotoxy(1, 26); printf("                                                                 ��");
gotoxy(1, 27); printf("                                                                 ��");
gotoxy(1, 28); printf("                                                                 �����  ");
gotoxy(1, 29); printf("                                                                 �� ");
gotoxy(1, 30); printf("                                                                 ��  ");
gotoxy(1, 31); printf("                                                           ��    �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��                       ");
gotoxy(1, 17); printf("��      ��    ��                              ");
gotoxy(1, 18); printf("��      ��    ��                             ");
gotoxy(1, 19); printf("������    ��                                   ");
gotoxy(1, 20); printf("��      ��    ��                               ");
gotoxy(1, 21); printf("��      ��    ��                          ");
gotoxy(1, 22); printf("��      ��    ��        ");
gotoxy(1, 25); printf("                                                       ���      ����� ");
gotoxy(1, 26); printf("                                                                 ��");
gotoxy(1, 27); printf("                                                                 ��");
gotoxy(1, 28); printf("                                                                 �����  ");
gotoxy(1, 29); printf("                                                                 �� ");
gotoxy(1, 30); printf("                                                                 ��  ");
gotoxy(1, 31); printf("                                                           ��    �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��                        ");
gotoxy(1, 17); printf("��      ��    ��                               ");
gotoxy(1, 18); printf("��      ��    ��                               ");
gotoxy(1, 19); printf("������    ��                                       ");
gotoxy(1, 20); printf("��      ��    ��                              ");
gotoxy(1, 21); printf("��      ��    ��                   ");
gotoxy(1, 22); printf("��      ��    �����       ");
gotoxy(1, 25); printf("                                                  ������     ����� ");
gotoxy(1, 26); printf("                                                      ��         ��");
gotoxy(1, 27); printf("                                                      ��         ��");
gotoxy(1, 28); printf("                                                      ��         �����  ");
gotoxy(1, 29); printf("                                                      ��         �� ");
gotoxy(1, 30); printf("                                                      ��         ��  ");
gotoxy(1, 31); printf("                                                      ��   ��    �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��                        ");
gotoxy(1, 17); printf("��      ��    ��                               ");
gotoxy(1, 18); printf("��      ��    ��                               ");
gotoxy(1, 19); printf("������    ��                                       ");
gotoxy(1, 20); printf("��      ��    ��                              ");
gotoxy(1, 21); printf("��      ��    ��                   ");
gotoxy(1, 22); printf("��      ��    �����       ");
gotoxy(1, 25); printf("                                                  ������     ����� ");
gotoxy(1, 26); printf("                                                      ��         ��");
gotoxy(1, 27); printf("                                                      ��         ��");
gotoxy(1, 28); printf("                                                      ��         �����  ");
gotoxy(1, 29); printf("                                                      ��         �� ");
gotoxy(1, 30); printf("                                                      ��         ��  ");
gotoxy(1, 31); printf("                                               ��     ��   ��    �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                      ");
gotoxy(1, 19); printf("������    ��         ��                                      ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   ��         ");
gotoxy(1, 25); printf("                                         ���     ������     ����� ");
gotoxy(1, 26); printf("                                                      ��         ��");
gotoxy(1, 27); printf("                                                      ��         ��");
gotoxy(1, 28); printf("                                                      ��         �����  ");
gotoxy(1, 29); printf("                                                      ��         �� ");
gotoxy(1, 30); printf("                                                      ��         ��  ");
gotoxy(1, 31); printf("                                         ���  ��     ��   ��    �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                      ");
gotoxy(1, 19); printf("������    ��         ��                                      ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   �����  ");
gotoxy(1, 25); printf("                                     �����     ������     ����� ");
gotoxy(1, 26); printf("                                        ��            ��         ��");
gotoxy(1, 27); printf("                                        ��            ��         ��");
gotoxy(1, 28); printf("                                        ��            ��         �����  ");
gotoxy(1, 29); printf("                                        ��            ��         �� ");
gotoxy(1, 30); printf("                                        ��            ��         ��  ");
gotoxy(1, 31); printf("                                     �����  ��     ��   ��    �����");
Sleep(PRINTSPEED);


gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                      ");
gotoxy(1, 19); printf("������    ��         ��                                      ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   �����   ");
gotoxy(1, 25); printf("                                     �����     ������     ����� ");
gotoxy(1, 26); printf("                                        ��            ��         ��");
gotoxy(1, 27); printf("                                        ��            ��         ��");
gotoxy(1, 28); printf("                                        ��            ��         �����  ");
gotoxy(1, 29); printf("                                        ��            ��         �� ");
gotoxy(1, 30); printf("                                        ��            ��         ��  ");
gotoxy(1, 31); printf("                               ��    �����  ��     ��   ��    �����");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                       ");
gotoxy(1, 19); printf("������    ��         ��           ���                            ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   �����         ");
gotoxy(1, 25); printf("                          ��       �����      ������      �����  ");
gotoxy(1, 26); printf("                          ��          ��             ��          ��          ");
gotoxy(1, 27); printf("                          ��          ��             ��          ��          ");
gotoxy(1, 28); printf("                          ��          ��             ��          �����    ");
gotoxy(1, 29); printf("                          ��          ��             ��          ��          ");
gotoxy(1, 30); printf("                          ��          ��             ��          ��          ");
gotoxy(1, 31); printf("                          ��   ��  �����    ��    ��    ��    �����  ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                      ");
gotoxy(1, 19); printf("������    ��         ��           �����                             ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   �����         ");
gotoxy(1, 25); printf("                  ��    ���       �����      ������      �����  ");
gotoxy(1, 26); printf("                   ��  �� ��          ��             ��          ��          ");
gotoxy(1, 27); printf("                     ��   ��          ��             ��          ��          ");
gotoxy(1, 28); printf("                          ��          ��             ��          �����    ");
gotoxy(1, 29); printf("                          ��          ��             ��          ��          ");
gotoxy(1, 30); printf("                          ��          ��             ��          ��          ");
gotoxy(1, 31); printf("                          ��   ��  �����    ��    ��    ��    �����  ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                      ");
gotoxy(1, 19); printf("������    ��         ��           �������                             ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   �����         ");
gotoxy(1, 25); printf("                ���    ���       �����      ������      �����  ");
gotoxy(1, 26); printf("                �� ��  �� ��          ��             ��          ��          ");
gotoxy(1, 27); printf("                ��   ��   ��          ��             ��          ��          ");
gotoxy(1, 28); printf("                ��        ��          ��             ��          �����    ");
gotoxy(1, 29); printf("                ��        ��          ��             ��          ��          ");
gotoxy(1, 30); printf("                ��        ��          ��             ��          ��          ");
gotoxy(1, 31); printf("                ��        ��   ��  �����    ��    ��    ��    �����  ");
Sleep(PRINTSPEED);

gotoxy(1, 16); printf("  ����      ��         ��               ");
gotoxy(1, 17); printf("��      ��    ��         ��                      ");
gotoxy(1, 18); printf("��      ��    ��         ��                      ");
gotoxy(1, 19); printf("������    ��         ��           �������                             ");
gotoxy(1, 20); printf("��      ��    ��         ��                      ");
gotoxy(1, 21); printf("��      ��    ��         ��                      ");
gotoxy(1, 22); printf("��      ��    �����   �����         ");
gotoxy(1, 25); printf("                ���    ���       �����      ������      �����  ");
gotoxy(1, 26); printf("                �� ��  �� ��          ��             ��          ��          ");
gotoxy(1, 27); printf("                ��   ��   ��          ��             ��          ��          ");
gotoxy(1, 28); printf("                ��        ��          ��             ��          �����    ");
gotoxy(1, 29); printf("                ��        ��          ��             ��          ��          ");
gotoxy(1, 30); printf("                ��        ��          ��             ��          ��          ");
gotoxy(1, 31); printf("                ��        ��   ��  �����    ��    ��    ��    �����  ");
Sleep(PRINTSPEED);

Sleep(500);
printxy(52,41,BY JSM & HSJ & LJH & LMS);
PlaySound(NULL, 0, 0);
Sleep(2200);

}
// Ÿ��Ʋ b*/
void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
// Ŀ�� �̵�
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
//Ŀ�� Ÿ�� ����
void setColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
//���� �ֱ�
void loadingStage(){
	int i;
	int	move = 4;
	int move2 = 2;

	for (i = 6 + move; i < 68 + move; i += 2)
	{
		setColor(RED);
		printxy(i, 9, ��);
		Sleep(15);
	}

	printxy(66 + move, 9, ��);
	for (i = 10; i < 38; i++)
	{
		setColor(RED);
		printxy(66 + move, i, ��);
		Sleep(25);
	}

	printxy(66 + move, 37, ��);
	for (i = 64 + move; i > 4 + move; i -= 2)
	{
		setColor(VIOLET);
		printxy(i, 37, ��);
		Sleep(15);
	}

	printxy(4 + move, 37, ��);
	for (i = 36; i > 9; i--)
	{
		setColor(VIOLET);
		printxy(4 + move, i, ��);
		Sleep(25);
	}
	setColor(RED);
	printxy(4 + move, 9, ��);

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
	gotoxy(51, 8); // ��������
	printf("SHOOT & DODGE v%s", VERSION);

	printxy(14, 38, Copyright�� 2015.All - M.I.T.E All Rights Reserved.);

	setColor(YELLOW);


	while (1){
		printxy(29, 45, PRESS ANY KEY TO START);
		Sleep(600);
		printxy(29, 45, ����������������������);
		Sleep(600);

		if (kbhit())
			break;
	}

}
//���� ����ȭ��   �� ���� ����
void drawPlayer()
{
	setColor(WHITE);
	gotoxy(px + 2, py - 1);
	printf("��");
	gotoxy(px, py);
	printf("���ᢺ");
}
//�÷��̾� ���
void erasePlayer(){
	printxy(px + 2, py - 1, ��);
	printxy(px, py, ������);
}
// ������ �÷��̾� �����
void playerMoveAction(){
	int i, ch;

	if (GetAsyncKeyState(VK_LEFT)<0 && px>1){
		erasePlayer();
		px--;
		drawPlayer();
	}//�����̵�
	else if (GetAsyncKeyState(VK_RIGHT)<0 && px<70){
		erasePlayer();
		px++;
		drawPlayer();
	}//�������̵�
	else if (GetAsyncKeyState(VK_UP)<0 && py > 28){
		erasePlayer();
		py--;
		drawPlayer();
	}//�����̵�
	else if (GetAsyncKeyState(VK_DOWN)<0 && py < 41){
		erasePlayer();
		py++;
		drawPlayer();
	}//�Ʒ����̵�
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
// �÷��̾� �̵� 
void statusBar(){
	int i, count;

	setColor(WHITE);
	printxy(0, 42, �âââââââââââââââââââââââââââââââââââââââ�);
	printxy(5, 46, HEALTH:);

	setColor(WHITE);

	for (i = 0; i < playerHealth; i++){
		gotoxy(13 + 2 * i, 46);
		printf("��");
	}
	count = 4 - playerHealth;
	for (i = 0; i < count; i++){
		gotoxy(19 - 2 * i, 46);
		printf("��");
	}

	if (combo >= 10){
		gotoxy(50, 44);
		setColor(YELLOW);
		printf("%d ", combo);
		setColor(WHITE);
		printf("�޺�!  BONUS + %d ��", comboBonus(combo));
	}
	else if (combo == 0){
		gotoxy(50, 44);
		printf("������                         ");
	}

	gotoxy(50, 46);
	printf("���� : %d ��", score);

	finish = clock();
	playtime = (finish - start) / CLOCKS_PER_SEC;
	gotoxy(50, 48);
	printf("Play Time : %d �� %d ��", (int)playtime / 60, (int)playtime % 60);
}
// ���¹� ����
void drawBullet(int i){
	setColor(WHITE);
	printxy(Bullet[i].x, Bullet[i].y, *);
}
// �÷��̾� �Ѿ� ���
void eraseBullet(int i){
	printxy(Bullet[i].x, Bullet[i].y, ��);
}
// ������ �÷��̾� �Ѿ� �����
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
// �÷��̾� �Ѿ� ������ ����
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

			Enemy1[i].nFrame = Enemy1[i].nCount = random(2) + 1; // �⺻ �ӵ� 1 �߰��ӵ� (0~5)
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

			Enemy2[i].nFrame = Enemy2[i].nCount = random(2) + 1; // �⺻ �ӵ� 4 �߰��ӵ� (0~2)
			Enemy2[i].type = random(sizeof(arEnemy) / sizeof(arEnemy[0]));
			Enemy2[i].exist = TRUE;
		}
	}
}
// ���� ���� / enemymoveaction ���� 
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
// ���� ��� �� ������
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
// ���� �Ѿ� (�ӵ�/��ǥ)
void drawEnemy1Bullet(int i)
{
	setColor(YELLOW);
	printxy(Enemy1Bullet[i].x, Enemy1Bullet[i].y, ��);
}
void drawEnemy2Bullet(int i)
{
	setColor(YELLOW);
	printxy(Enemy2Bullet[i].x, Enemy2Bullet[i].y, ��);
}
// ���� �Ѿ� ���
void eraseEnemy1Bullet(int i)
{
	printxy(Enemy1Bullet[i].x, Enemy1Bullet[i].y, ��);
}
void eraseEnemy2Bullet(int i)
{
	printxy(Enemy2Bullet[i].x, Enemy2Bullet[i].y, ��);
}
// ������ ���� �Ѿ� ����
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
// ���� �Ѿ� ������
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
			printxy(Enemy1[i].x - 1, Enemy1[i].y, +10�� + );
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
			printxy(Enemy2[i].x - 1, Enemy2[i].y, +10�� + );
			printxy(Enemy2[i].x - 1, Enemy2[i].y + 1, ++++ + );
			score += 10;
			Enemy2[i].nFrame = Enemy2[i].nCount = 50;
			break;
		}

	}
}
// ���� �ǰ�����
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
// �÷��̾� �ǰ�����
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
//�÷��̾� �Ѿ�, ���� �Ѿ�, ���� �ʱ�ȭ (��� �ʱ� ���� �ʱ�ȭ)
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
//����, �Ѿ�, �÷��̾ ����
void printGameTitle(int x, int y){
	int i;
	x = x + 1;
	i = y;
	setColor(WHITE);
	printxy(x, i, )Sleep(WAITTIME);
	printxy(x, i + 1, )Sleep(WAITTIME);
	printxy(x, i + 2,�����ᡡ���ᡡ���ᡡ����ᡡ������ᡡ�������ᡡ��)Sleep(WAITTIME);
	printxy(x, i + 3,�ᡡ�����ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�����ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 4,�ᡡ������������ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�����ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 5,�����ᡡ���ᡡ���ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�����ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 6,���������ᡡ�ᡡ���ᡡ����ᡡ������ᡡ�������ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 7,���������ᡡ�ᡡ���ᡡ��������  ���������������ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 8,�����ᡡ�������������������������������������ᡡ������)Sleep(WAITTIME);
	setColor(YELLOW);
	printxy(x, i + 9, �����������������������ᡡ������������������������)Sleep(WAITTIME);
	printxy(x, i + 10,�������������������ᡡ�����ᡡ����������������������)Sleep(WAITTIME);
	printxy(x, i + 11,�������������������ᡡ�����ᡡ����������������������)Sleep(WAITTIME);
	printxy(x, i + 12,������������������ ���ᡡ �ᡡ������������������)Sleep(WAITTIME);
	printxy(x, i + 13,�������������������ᡡ�� �� �ᡡ��������������������)Sleep(WAITTIME);
	printxy(x, i + 14,�������������������ᡡ�����ᡡ����������������������)Sleep(WAITTIME);
	printxy(x, i + 15,�����������������������ᡡ�ᡡ��������������������)Sleep(WAITTIME);
	printxy(x, i + 16,�������������������������������ᡡ������������������)Sleep(WAITTIME);
	printxy(x, i + 17, )Sleep(WAITTIME);

	setColor(WHITE);

	printxy(x, i + 18, ���ᡡ�����������������������������ᡡ��������������)Sleep(WAITTIME);
	printxy(x, i + 19, �ᡡ���ᡡ������ᡡ�����ᡡ���ᡡ�����ᡡ�����ᡡ��)Sleep(WAITTIME);
	printxy(x, i + 20, �ᡡ�����ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�ᡡ�����ᡡ���ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 21, �ᡡ�����ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�ᡡ�������������ᡡ��)Sleep(WAITTIME);
	printxy(x, i + 22, �ᡡ�����ᡡ�ᡡ���ᡡ�ᡡ���ᡡ�ᡡ����ᡡ�ᡡ������)Sleep(WAITTIME);
	printxy(x, i + 23, �ᡡ���ᡡ������ᡡ�����ᡡ���ᡡ������ᡡ���ᡡ��)Sleep(WAITTIME);
	printxy(x, i + 24, ���ᡡ�����������������������������ᡡ�ᡡ����������)Sleep(WAITTIME);
	printxy(x, i + 25, ������������)Sleep(WAITTIME);
	printxy(x, i + 26, )Sleep(WAITTIME);
}
//��������ǥ��
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
	printxy(px - 2, py, @��������@);
	printxy(px - 2, py + 1, @��������@);
	printxy(px - 2, py + 2, @��������@);
	printxy(px - 2, py + 3, @@@@@@@@@@);
	Sleep(2000);

	system("cls");
}//����� �׼�
//�÷��̾� ��� �׼�
void GAMEOVER() {
	system("cls");
	setColor(WHITE);
	printxy(0, 12, ================================== GAME OVER =================================== );
	setColor(SKYBLUE);
	gotoxy(32, 14);
	printf("Score = %d��", score);
	gotoxy(28, 16);
	printf("Play Time = %2d �� %2d��", (int)playtime / 60, (int)playtime % 60);
	setColor(WHITE);
	printxy(0, 32, ================================================================================ );
}
//���ӿ��� �޼���	
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
		printf("���� ���");
		Sleep(200);
		gotoxy(20, 18);
		printf("        ");
		Sleep(200);
	}

	gotoxy(20, 18);
	printf("���� ���");

	Sleep(250);

	for (j = 0; j < 10; j++) {
		gotoxy(10, 20 + j);
		printf("%2d�� : %s ", j + 1, list[j].name);
		Sleep(250);
	}

	for (j = 0; j < 10; j++) {
		gotoxy(30, 20 + j);
		printf("%3d ��", list[j].score);
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
		printf("�ð� ���");
		Sleep(200);
		gotoxy(50, 18);
		printf("        ");
		Sleep(200);
	}
	gotoxy(50, 18);
	printf("�ð� ���");
	Sleep(250);

	for (j = 0; j < 10; j++) {
		gotoxy(40, 20 + j);
		printf("%2d�� : %s ", j + 1, list[j].name);
		Sleep(250);
	}

	for (j = 0; j < 10; j++) {
		gotoxy(60, 20 + j);
		printf("%2d�� %2d��", (int)list[j].time / 60, (int)list[j].time % 60);
		Sleep(250);
	}
}
//��ŷ ���
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
//�޺����ʽ�
/*void Init()
{
FMOD_System_Create(&g_System);
FMOD_System_Init( g_System, 32, FMOD_INIT_NORMAL,  NULL);
FMOD_System_CreateSound( g_System, "BGM.wav", FMOD_DEFAULT, 0, &g_Sound);
}
//BGM����
void Release()
{
FMOD_Sound_Release(g_Sound);
FMOD_System_Close(g_System);
FMOD_System_Release(g_System);
}
//BGM�޸� ����*/
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

		boss.nFrame = boss.nCount = 2; // �⺻ �ӵ� 4 �߰��ӵ� (0~2)
		boss.yFrame = boss.yCount = 100;
		boss.exist = TRUE;
	}
}
// ���� ���� / bossmoveaction ���� 
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
			gotoxy(boss.x - 3, boss.y - 1);   printf("  �������  ");
			gotoxy(boss.x - 3, boss.y);       printf("  ��  ���  ��  ");
			gotoxy(boss.x - 3, boss.y + 1);   printf("    �����  ");

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
			gotoxy(boss.x - 3, boss.y - 1);   printf("  �������  ");
			gotoxy(boss.x - 3, boss.y);       printf("  ��  ���  ��  ");
			gotoxy(boss.x - 3, boss.y + 1);   printf("    �����  ");

		}//inner else
	}//outer if
	bossPattern1();
	bossPattern2();

}
// ���� ������
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
//���� �Ѿ� ����
void drawBossBullet1(int i)
{
	setColor(YELLOW);
	printxy(bossBullet1[i].x - 5, bossBullet1[i].y, ��);

}
void drawBossBullet2(int i)
{
	setColor(YELLOW);
	printxy(bossBullet2[i].x + 5, bossBullet2[i].y, ��);

}
//�����Ѿ� ���
void eraseBossBullet1(int i)
{
	printxy(bossBullet1[i].x - 5, bossBullet1[i].y, ��);
}
void eraseBossBullet2(int i)
{
	printxy(bossBullet2[i].x + 5, bossBullet2[i].y, ��);
}
//�����Ѿ� ����
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
//���� �Ѿ� ������
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
//���� �ǰ�����
//========================================================================
void main() {
	int i = 0;

	setcursortype(NOCURSOR);  //Ŀ�� ����
	system("mode con: cols=80 lines=50"); //�ܼ�â ũ�� ���� x = 80, y = 50
	//Init();
	
	
	title_a();
	
	
	while (1) { //���� �ݺ�

		system("cls");
		//FMOD_System_PlaySound( g_System, FMOD_CHANNEL_FREE, g_Sound,0, &channel);
		PlaySound(TEXT("BGM.wav"), NULL, SND_ASYNC || SND_LOOP);
		
		setColor(WHITE);
		loadingStage();		//�ε��������� �ҷ�����

		system("cls");
		randomize();				// ���� �߻��� �ʱ�ȭ
		start = clock();
		reset();

		
		while (1) { //��������1
			statusBar();				// ���¹� ���

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}
			if (kill >= 10){
				system("cls");
				gotoxy(40, 25); printf("��������2");
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

			Sleep(GAMESPEED);//���Ӽӵ� ����
		}// ��������1

		if (playerHealth>0)
			semiReset();

		diff = 2;  //���̵� ����

		while (1) {//��������2

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}

			if (kill >= 20){
				system("cls");
				gotoxy(40, 25); printf("��������3");
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

			Sleep(GAMESPEED);//���Ӽӵ� ����
		}// �������� 2

		if (playerHealth>0)
			semiReset();
		diff = 1;  //���̵� ����

		while (1) {//�������� 3

			if (playerHealth <= 0) {
				finish = clock();
				playtime = (double)(finish - start) / CLOCKS_PER_SEC;
				break;
			}

			if (kill >= 30){
				system("cls");
				gotoxy(40, 25); printf("����");
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

			Sleep(GAMESPEED);//���Ӽӵ� ����
		}// �������� 3

		if (playerHealth>0)
			semiReset();

		while (1){//����
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
			printf("���������������������������������������");


			if (bossHealth <= 0){
				system("cls");
				gotoxy(40, 25); printf("���Ѹ��");
				Sleep(2000);
				break;
			}

			statusBar();

			Sleep(GAMESPEED);//���Ӽӵ� ����
		}

		while (1) {//���Ѹ��

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

			Sleep(GAMESPEED);//���Ӽӵ� ����
		}//���Ѹ��

		playerDie();

		fflush(stdin);
		setColor(WHITE);
		printxy(28, 12, �̸��� �Է��ϼ��� : );
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