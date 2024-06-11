#include "DxLib.h"

//�}�N��
#define PLAYER_NUM 2	//�v���C���[�̐l�� *2�l�p�ɊJ�����Ă��邽�߁A�l���ύX�͕s�ł��B
#define Y_BOARD 8	//�c�̃}�X�ڂ̐�
#define X_BOARD 8	//���̃}�X�ڂ̐�
#define STATUS_NOSTONE -1	//�Ֆʂɐ΂��u����Ă��Ȃ��Ƃ��̒l
#define EOP -1		//�p�^�[���I�[

//�\����
typedef struct boardData {	//�Ֆʂ̏��
	int status[Y_BOARD][X_BOARD];			//�΂̔z�u���
	int allocPosition[Y_BOARD][X_BOARD];	//�΂̔z�u�\�ꏊ�i���㗘�p�\��j
	int stoneNum = 0;
}Board;

typedef struct {	//�v���C���[���
	char name[100];
	unsigned int color;
	int stoneNum;
}Player;

typedef struct gameData{	//�Q�[���i�s�֘A
	int playerNum;	//�v���C���[�̐l��
	int turn;		//�ǂ̃v���C���[�̃^�[���ł��邩
	int isSkipped = 0;	//�X�L�b�v�������ǂ���
}Game;

//�v���g�^�C�v�錾
Board initBoard(void);
void drawBoard(int board[Y_BOARD][X_BOARD], int turn, Player* players);
void clearArray(int* array, int y, int x, int value);
void SetStone(int x, int y, unsigned int color);
int checkBoard(int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD], int turn, unsigned int pointColor);
void turnStone(int posX, int posY, int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD]);
int GetClickArea(int* posX, int* posY);
void changeTurn(int* turn);
void showMessage(const char* message);


//�O���[�o���ϐ�
int gr_direction[8][2] = {	//����
		{-1,-1},
		{-1,0},
		{-1,1},
		{0,-1},
		{0,1},
		{1,-1},
		{1,0},
		{1,1}
};

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);	//�E�B���h�E���[�h
	SetMainWindowText("���o�[�V");
	SetGraphMode(480, 530, 32);
	SetBackgroundColor(28, 24, 20);

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;				// �G���[���N�����璼���ɏI��
	}

	
	//�e��ϐ�
	Board board = initBoard();		//�ՖʂɊւ���l
	Player players[2] = {			//�v���C���[�Ɋւ���l
		{"��", GetColor(0,0,0)},
		{"��", GetColor(255,255,255)}
	};
	Game game = { PLAYER_NUM, 0, 0 };	//�Q�[���i�s�Ɋւ���l

	int i;		//���[�v�J�E���^
	int j;		//���[�v�J�E���^
	
	int* allocablePosition[8][8];	//�΂̔z�u�\�ꏊ
	for (i = 0; i < 64; i++) {		//�z�u�\�ꏊ�̊e�v�f�ɋ�|�C���^������
		allocablePosition[i / 8][i % 8] = NULL;
	}
	
	char resultStr[40];	//�A��������������i�[����o�b�t�@
	
	//�Q�[���i�s
	while (1) {	//�Q�[���i�s���[�v
		drawBoard(board.status, game.turn, players);	//��ʂ̕`��

		//�΂̔z�u�\�Ȉʒu�𔻒�E�\��
		if (checkBoard(board.status, allocablePosition, game.turn, players[game.turn].color) == -1) {
			//�΂�u����ꏊ���Ȃ������ꍇ
			if (game.isSkipped == 1) {	//�X�L�b�v���A��2��ڂ̏ꍇ
				showMessage("����ȏ�u���܂���");
				drawBoard(board.status, game.turn, players);
				break;
			}
			else {
				game.isSkipped = 1;
				showMessage("�u����ꏊ���Ȃ����߁A�p�X���܂�");
				drawBoard(board.status, game.turn, players);
				changeTurn(&game.turn);
				drawBoard(board.status, game.turn, players);
				continue;
			}
		}
		game.isSkipped = 0;

		while (1) {	//�Δz�u���胋�[�v
			//�΂��z�u�A�܂��̓X�L�b�v�����܂ŌJ��Ԃ��܂�
			int clickPosX;	//�N���b�N�ʒuX(0~7)
			int clickPosY;	//�N���b�N�ʒuY(0~7)
			if (GetClickArea(&clickPosX, &clickPosY) == -1) {	//�N���b�N���蒆�ɃE�B���h�E������ꂽ�܂��ُ͈킪�������ꍇ
				DxLib_End();	//�I������
				return 0;
			}

			if (allocablePosition[clickPosY][clickPosX] != NULL) {	//�N���b�N�����ꏊ���z�u�\�ȏꍇ
				board.status[clickPosY][clickPosX] = game.turn;
				turnStone(clickPosX, clickPosY, board.status, allocablePosition);
				board.stoneNum++;
				break;
			}

		}
		if (board.stoneNum == 64) {	//�Ֆʂ̂��ׂĂ̏ꏊ�ɐ΂��z�u���ꂽ�ꍇ
			drawBoard(board.status, game.turn, players);
			break;
		}

		//�^�[���`�F���W
		changeTurn(&game.turn);
	}
	
	//�΂𐔂���
	//���ʂƏ��s��\������
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			players[board.status[j][i]].stoneNum++;
		}
	}

	//���̕���2�l�v���C�p�Ɋȗ������Ă��܂�
	if (players[0].stoneNum > players[1].stoneNum) {
		sprintf_s(resultStr,40, "%2d��%-2d  %s�̏���", players[0].stoneNum, players[1].stoneNum, players[0].name);
		showMessage(resultStr);
	}
	else if (players[1].stoneNum > players[0].stoneNum) {
		sprintf_s(resultStr, 40, "%2d��%-2d  %s�̏���", players[0].stoneNum, players[1].stoneNum, players[1].name);
		showMessage(resultStr);
	}
	else {
		sprintf_s(resultStr, 40, "%2d��%-2d  ���������ł�", players[0].stoneNum, players[1].stoneNum);
		showMessage(resultStr);
	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}

