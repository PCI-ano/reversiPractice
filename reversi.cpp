#include "DxLib.h"

//マクロ
#define PLAYER_NUM 2	//プレイヤーの人数 *2人用に開発しているため、人数変更は不可です。
#define Y_BOARD 8	//縦のマス目の数
#define X_BOARD 8	//横のマス目の数
#define STATUS_NOSTONE -1	//盤面に石が置かれていないときの値
#define EOP -1		//パターン終端

//構造体
typedef struct boardData {	//盤面の情報
	int status[Y_BOARD][X_BOARD];			//石の配置状態
	int allocPosition[Y_BOARD][X_BOARD];	//石の配置可能場所（今後利用予定）
	int stoneNum = 0;
}Board;

typedef struct {	//プレイヤー情報
	char name[100];
	unsigned int color;
	int stoneNum;
}Player;

typedef struct gameData{	//ゲーム進行関連
	int playerNum;	//プレイヤーの人数
	int turn;		//どのプレイヤーのターンであるか
	int isSkipped = 0;	//スキップしたかどうか
}Game;

//プロトタイプ宣言
Board initBoard(void);
void drawBoard(int board[Y_BOARD][X_BOARD], int turn, Player* players);
void clearArray(int* array, int y, int x, int value);
void SetStone(int x, int y, unsigned int color);
int checkBoard(int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD], int turn, unsigned int pointColor);
void turnStone(int posX, int posY, int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD]);
int GetClickArea(int* posX, int* posY);
void changeTurn(int* turn);
void showMessage(const char* message);


//グローバル変数
int gr_direction[8][2] = {	//方向
		{-1,-1},
		{-1,0},
		{-1,1},
		{0,-1},
		{0,1},
		{1,-1},
		{1,0},
		{1,1}
};

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);	//ウィンドウモード
	SetMainWindowText("リバーシ");
	SetGraphMode(480, 530, 32);
	SetBackgroundColor(28, 24, 20);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;				// エラーが起きたら直ちに終了
	}

	
	//各種変数
	Board board = initBoard();		//盤面に関する値
	Player players[2] = {			//プレイヤーに関する値
		{"黒", GetColor(0,0,0)},
		{"白", GetColor(255,255,255)}
	};
	Game game = { PLAYER_NUM, 0, 0 };	//ゲーム進行に関する値

	int i;		//ループカウンタ
	int j;		//ループカウンタ
	
	int* allocablePosition[8][8];	//石の配置可能場所
	for (i = 0; i < 64; i++) {		//配置可能場所の各要素に空ポインタを入れる
		allocablePosition[i / 8][i % 8] = NULL;
	}
	
	char resultStr[40];	//連結した文字列を格納するバッファ
	
	//ゲーム進行
	while (1) {	//ゲーム進行ループ
		drawBoard(board.status, game.turn, players);	//画面の描画

		//石の配置可能な位置を判定・表示
		if (checkBoard(board.status, allocablePosition, game.turn, players[game.turn].color) == -1) {
			//石を置ける場所がなかった場合
			if (game.isSkipped == 1) {	//スキップが連続2回目の場合
				showMessage("これ以上置けません");
				drawBoard(board.status, game.turn, players);
				break;
			}
			else {
				game.isSkipped = 1;
				showMessage("置ける場所がないため、パスします");
				drawBoard(board.status, game.turn, players);
				changeTurn(&game.turn);
				drawBoard(board.status, game.turn, players);
				continue;
			}
		}
		game.isSkipped = 0;

		while (1) {	//石配置判定ループ
			//石が配置、またはスキップされるまで繰り返します
			int clickPosX;	//クリック位置X(0~7)
			int clickPosY;	//クリック位置Y(0~7)
			if (GetClickArea(&clickPosX, &clickPosY) == -1) {	//クリック判定中にウィンドウが閉じられたまたは異常があった場合
				DxLib_End();	//終了処理
				return 0;
			}

			if (allocablePosition[clickPosY][clickPosX] != NULL) {	//クリックした場所が配置可能な場合
				board.status[clickPosY][clickPosX] = game.turn;
				turnStone(clickPosX, clickPosY, board.status, allocablePosition);
				board.stoneNum++;
				break;
			}

		}
		if (board.stoneNum == 64) {	//盤面のすべての場所に石が配置された場合
			drawBoard(board.status, game.turn, players);
			break;
		}

		//ターンチェンジ
		changeTurn(&game.turn);
	}
	
	//石を数える
	//結果と勝敗を表示する
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			players[board.status[j][i]].stoneNum++;
		}
	}

	//この文は2人プレイ用に簡略化しています
	if (players[0].stoneNum > players[1].stoneNum) {
		sprintf_s(resultStr,40, "%2d対%-2d  %sの勝ち", players[0].stoneNum, players[1].stoneNum, players[0].name);
		showMessage(resultStr);
	}
	else if (players[1].stoneNum > players[0].stoneNum) {
		sprintf_s(resultStr, 40, "%2d対%-2d  %sの勝ち", players[0].stoneNum, players[1].stoneNum, players[1].name);
		showMessage(resultStr);
	}
	else {
		sprintf_s(resultStr, 40, "%2d対%-2d  引き分けです", players[0].stoneNum, players[1].stoneNum);
		showMessage(resultStr);
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

Board initBoard(void) {
	Board board;
	
	//盤面を初期化
	clearArray(&board.status[0][0], Y_BOARD, X_BOARD, STATUS_NOSTONE);
	clearArray(&board.allocPosition[0][0], Y_BOARD, X_BOARD, STATUS_NOSTONE);
	
	//石の初期配置
	board.status[3][3] = 1;
	board.status[3][4] = 0;
	board.status[4][3] = 0;
	board.status[4][4] = 1;
	board.stoneNum = 4;

	return board;
}

//2次元配列の各要素にvalueを代入
//int* arrayには、配列の先頭要素[0][0]のアドレスを指定する
void clearArray(int* array, int y, int x, int value) {
	int i;	//ループカウンタ
	int j;	//ループカウンタ
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++) {
			 *(array + (y * i + j)) = value;
		}
	}

}

