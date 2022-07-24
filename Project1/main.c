#define _CRT_SECURE_NO_WARNINGS
#define MAX_GAME_HISTORY 100
#define MAX_PLAYER 10
#define MAX_INPUT 100
#define FILE_NAME "game.dat"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
struct GamePlayer {
	//用于存放玩家相关信息的结构体
	char account[MAX_INPUT];								//账号
	char password[MAX_INPUT];								//密码
	int gamecount;											//已进行游戏数
	int history[MAX_GAME_HISTORY][5];						//游戏历史
}player[MAX_PLAYER];
int key = 0;												//主键
void Update() {
	//将内存中的数据更新到文件
	FILE *fp = fopen(FILE_NAME, "wt");
	if (!fp) {
		printf("Error writing data!\n\n");
		return -1;
	}
	for (int i = 0; i <= key; i++) {
		fprintf(fp, "%s %s\n", player[i].account, player[i].account);
		if (player[i].account[0] == '\0')break;
		for (int j = 0; j < MAX_GAME_HISTORY; j++) {
			int round, win, draw, loss, gamewin;
			round = player[i].history[j][0];
			win = player[i].history[j][1];
			draw = player[i].history[j][2];
			loss = player[i].history[j][3];
			gamewin = player[i].history[j][4];
			fprintf(fp, "%d %d %d %d %d\n", round, win, draw, loss, gamewin);
		}
	}
	fclose(fp);
}
void Read() {
	//从文件读取数据到内存
	void LoadHistoiry(int player_index, int game, int round, int win, int draw, int loss, int gamewin);
	int round, win, draw, loss, gamewin;
	for (int i = 0; i < MAX_PLAYER; i++) {					//初始化
		for (int j = 0; j < MAX_INPUT; j++)
			player[i].account[j] = '\0';
		for (int j = 0; j < MAX_INPUT; j++)
			player[i].password[j] = '\0';
		for (int j = 0; j < MAX_GAME_HISTORY; j++)
			LoadHistoiry(i, j, 0, 0, 0, 0, 0);
	}
	FILE *fp = fopen(FILE_NAME, "rt");
	if (!fp) {												//如果游戏数据文件不存在，就创建这个文件
		Update();
		return;
	}
	while (!feof(fp)) {
		fscanf(fp, "%s %s\n", player[key].account, player[key].password);
		while (player[key].account[0] != '\0') {
			for (int game = 0; game < MAX_GAME_HISTORY; game++) {
				fscanf_s(fp, "%d %d %d %d %d\n", &round, &win, &draw, &loss, &gamewin);
				LoadHistoiry(key, game, round, win, draw, loss, gamewin);
				if (round != 0)player[key].gamecount++;
			}
			key++;
		}
	}
	fclose(fp);
}
char Select() {
	char ch[MAX_INPUT];
	gets_s(ch, MAX_INPUT);
	system("cls");
	if (strlen(ch) > 1)
		return 'z';
	else
		return ch[0];
}
void HidePasswd(char passwd[]) {
	//隐藏密码
	char c;
	int i = 0;
	while ((c = getch()) != '\r') {							//输入不为回车
		if (c == '\b') {									//输入退格键
			if (i <= 0)continue;							//如果数据为空，不继续删除
			i--;
			passwd[i] = '\0';								//删除最后一位的数据
			printf("\b \b");								//用空格盖住被删除内容
			continue;
		}
		else {
			passwd[i] = c;									//写入该位的密码
			putchar('*');									//输出星号
			i++;
		}
	}
	printf("\n");
}
int Find(char account[MAX_INPUT]) {
	//寻找所给账号
	for (int i = 0; i < MAX_PLAYER; i++)					//遍历已有数据
		if (strcmp(account, player[i].account) == 0)		//已有数据中存在该账号
			return i;										//返回该账号的索引
	return -1;												//未找到该账号
}
void GetAccount(char account[MAX_INPUT]) {
	//获取合法账号
	while (1) {
		printf("\nPlease enter the account：");
		gets_s(account, MAX_INPUT);
		if (account[0] == '\0')continue;					//如果未输入，重新提示输入
		break;
	}
}
void GetPasswd(char password[MAX_INPUT]) {
	//获取合法密码
	while (1) {
		printf("\nPlease enter the password：");
		HidePasswd(password);
		if (password[0] == '\0')continue;					//如果未输入，重新提示输入
		break;
	}
}
void Register() {
	//注册界面
	char account[MAX_INPUT];								//输入的账号
	char password[MAX_INPUT] = { 0 };						//输入的密码
	while (1) {
		system("cls");
		while (1) {											//设置账号
			GetAccount(account);
			if (Find(account) != -1) {						//已有用户使用该账号
				printf("\n【ERROR】This account is already taken!\n");
				continue;
			}
			break;
		}
		while (1) {											//设置密码
			char verify[MAX_INPUT];
			GetPasswd(password);
			for (int i = 0; i < 20; i++)					//初始化确认密码
				verify[i] = '\0';
			printf("\nPlease confirm your password：");
			HidePasswd(verify);
			if (strcmp(password, verify) != 0) {			//检查两次密码是否一致
				printf("\n【ERROR】The two entries are inconsistent!\n");
				continue;
			}
			break;
		}
		//上述步骤都没问题，开始加载数据到结构体
		strcpy_s(player[key].account, MAX_INPUT, account);	//加载账号
		strcpy_s(player[key].password, MAX_INPUT, password);//加载密码
		for (int i = 0; i < MAX_GAME_HISTORY; i++)			//初始化新用户的游戏历史
			for (int j = 0; j < 5; j++)
				player[key].history[i][j] = 0;
		system("cls");
		printf("\nRegister successful!\n\n");
		Update();											//将新用户信息更新到文件
		key++;
		break;
	}
}
void Login() {
	//登录界面
	void GameMenu();										//声明开始菜单
	char account[MAX_INPUT];								//输入的账号
	char password[MAX_INPUT] = { 0 };						//输入的密码
	int player_index = -1;									//玩家索引
	while (1) {
		system("cls");
		while (1) {
			GetAccount(account);
			player_index = Find(account);
			if (player_index == -1) {
				printf("\n【ERROR】No account found, please check if the input is correct!\n");
				continue;
			}
			break;
		}
		int failcount = 0;
		while (1) {											//检查输入的密码是否与账号匹配
			GetPasswd(password);
			if (strcmp(password, player[player_index].password) != 0) {
				failcount++;								//失败计数
				if (failcount == 3) {						//登录失败三次自动退出游戏
					system("cls");
					printf("\nYou have run out of chances!\n");
					printf("You will be logged out automatically!\n\n");
					system("pause");
					exit(0);
				}
				printf("\n【ERROR】The password does not match the account number, please check if the input is correct!\n");
				printf("You have %d more chance!\n", 3 - failcount);
				continue;
			}
			else break;
		}
		system("cls");
		printf("\nLogin successful!\n\n");
		system("pause");
		GameMenu(player_index);								//登录成功，玩家进入游戏菜单
		break;
	}
}
int Dudge(char a, char b) {
	//判断胜负并返回结果，1为胜利，0为平局，-1为失败
	if (a == 'A'&&b == 'B')return 1;
	else if (a == 'B'&&b == 'C')return 1;
	else if (a == 'C'&&b == 'A')return 1;
	else if (a == 'A'&&b == 'A')return 0;
	else if (a == 'B'&&b == 'B')return 0;
	else if (a == 'C'&&b == 'C')return 0;
	else if (a == 'B'&&b == 'A')return -1;
	else if (a == 'C'&&b == 'B')return -1;
	else if (a == 'A'&&b == 'C')return -1;
}
void LoadHistoiry(int player_index, int game, int round, int win, int draw, int loss, int gamewin) {
	//加载游戏记录到结构体
	player[player_index].history[game][0] = round;
	player[player_index].history[game][1] = win;
	player[player_index].history[game][2] = draw;
	player[player_index].history[game][3] = loss;
	player[player_index].history[game][4] = gamewin;
}
void PrintHistory(int player_index, int game) {
	//显示游戏历史
	int round, win, draw, loss, gamewin;
	round = player[player_index].history[game][0];
	win = player[player_index].history[game][1];
	draw = player[player_index].history[game][2];
	loss = player[player_index].history[game][3];
	gamewin = player[player_index].history[game][4];
	if (gamewin == 1)printf("\nYou win!\n");
	else if (gamewin == 0)printf("\nDarw!\n");
	else if (gamewin == -1)printf("\nYou lose!\n");
	printf("\nDetail:\n");
	printf("Number of rounds in this game:%d\n", round);
	printf("Number of player wins:%d\n", win);
	printf("Number of draws:%d\n", draw);
	printf("Number of computer wins:%d\n\n", loss);
}
void Display(char a[MAX_INPUT], int dudgelist[MAX_INPUT], char b[MAX_INPUT], int round) {
	//显示本次游戏的结果
	printf("\nComputer's letter:");
	for (int i = 0; i < round; i++)
		printf(" %c", a[i]);
	printf("\n                  ");
	for (int i = 0; i < round; i++) {
		int f = dudgelist[i];
		if (f == 1)printf("%s", "↑");
		else if (f == 0)printf(" %c", '||');
		else if (f == -1)printf("%s", "↓");
	}
	printf("\nYour letter:      ");
	for (int i = 0; i < round; i++)
		printf(" %c", b[i]);
	printf("\n\n");
}
void PlayGame(int player_index) {
	//游戏主函数
	while (1) {
		int round = 0;										//回合数
		int score = 0;										//得分（用于判断胜负）
		printf("\nGame:No.%d\n", player[player_index].gamecount);
		printf("\nInput nmuber of rounds in the game: ");
		scanf_s("%d", &round);
		char constom[3] = { 'A','B','C' };
		char a[MAX_INPUT] = { 0 }, b[MAX_INPUT] = {0};
		int dudgelist[MAX_INPUT] = {0};
		int win = 0, draw = 0, loss = 0;
		int gamewin = 0;									//胜利标识
		for (int i = 0; i < round; i++) {					//生成电脑玩家的字母列表
			srand((unsigned)time(NULL) + i * i);
			a[i] = constom[rand() % 3];
		}
		for (int i = round; i < 2 * round; i++) {			//生成玩家的字母列表
			srand((unsigned)time(NULL) + i * i);
			b[i-round] = constom[rand() % 3];
		}
		for (int i = 0; i < round; i++) {					//计算每个回合的输赢
			dudgelist[i] = Dudge(a[i], b[i]);
			if (dudgelist[i] == 1)win++;
			else if (dudgelist[i] == 0)draw++;
			else if (dudgelist[i] == -1)loss++;
			score += dudgelist[i];
		}
		if (score > 0)gamewin = 1;
		else if (score == 0)gamewin = 0;
		else if (score < 0)gamewin = -1;
		Display(a, dudgelist, b, round);
		LoadHistoiry(player_index, player[player_index].gamecount, round, win, draw, loss, gamewin);
		PrintHistory(player_index, player[player_index].gamecount);
		player[player_index].gamecount++;
		Update();
		char ch;
		printf("Press [Enter] To start a new game, enter \"q\" To quit:");
		getchar();
		ch = Select();
		if (ch == 'q') break;
		else if (ch == '\0') continue;
	}
}
void ReviewHistory(int player_index) {
	//回顾游戏历史
	if (player[player_index].gamecount == 0) {
		printf("\nNo History!\n\n");
		system("pause");
		return;
	}
	for (int i = 0; i < player[player_index].gamecount; i++) {
		printf("───────────────────────────\n");
		printf("Game:No.%d\n", i);
		PrintHistory(player_index, i);
	}
	system("pause");
}
void ClearHistory(int player_index) {
	//清除游戏历史
	for (int i = 0; i < player[player_index].gamecount; i++)
		LoadHistoiry(player_index, i, 0, 0, 0, 0, 0);
	Update();
	printf("\nGame history is cleared successful!\n\n");
	system("pause");
}
void GameMenu(int player_index) {
	//游戏菜单
	char ch;
	while (1) {
		system("cls");
		printf("\n\n\n\t\t\t\t\t\t           The Game\n");
		printf("\n\t\t\t\t\t\t         Player:%s\n\n\n",player[player_index].account);
		printf("\t\t\t\t\t\t┌───────────────────────────┐\n");
		printf("\t\t\t\t\t\t│                           │\n");
		printf("\t\t\t\t\t\t│  【a】Start a new game    │\n");
		printf("\t\t\t\t\t\t│                           │\n");
		printf("\t\t\t\t\t\t│  【b】Review game history │\n");
		printf("\t\t\t\t\t\t│                           │\n");
		printf("\t\t\t\t\t\t│  【c】Clear game history  │\n");
		printf("\t\t\t\t\t\t│                           │\n");
		printf("\t\t\t\t\t\t│  【d】Logout              │\n");
		printf("\t\t\t\t\t\t│                           │\n");
		printf("\t\t\t\t\t\t└───────────────────────────┘\n\n\t\t\t\t\t\t");
		ch = Select();
		if (ch == 'a') PlayGame(player_index);				//开始新游戏
		else if (ch == 'b') ReviewHistory(player_index);	//回顾游戏历史
		else if (ch == 'c') ClearHistory(player_index);		//清除游戏历史
		else if (ch == 'd') {								//注销账号
			printf("\nLogout successful! You are about to return to the main screen.\n\n");
			break;
		}
		else if (ch == '\0') continue;
		else {
			printf("\nInput error, please try again!\n\n");
			system("pause");
		}
	}
}
void Start() {
	//开始界面
	char ch;
	while (1) {
		system("cls");
		printf("\n\n\n\t\t\t\t\t\t      The Game\n\n\n");
		printf("\t\t\t\t\t\t┌──────────────────┐\n");
		printf("\t\t\t\t\t\t│                  │\n");
		printf("\t\t\t\t\t\t│  【a】Register   │\n");
		printf("\t\t\t\t\t\t│                  │\n");
		printf("\t\t\t\t\t\t│  【b】Login      │\n");
		printf("\t\t\t\t\t\t│                  │\n");
		printf("\t\t\t\t\t\t│  【c】Exit       │\n");
		printf("\t\t\t\t\t\t│                  │\n");
		printf("\t\t\t\t\t\t└──────────────────┘\n\n\t\t\t\t\t\t");
		ch = Select();
		if (ch == 'a') Register();							//进入注册界面
		else if (ch == 'b') Login();						//进入登录界面
		else if (ch == 'c') break;							//退出
		else if (ch == '\0') continue;
		else printf("\nInput error, please try again!\n\n");
		system("pause");
	}
}
int main() {
	Read();
	Start();
	return 0;
}