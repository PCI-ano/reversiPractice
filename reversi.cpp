#include "DxLib.h"

//�v���g�^�C�v�錾
void drawBoard(int board[][8], int turn, char* name, unsigned int colors[]);
void SetStone(int x, int y, unsigned int color);
int checkBoard(int board[][8], int* allocablePosition[][8], int turn, unsigned int pointColor);
void turnStone(int posX, int posY, int board[][8], int* allocablePosition[][8]);
int GetClickArea(int* posX, int* posY);
void changeTurn(int* turn);
void showMessage(const char* message);

#define EOP -1	//�p�^�[���I�[

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
	SetGraphMode(480, 530, 32);
	SetBackgroundColor(36, 36, 36);

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	//�e��ϐ�
	int i;		//���[�v�J�E���^
	int j;		//���[�v�J�E���^
	int turn = 1;		//��ԁi1����U[��]�A2����U[��]�j
	int stoneNum = 4;	//�΂̐�
	int skip = 0;		//�X�L�b�v���ꂽ���ǂ��� *2��A���X�L�b�v���ꂽ�ꍇ�I��
	int board[8][8] = {	//���o�[�V�{�[�h�i0���z�u,1��,2���j
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,2,1,0,0,0},
		{0,0,0,1,2,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	};
	int* allocablePosition[8][8];	//�΂̔z�u�\�ꏊ
	for (i = 0; i < 64; i++) {		//�z�u�\�ꏊ�̊e�v�f�ɋ�|�C���^������
		allocablePosition[i / 8][i % 8] = NULL;
	}
	int playerStoneNum[3] = { -1,0,0 };	//�v���C���[���Ƃ̐΂̐�

	//�F�i�ՖʁA���̐΁A���̐΁j�A�v���C���[��
	unsigned int gameColor[3] = { GetColor(0,128,0),GetColor(0,0,0),GetColor(255,255,255) };
	char playerName[3][40] = { "","��","��"};
	char resultStr[40];

	//�Ֆʂ̏����z�u��`��
	drawBoard(board, turn, playerName[turn], gameColor);
	
	//�Q�[���i�s
	while (1) {	//�Q�[���i�s���[�v
		//�΂̔z�u�\�Ȉʒu�𔻒�E�\��
		if (checkBoard(board, allocablePosition, turn, gameColor[turn]) == -1) {
			//�΂�u����ꏊ���Ȃ������ꍇ
			if (skip == 1) {	//�X�L�b�v���A��2��ڂ̏ꍇ
				showMessage("����ȏ�u���܂���");
				drawBoard(board, turn, playerName[turn], gameColor);
				break;
			}
			else {
				skip = 1;
				showMessage("�u����ꏊ���Ȃ����߁A�p�X���܂�");
				drawBoard(board, turn, playerName[turn], gameColor);
				changeTurn(&turn);
				drawBoard(board, turn, playerName[turn], gameColor);
				continue;
			}
		}
		skip = 0;

		while (1) {	//�Δz�u���胋�[�v
			//�΂��z�u�A�܂��̓X�L�b�v�����܂ŌJ��Ԃ��܂�
			int clickPosX;	//�N���b�N�ʒuX(0~7)
			int clickPosY;	//�N���b�N�ʒuY(0~7)
			if (GetClickArea(&clickPosX, &clickPosY) == -1) {	//�N���b�N���蒆�ɃE�B���h�E������ꂽ�܂��ُ͈킪�������ꍇ
				DxLib_End();	//�I������
				return 0;
			}

			if (allocablePosition[clickPosY][clickPosX] != NULL) {	//�N���b�N�����ꏊ���z�u�\�ȏꍇ
				board[clickPosY][clickPosX] = turn;
				turnStone(clickPosX, clickPosY, board, allocablePosition);
				stoneNum++;
				break;
			}

		}
		if (stoneNum == 64) {	//�Ֆʂ̂��ׂĂ̏ꏊ�ɐ΂��z�u���ꂽ�ꍇ
			drawBoard(board, turn, playerName[turn], gameColor);
			break;
		}

		//�^�[���`�F���W
		changeTurn(&turn);
		drawBoard(board, turn, playerName[turn], gameColor);
	}
	
	//�΂𐔂���
	//���ʂƏ��s��\������
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			playerStoneNum[board[j][i]]++;
		}
	}

	//���̕���2�l�v���C�p�Ɋȗ������Ă��܂�
	if (playerStoneNum[1] > playerStoneNum[2]) {
		sprintf_s(resultStr,40, "%2d��%-2d  %s�̏���", playerStoneNum[1], playerStoneNum[2], playerName[1]);
		showMessage(resultStr);
	}
	else if (playerStoneNum[2] > playerStoneNum[1]) {
		sprintf_s(resultStr, 40, "%2d��%-2d  %s�̏���", playerStoneNum[1], playerStoneNum[2], playerName[2]);
		showMessage(resultStr);
	}
	else {
		sprintf_s(resultStr, 40, "%2d��%-2d  ���������ł�", playerStoneNum[1], playerStoneNum[2]);
		showMessage(resultStr);
	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}


//�Ֆʂ̏����� ����int* board�c�Ֆʔz��̋N�_[0][0]�̃A�h���X
void drawBoard(int board[][8], int turn, char* name, unsigned int colors[]) {
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
			if (board[i][j] != 0) {	//�΂�����ꏊ�̏ꍇ
				SetStone(j, i, colors[board[i][j]]);
			}
		}
	}

	//���݂̃^�[����\��
	SetStone(0, 8, colors[turn]);
	DrawFormatString(100, 480, GetColor(255, 255, 255),"%s�̃^�[��", name);

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
int checkBoard(int board[][8], int* allocablePosition[][8], int turn, unsigned int pointColor) {
	
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
			if (board[yPos][xPos] != 0) {	
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

					if (board[yChk][xChk] != turn && board[yChk][xChk] != 0) {
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

void turnStone(int posX, int posY, int board[][8], int* allocablePosition[][8]) {
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
	if (*turn == 1) {
		*turn = 2;
	}
	else {
		*turn = 1;
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