//盤面の描画
void drawBoard(int board[Y_BOARD][X_BOARD], int turn, Player* players) {
	int i;	//ループカウンタ
	int j;	//ループカウンタ
	int n;	//配置座標

	//リバーシボードの描画
	ClearDrawScreen();
	DrawBox(20, 20, 460, 460, GetColor(0, 128, 0), TRUE);
	for (i = 1, n = 75; i <= 7; i++, n += 55) {	//リバーシ罫線
		DrawLine(n, 20, n, 460, GetColor(32, 32, 32));
		DrawLine(20, n, 460, n, GetColor(32, 32, 32));
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (board[i][j] != STATUS_NOSTONE) {	//石がある場所の場合
				SetStone(j, i, players[board[i][j]].color);
			}
		}
	}

	//現在のターンを表示
	DrawCircleAA(47.5, 487.5, 26, 80, GetColor(128, 128, 128), TRUE);	//背景が黒に近いので、黒石が見やすいように後ろをつける
	SetStone(0, 8, players[turn].color);	//現在の石の色を表示
	DrawFormatString(100, 480, GetColor(255, 255, 255),"%sのターン", players[turn].name);

	/*
	//石の初期配置
	SetStone(3, 3, GetColor(255, 255, 255));
	SetStone(3, 4, GetColor(0, 0, 0));
	SetStone(4, 3, GetColor(0, 0, 0));
	SetStone(4, 4, GetColor(255, 255, 255));
	*/
}

//石の配置
void SetStone(int x, int y, unsigned int color) {
	DrawCircleAA(47.5 + 55 * x, 47.5 + 55 * y, 25, 80, color, TRUE);
}