Board initBoard(void) {
	Board board;
	
	//�Ֆʂ�������
	clearArray(&board.status[0][0], Y_BOARD, X_BOARD, STATUS_NOSTONE);
	clearArray(&board.allocPosition[0][0], Y_BOARD, X_BOARD, STATUS_NOSTONE);
	
	//�΂̏����z�u
	board.status[3][3] = 1;
	board.status[3][4] = 0;
	board.status[4][3] = 0;
	board.status[4][4] = 1;
	board.stoneNum = 4;

	return board;
}

//2�����z��̊e�v�f��value����
//int* array�ɂ́A�z��̐擪�v�f[0][0]�̃A�h���X���w�肷��
void clearArray(int* array, int y, int x, int value) {
	int i;	//���[�v�J�E���^
	int j;	//���[�v�J�E���^
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++) {
			 *(array + (y * i + j)) = value;
		}
	}

}

//�Ֆʂ̕`��
void drawBoard(int board[Y_BOARD][X_BOARD], int turn, Player* players) {
	int i;	//���[�v�J�E���^
	int j;	//���[�v�J�E���^
	int n;	//�z�u���W

	//���o�[�V�{�[�h�̕`��
	ClearDrawScreen();
	DrawBox(20, 20, 460, 460, GetColor(0, 128, 0), TRUE);
	for (i = 1, n = 75; i <= 7; i++, n += 55) {	//���o�[�V�r��
		DrawLine(n, 20, n, 460, GetColor(32, 32, 32));
		DrawLine(20, n, 460, n, GetColor(32, 32, 32));
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (board[i][j] != STATUS_NOSTONE) {	//�΂�����ꏊ�̏ꍇ
				SetStone(j, i, players[board[i][j]].color);
			}
		}
	}

	//���݂̃^�[����\��
	DrawCircleAA(47.5, 487.5, 26, 80, GetColor(128, 128, 128), TRUE);	//�w�i�����ɋ߂��̂ŁA���΂����₷���悤�Ɍ�������
	SetStone(0, 8, players[turn].color);	//���݂̐΂̐F��\��
	DrawFormatString(100, 480, GetColor(255, 255, 255),"%s�̃^�[��", players[turn].name);

	/*
	//�΂̏����z�u
	SetStone(3, 3, GetColor(255, 255, 255));
	SetStone(3, 4, GetColor(0, 0, 0));
	SetStone(4, 3, GetColor(0, 0, 0));
	SetStone(4, 4, GetColor(255, 255, 255));
	*/
}

//�΂̔z�u
void SetStone(int x, int y, unsigned int color) {
	DrawCircleAA(47.5 + 55 * x, 47.5 + 55 * y, 25, 80, color, TRUE);
}

