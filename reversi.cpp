#include "DxLib.h"

//プロトタイプ宣言
void drawBoard(int board[][8], int turn, char* name, unsigned int colors[]);
void SetStone(int x, int y, unsigned int color);
int checkBoard(int board[][8], int* allocablePosition[][8], int turn, unsigned int pointColor);
void turnStone(int posX, int posY, int board[][8], int* allocablePosition[][8]);
int GetClickArea(int* posX, int* posY);
void changeTurn(int* turn);
void showMessage(const char* message);

#define EOP -1	//パターン終端

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
	SetGraphMode(480, 530, 32);
	SetBackgroundColor(36, 36, 36);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	//各種変数
	int i;		//ループカウンタ
	int j;		//ループカウンタ
	int turn = 1;		//手番（1が先攻[黒]、2が後攻[白]）
	int stoneNum = 4;	//石の数
	int skip = 0;		//スキップされたかどうか *2回連続スキップされた場合終了
	int board[8][8] = {	//リバーシボード（0未配置,1黒,2白）
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,2,1,0,0,0},
		{0,0,0,1,2,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	};
	int* allocablePosition[8][8];	//石の配置可能場所
	for (i = 0; i < 64; i++) {		//配置可能場所の各要素に空ポインタを入れる
		allocablePosition[i / 8][i % 8] = NULL;
	}
	int playerStoneNum[3] = { -1,0,0 };	//プレイヤーごとの石の数

	//色（盤面、黒の石、白の石）、プレイヤー名
	unsigned int gameColor[3] = { GetColor(0,128,0),GetColor(0,0,0),GetColor(255,255,255) };
	char playerName[3][40] = { "","黒","白"};
	char resultStr[40];

	//盤面の初期配置を描画
	drawBoard(board, turn, playerName[turn], gameColor);
	
	//ゲーム進行
	while (1) {	//ゲーム進行ループ
		//石の配置可能な位置を判定・表示
		if (checkBoard(board, allocablePosition, turn, gameColor[turn]) == -1) {
			//石を置ける場所がなかった場合
			if (skip == 1) {	//スキップが連続2回目の場合
				showMessage("これ以上置けません");
				drawBoard(board, turn, playerName[turn], gameColor);
				break;
			}
			else {
				skip = 1;
				showMessage("置ける場所がないため、パスします");
				drawBoard(board, turn, playerName[turn], gameColor);
				changeTurn(&turn);
				drawBoard(board, turn, playerName[turn], gameColor);
				continue;
			}
		}
		skip = 0;

		while (1) {	//石配置判定ループ
			//石が配置、またはスキップされるまで繰り返します
			int clickPosX;	//クリック位置X(0~7)
			int clickPosY;	//クリック位置Y(0~7)
			if (GetClickArea(&clickPosX, &clickPosY) == -1) {	//クリック判定中にウィンドウが閉じられたまたは異常があった場合
				DxLib_End();	//終了処理
				return 0;
			}

			if (allocablePosition[clickPosY][clickPosX] != NULL) {	//クリックした場所が配置可能な場合
				board[clickPosY][clickPosX] = turn;
				turnStone(clickPosX, clickPosY, board, allocablePosition);
				stoneNum++;
				break;
			}

		}
		if (stoneNum == 64) {	//盤面のすべての場所に石が配置された場合
			drawBoard(board, turn, playerName[turn], gameColor);
			break;
		}

		//ターンチェンジ
		changeTurn(&turn);
		drawBoard(board, turn, playerName[turn], gameColor);
	}
	
	//石を数える
	//結果と勝敗を表示する
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			playerStoneNum[board[j][i]]++;
		}
	}

	//この文は2人プレイ用に簡略化しています
	if (playerStoneNum[1] > playerStoneNum[2]) {
		sprintf_s(resultStr,40, "%2d対%-2d  %sの勝ち", playerStoneNum[1], playerStoneNum[2], playerName[1]);
		showMessage(resultStr);
	}
	else if (playerStoneNum[2] > playerStoneNum[1]) {
		sprintf_s(resultStr, 40, "%2d対%-2d  %sの勝ち", playerStoneNum[1], playerStoneNum[2], playerName[2]);
		showMessage(resultStr);
	}
	else {
		sprintf_s(resultStr, 40, "%2d対%-2d  引き分けです", playerStoneNum[1], playerStoneNum[2]);
		showMessage(resultStr);
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}


//盤面の初期化 引数int* board…盤面配列の起点[0][0]のアドレス
void drawBoard(int board[][8], int turn, char* name, unsigned int colors[]) {
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
			if (board[i][j] != 0) {	//石がある場所の場合
				SetStone(j, i, colors[board[i][j]]);
			}
		}
	}

	//現在のターンを表示
	SetStone(0, 8, colors[turn]);
	DrawFormatString(100, 480, GetColor(255, 255, 255),"%sのターン", name);

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
int checkBoard(int board[][8], int* allocablePosition[][8], int turn, unsigned int pointColor) {
	
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
			if (board[yPos][xPos] != 0) {	
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

					if (board[yChk][xChk] != turn && board[yChk][xChk] != 0) {
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

void turnStone(int posX, int posY, int board[][8], int* allocablePosition[][8]) {
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
	if (*turn == 1) {
		*turn = 2;
	}
	else {
		*turn = 1;
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