//石の配置可能な位置を判定・表示
int checkBoard(int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD], int turn, unsigned int pointColor) {
	
	int success = 0;	//配置可能な位置を発見できたかどうか
	int xPos;	//xループカウンタ
	int yPos;	//yループカウンタ
	int i;		//ループカウンタ

	//メモリ開放
	for (xPos = 0; xPos < 8; xPos++) {
		for (yPos = 0; yPos < 8; yPos++) {
			if (allocablePosition[yPos][xPos] != NULL) {
				delete[] allocablePosition[yPos][xPos];
				allocablePosition[yPos][xPos] = NULL;
			}
		}
	}

	//盤面の各マスごとに判定
	for (xPos = 0; xPos < 8; xPos++) {
		for (yPos = 0; yPos < 8; yPos++) {
			
			//マスが開いていることを確認
			if (board[yPos][xPos] != STATUS_NOSTONE) {	
				continue;
			}

			//方向ごとに繰り返す
			for (i = 0; i < 8; i++) {
				int xChk = xPos;
				int yChk = yPos;

				int result;	//配置検証結果
				result = 0;

				while (1) {	//配置可能条件を満たさなくなるまで繰り返す

					if (xChk < 0 || xChk > 7 || yChk < 0 || yChk > 7) {	//盤面の端に到達している場合
						break;
					}

					//確認位置を1マス進める
					xChk += gr_direction[i][0];
					yChk += gr_direction[i][1];

					if (board[yChk][xChk] != turn && board[yChk][xChk] != STATUS_NOSTONE) {
						//*手番と異なる色の石が隣接・連続している
						result = 1;
					}
					else {
						break;
					}

				}

				if (result == 1 && board[yChk][xChk] == turn) {	//石を挟む条件を満たしている場合
					success = 1;
					if (allocablePosition[yPos][xPos] == NULL) {	//パターン格納用配列が生成されていない場合
						allocablePosition[yPos][xPos] = new int[8];
						(allocablePosition[yPos][xPos])[0] = EOP;
					}
					int j = 0;
					while ((allocablePosition[yPos][xPos])[j++] != EOP);	//パターン終端を探索
					//方向の値を追加
					(allocablePosition[yPos][xPos])[j-1] = i;
					(allocablePosition[yPos][xPos])[j] = EOP;

					DrawCircleAA(47.5 + 55 * xPos, 47.5 + 55 * yPos, 6, 40, pointColor, TRUE);
				}
			}
		}
	}
	if (success == 1) {	//配置可能な位置が見つかった場合
		return 0;
	}
	else {	//配置可能な場所が無い場合
		return -1;
	}
}

void turnStone(int posX, int posY, int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD]) {
	int i = 0;	//ループカウンタ
	int turnX;
	int turnY;

	while ((allocablePosition[posY][posX])[i] != EOP) {
		turnX = posX;
		turnY = posY;

		while (1) {
			turnX += gr_direction[(allocablePosition[posY][posX])[i]][0];
			turnY += gr_direction[(allocablePosition[posY][posX])[i]][1];

			if (board[turnY][turnX] == board[posY][posX]) {
				break;
			}
			board[turnY][turnX] = board[posY][posX];
		}

		i++;
	}
}

//クリック位置の取得
int GetClickArea(int* posX, int* posY) {
	while (1) {	//クリック判定ループ
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)	//マウスがクリックされている場合
		{
			int mouseX;
			int mouseY;
			GetMousePoint(&mouseX, &mouseY);	//マウスの位置を取得

			//マウスの座標範囲チェック
			if (20 <= mouseX && mouseX < 460) {
				if (20 <= mouseY && mouseY < 460) {
					*posX = (mouseX - 20) / 55;
					*posY = (mouseY - 20) / 55;
					break;
				}
			}
		}

		//プロセスメッセージ処理 ループし続けている箇所がクリック待ちぐらいしか無いためここに配置
		if (ProcessMessage() == -1)
		{
			return -1;
		}
	}

	return 0;
}


//ターン変更（黒⇔白）
void changeTurn(int* turn) {
	//ターンチェンジ
	if (*turn == 0) {
		*turn = 1;
	}
	else {
		*turn = 0;
	}
}


//メッセージを画面中央に表示する
void showMessage(const char* message) {
	DrawBox(80, 160, 400, 320, GetColor(200, 200, 200), TRUE);
	DrawString(100, 210, message, GetColor(0,0,0));
	WaitTimer(500);
	DrawString(160, 280, "クリックして閉じる", GetColor(64, 64, 64));
	WaitKey();
	
}