//�΂̔z�u�\�Ȉʒu�𔻒�E�\��
int checkBoard(int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD], int turn, unsigned int pointColor) {
	
	int success = 0;	//�z�u�\�Ȉʒu�𔭌��ł������ǂ���
	int xPos;	//x���[�v�J�E���^
	int yPos;	//y���[�v�J�E���^
	int i;		//���[�v�J�E���^

	//�������J��
	for (xPos = 0; xPos < 8; xPos++) {
		for (yPos = 0; yPos < 8; yPos++) {
			if (allocablePosition[yPos][xPos] != NULL) {
				delete[] allocablePosition[yPos][xPos];
				allocablePosition[yPos][xPos] = NULL;
			}
		}
	}

	//�Ֆʂ̊e�}�X���Ƃɔ���
	for (xPos = 0; xPos < 8; xPos++) {
		for (yPos = 0; yPos < 8; yPos++) {
			
			//�}�X���J���Ă��邱�Ƃ��m�F
			if (board[yPos][xPos] != STATUS_NOSTONE) {	
				continue;
			}

			//�������ƂɌJ��Ԃ�
			for (i = 0; i < 8; i++) {
				int xChk = xPos;
				int yChk = yPos;

				int result;	//�z�u���،���
				result = 0;

				while (1) {	//�z�u�\�����𖞂����Ȃ��Ȃ�܂ŌJ��Ԃ�

					if (xChk < 0 || xChk > 7 || yChk < 0 || yChk > 7) {	//�Ֆʂ̒[�ɓ��B���Ă���ꍇ
						break;
					}

					//�m�F�ʒu��1�}�X�i�߂�
					xChk += gr_direction[i][0];
					yChk += gr_direction[i][1];

					if (board[yChk][xChk] != turn && board[yChk][xChk] != STATUS_NOSTONE) {
						//*��ԂƈقȂ�F�̐΂��אځE�A�����Ă���
						result = 1;
					}
					else {
						break;
					}

				}

				if (result == 1 && board[yChk][xChk] == turn) {	//�΂����ޏ����𖞂����Ă���ꍇ
					success = 1;
					if (allocablePosition[yPos][xPos] == NULL) {	//�p�^�[���i�[�p�z�񂪐�������Ă��Ȃ��ꍇ
						allocablePosition[yPos][xPos] = new int[8];
						(allocablePosition[yPos][xPos])[0] = EOP;
					}
					int j = 0;
					while ((allocablePosition[yPos][xPos])[j++] != EOP);	//�p�^�[���I�[��T��
					//�����̒l��ǉ�
					(allocablePosition[yPos][xPos])[j-1] = i;
					(allocablePosition[yPos][xPos])[j] = EOP;

					DrawCircleAA(47.5 + 55 * xPos, 47.5 + 55 * yPos, 6, 40, pointColor, TRUE);
				}
			}
		}
	}
	if (success == 1) {	//�z�u�\�Ȉʒu�����������ꍇ
		return 0;
	}
	else {	//�z�u�\�ȏꏊ�������ꍇ
		return -1;
	}
}

void turnStone(int posX, int posY, int board[Y_BOARD][X_BOARD], int* allocablePosition[Y_BOARD][X_BOARD]) {
	int i = 0;	//���[�v�J�E���^
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

//�N���b�N�ʒu�̎擾
int GetClickArea(int* posX, int* posY) {
	while (1) {	//�N���b�N���胋�[�v
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)	//�}�E�X���N���b�N����Ă���ꍇ
		{
			int mouseX;
			int mouseY;
			GetMousePoint(&mouseX, &mouseY);	//�}�E�X�̈ʒu���擾

			//�}�E�X�̍��W�͈̓`�F�b�N
			if (20 <= mouseX && mouseX < 460) {
				if (20 <= mouseY && mouseY < 460) {
					*posX = (mouseX - 20) / 55;
					*posY = (mouseY - 20) / 55;
					break;
				}
			}
		}

		//�v���Z�X���b�Z�[�W���� ���[�v�������Ă���ӏ����N���b�N�҂����炢�����������߂����ɔz�u
		if (ProcessMessage() == -1)
		{
			return -1;
		}
	}

	return 0;
}


//�^�[���ύX�i���̔��j
void changeTurn(int* turn) {
	//�^�[���`�F���W
	if (*turn == 0) {
		*turn = 1;
	}
	else {
		*turn = 0;
	}
}


//���b�Z�[�W����ʒ����ɕ\������
void showMessage(const char* message) {
	DrawBox(80, 160, 400, 320, GetColor(200, 200, 200), TRUE);
	DrawString(100, 210, message, GetColor(0,0,0));
	WaitTimer(500);
	DrawString(160, 280, "�N���b�N���ĕ���", GetColor(64, 64, 64));
	WaitKey();
	
}