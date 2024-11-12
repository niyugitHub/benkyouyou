#include<iostream>
#include<DxLib.h>
#include <cassert>
#include<vector>

namespace
{
	constexpr int kSizeX = 10;
	constexpr int kSizeY = 10;
}

//�X�e�C�g���
enum State
{
	None,
	Open,
	Closed,
};

//�m�[�h�f�[�^�̍\����
struct NodeData
{
	int IndexX;	//X���W
	int IndexY;	//Y���W
	int PosX;	//X���W
	int PosY;	//Y���W
	int Cost;	//�R�X�g
	int EstimatedCost;	//����R�X�g
	int Score;			//�X�R�A
	bool IsKabe;		//�ǂ��ǂ���

	State state;		//���

	std::vector<int> m_rootX;//�ǂ̃��[�g�����ǂ��Ă����̂�
	std::vector<int> m_rootY;//�ǂ̃��[�g�����ǂ��Ă����̂�

	//�S�[������T���������ǂ邽�߂̕ϐ�
	int findMasX = 0;	//���̃}�X�������Ă��ꂽ�}�X
	int findMasY = 0;	//���̃}�X�������Ă��ꂽ�}�X
};

//���[�g�f�[�^�̍\����
struct routeData
{
	int PosX;	//X���W
	int PosY;	//Y���W

	bool isPass = false;	//�����܂ł��ǂ蒅�������ǂ���
	bool isGoal = false;	//�S�[���}�X���ǂ���
};

//�v���C���[�̍\����
struct player
{
	int PosX;
	int PosY;

	bool isGoal = false;
};

//�T��
bool RouteSearch(int startX, int startY, int goalX, int goalY);
//���Z�b�g
void ResetSearch();
//����R�X�g�v�Z
int CalculationEstimatedCost(int nowX, int nowY, int goalX, int goalY);
//�v���C���[������
void PlayerInit();
//�v���C���[�X�V
void PlayerUpdate();
//�T�����[�g�N���A
void RouteClear();
//�T�����[�g�X�V
void RouteUpdate();
//�X�^�[�g����S�[���܂ł̃m�[�h�f�[�^�N���A
void StartToGoalNodeDataClear();
//�X�^�[�g����S�[���܂ł̃m�[�h�f�[�^�쐬
void StartToGoalNodeDataCreate();
//�}�b�v�`��
void MapDraw();
//�X�^�[�g�ʒu�X�V
void UpdateStart(int& startX, int& startY);
//�S�[���ʒu�X�V
void UpdateGoal(int& goalX, int& goalY);
//�I�[�v������
void OpenProcess(NodeData* nowData, int goalX, int goalY);
//�ǂ�p�����R�X�g�v�Z
void CostIsKabe(NodeData nowData, NodeData goolData);


//�O���[�o���ϐ�
int masSize = 50;	//�}�b�v�T�C�Y
int masCenter = masSize * 0.5f;	//�}�b�v�T�C�Y�̔����̃T�C�Y
int startX = 2;					//X���̃X�^�[�g�ʒu
int startY = 3;					//Y���̃X�^�[�g�ʒu
int goalX = 0;					//X���̃S�[���ʒu
int goalY = 3;					//Y���̃S�[���ʒu
std::vector<routeData> startToGoalRoute;	//�X�^�[�g����S�[���܂ł̃��[�g
player Player;					//�v���C���[
NodeData data[kSizeX][kSizeY]{};//�m�[�h�f�[�^
//�X�^�[�g����S�[���܂ł̃m�[�h�f�[�^
std::vector<NodeData> startToGoalData;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �E�B���h�E���[�h�ݒ�
	ChangeWindowMode(true);
	// �E�B���h�E���[�h�̃T�C�Y�ύX
	SetWindowSizeChangeEnableFlag(true);
	// �E�B���h�E���ݒ�
	SetMainWindowText(L"Game::kTitleText");
	// ��ʃT�C�Y�̐ݒ�
	SetGraphMode(640, 480, 32);
	// �_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);
	// XAudio ��L����
	SetEnableXAudioFlag(TRUE);

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	//�l��������
	ResetSearch();
	RouteClear();
	StartToGoalNodeDataClear();

	//�T�����J�n
	if (RouteSearch(startX, startY, goalX, goalY))
	{
		//�X�^�[�g����S�[���܂ł̃m�[�h�f�[�^
		StartToGoalNodeDataCreate();

		//���[�g�X�V
		RouteUpdate();
	}
	//�v���C���[�̏�����
	PlayerInit();


	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();

		// ��ʂ̃N���A
		ClearDrawScreen();

		UpdateStart(startX, startY);
		UpdateGoal(goalX, goalY);

		if (CheckHitKey(KEY_INPUT_1))
		{
			//���Z�b�g
			ResetSearch();
			RouteClear();
			StartToGoalNodeDataClear();
			//�T�����J�n
			if (RouteSearch(startX, startY, goalX, goalY))
			{
				//�X�^�[�g����S�[���܂ł̃m�[�h�f�[�^
				StartToGoalNodeDataCreate();

				//���[�g�X�V
				RouteUpdate();
			}

			//�v���C���[�̏�����
			PlayerInit();
		}

		//�v���C���[�X�V
		PlayerUpdate();
		
		//�}�b�v�`��
		MapDraw();

		//�v���C���[�`��
		DrawCircle(Player.PosX, Player.PosY, 10, 0xffffff, true);

		// ����ʂ�\��ʂƓ���ւ���
		ScreenFlip();

		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}
	}

	// DX���C�u�����I������
	DxLib_End();

	return 0;
}

bool RouteSearch(int startX, int startY, int goalX, int goalY)
{
	//�X�^�[�g�n�_�������̓S�[���n�_���ǂ̏ꍇassert����
	assert(!data[startX][startY].IsKabe);
	assert(!data[goalX][goalY].IsKabe);

	//���݌��Ă���}�X
	int lookMasX = startX;
	int lookMasY = startY;

	//�X�^�[�g�n�_�ݒ�
	data[lookMasX][lookMasY].Cost = 0;
	//����R�X�g�v�Z
	data[lookMasX][lookMasY].EstimatedCost = CalculationEstimatedCost(lookMasX, lookMasY, goalX, goalY);
	//�X�R�A�v�Z
	data[lookMasX][lookMasY].Score = data[lookMasX][lookMasY].Cost + data[lookMasX][lookMasY].EstimatedCost;
	
	data[lookMasX][lookMasY].state = Open;

	/*data[lookMasX][lookMasY].m_rootX.push_back(lookMasX);
	data[lookMasX][lookMasY].m_rootY.push_back(lookMasY);*/

	//��ԏ������X�R�A���i�[(�ŏ��̓X�^�[�g�n�_�ɐݒ�)
	NodeData* minScoreData = &data[lookMasX][lookMasY];

	OpenProcess(minScoreData, goalX, goalY);

	for(int k = 0; k < 10000; k++)
	{
		int minScore = 100;
		//�}�b�v�쐬
		for (int i = 0; i < kSizeX; i++)
		{
			for (int j = 0; j < kSizeY; j++)
			{
				//�ǂ̏ꍇ�͏������΂�
				if (data[i][j].IsKabe) continue;
				//�I�[�v����Ԃ̎����������s
				if (data[i][j].state == Open && minScore > data[i][j].Score)
				{
					minScoreData = &data[i][j];
					minScore = minScoreData->Score;
				}
			}
		}
		OpenProcess(minScoreData, goalX, goalY);

		//�S�[��������������true��Ԃ�
		if (data[goalX][goalY].state == Closed)
		{
			return true;
		}
	}

	//�����܂ŗ�����S�[����������Ȃ������Ƃ�
	return false;
}

void ResetSearch()
{
	//�l��������
	for (int i = 0; i < kSizeX; i++)
	{
		for (int j = 0; j < kSizeY; j++)
		{
			data[i][j].IndexX = i;
			data[i][j].IndexY = j;
			data[i][j].PosX = i * masSize;
			data[i][j].PosY = j * masSize;
			data[i][j].Cost = 0;
			data[i][j].EstimatedCost = 0;
			data[i][j].Score = 0;
			data[i][j].IsKabe = false;
			data[i][j].state = None;
		}
	}

	//�ǐݒ�
	for (int i = 0; i < kSizeX; i++)
	{
		for (int j = 0; j < kSizeY; j++)
		{
			int randomNum = GetRand(2);

			if (randomNum == 0)
			{
				data[i][j].IsKabe = true;
			}
		
			else
			{
				data[i][j].IsKabe = false;
			}
		}
	}

	//�X�^�[�g�ƃS�[���͕ǂ���Ȃ�����
	data[startX][startY].IsKabe = false;
	data[goalX][goalY].IsKabe = false;
}

int CalculationEstimatedCost(int nowX, int nowY, int goalX, int goalY)
{
	int scoreX = std::abs(goalX - nowX);
	int scoreY = std::abs(goalY - nowY);

	//�傫���ق��̒l���X�R�A�ɓ����
	int score = max(scoreX, scoreY);

	return score;
}

void PlayerInit()
{
	Player.PosX = data[startX][startY].IndexX * masSize;
	Player.PosY = data[startX][startY].IndexY * masSize;
	Player.isGoal = false;
}

void PlayerUpdate()
{
	for (int i = 0; i < startToGoalRoute.size(); i++)
	{
		if (Player.isGoal) break;
		//�ʂ����}�X�Ȃ珈�����΂�
		if (startToGoalRoute[i].isPass) continue;

		if (Player.PosX < startToGoalRoute[i].PosX + masCenter)
		{
			Player.PosX++;
		}
		else if (Player.PosX > startToGoalRoute[i].PosX + masCenter)
		{
			Player.PosX--;
		}
		if (Player.PosY < startToGoalRoute[i].PosY + masCenter)
		{
			Player.PosY++;
		}
		else if (Player.PosY > startToGoalRoute[i].PosY + masCenter)
		{
			Player.PosY--;
		}

		int difPosX = Player.PosX - startToGoalRoute[i].PosX - masCenter;
		int difPosY = Player.PosY - startToGoalRoute[i].PosY - masCenter;

		//�v���C���[�̍��W�Ǝw��̃}�X��XY���W�̍���
		//�Ƃ���10�ȉ��̏ꍇ
		if (fabs(difPosX) < 10 && fabs(difPosY) < 10)
		{
			startToGoalRoute[i].isPass = true;

			//�S�[���}�X�ł������ꍇ�v���C���[���S�[����Ԃɂ���
			if (startToGoalRoute[i].isGoal)
			{
				Player.isGoal = true;
			}
			break;
		}
		break;
	}
}

void RouteClear()
{
	startToGoalRoute.clear();
}

void RouteUpdate()
{
	for (int i = startToGoalData.size() - 1; i >= 0; i--)
	{
		routeData data;
		data.isPass = false;
		data.PosX = startToGoalData[i].IndexX * masSize;
		data.PosY = startToGoalData[i].IndexY * masSize;

		//�S�[���}�X�������ꍇ
		if (i == 0)
		{
			data.isGoal = true;
		}
		startToGoalRoute.push_back(data);
	}
}

void StartToGoalNodeDataClear()
{
	startToGoalData.clear();
}

void StartToGoalNodeDataCreate()
{
	NodeData DATA = data[goalX][goalY];

	while (true)
	{
		startToGoalData.push_back(DATA);
		if (DATA.IndexX == data[startX][startY].IndexX &&
			DATA.IndexY == data[startX][startY].IndexY)
		{
			break;
		}
		DATA = data[DATA.findMasX][DATA.findMasY];
	}
}

void MapDraw()
{
	//�}�b�v�쐬
	for (int i = 0; i < kSizeX; i++)
	{
		for (int j = 0; j < kSizeY; j++)
		{
			if (i == startX && j == startY)
			{
				DrawBoxAA(i * masSize, j * masSize, i * masSize + 45, j * masSize + 45, 0xff0000, true);
			}
			else if (i == goalX && j == goalY)
			{
				DrawBoxAA(i * masSize, j * masSize, i * masSize + 45, j * masSize + 45, 0x0000ff, true);
			}
			else if (data[i][j].IsKabe)
			{
				DrawBoxAA(i * masSize, j * masSize, i * masSize + 45, j * masSize + 45, 0x333333, true);
			}
			else
			{
				DrawBoxAA(i * masSize, j * masSize, i * masSize + 45, j * masSize + 45, 0xffffff, true);
			}


			DrawFormatString(i * masSize + 5, j * masSize + 5, 0x000000, L"%d", data[i][j].Cost);
			DrawFormatString(i * masSize + 5, j * masSize + 25, 0x000000, L"%d", data[i][j].Score);
		}
	}

	for (int i = 0; i < startToGoalData.size(); i++)
	{
		if (startToGoalData[i].IndexX == startX && startToGoalData[i].IndexY == startY)continue;
		else if (startToGoalData[i].IndexX == goalX && startToGoalData[i].IndexY == goalY)continue;

		DrawBoxAA(startToGoalData[i].IndexX * masSize, startToGoalData[i].IndexY * masSize, startToGoalData[i].IndexX * masSize + 45, startToGoalData[i].IndexY * masSize + 45, 0x888888, true);

		DrawFormatString(startToGoalData[i].IndexX * masSize + 5, startToGoalData[i].IndexY * masSize + 5, 0x000000, L"%d", startToGoalData[i].Cost);
		DrawFormatString(startToGoalData[i].IndexX * masSize + 5, startToGoalData[i].IndexY * masSize + 25, 0x000000, L"%d", startToGoalData[i].Score);
	}
}

void UpdateStart(int& startX, int& startY)
{
	if (CheckHitKey(KEY_INPUT_LEFT) && startX > 0)
	{
		startX -= 1;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT) && startX < kSizeX - 1)
	{
		startX += 1;
	}
	if (CheckHitKey(KEY_INPUT_UP) && startY > 0)
	{
		startY -= 1;
	}
	if (CheckHitKey(KEY_INPUT_DOWN) && startY < kSizeY - 1)
	{
		startY += 1;
	}
}

void UpdateGoal(int& goalX, int& goalY)
{
	if (CheckHitKey(KEY_INPUT_A) && goalX > 0)
	{
		goalX -= 1;
	}
	if (CheckHitKey(KEY_INPUT_D) && goalX < kSizeX - 1)
	{
		goalX += 1;
	}
	if (CheckHitKey(KEY_INPUT_W) && goalY > 0)
	{
		goalY -= 1;
	}
	if (CheckHitKey(KEY_INPUT_S) && goalY < kSizeY - 1)
	{
		goalY += 1;
	}
}

void OpenProcess(NodeData* nowData, int goalX, int goalY)
{
	nowData->state = Closed;


	//���Ɍ���f�[�^
	NodeData* nextData = nullptr;
	int minScore = 100;
	for (int i = nowData->IndexX - 1; i <= nowData->IndexX + 1; i++)
	{
		for (int j = nowData->IndexY - 1; j <= nowData->IndexY + 1; j++)
		{
			//���Ă���ꏊ�������ꍇ�������X�L�b�v
			if (i == nowData->IndexX && j == nowData->IndexY) continue;
			//i��j��0��菬�����ꍇ�������X�L�b�v
			if (i < 0 || j < 0) continue;
			//i��j��5���傫���ꍇ�������X�L�b�v
			if (i >= kSizeX || j >= kSizeY)continue;
			//�ǔ��肳��Ă���
			if (data[i][j].IsKabe) continue;
			//�X�e�C�g��None�ȊO�̏ꍇ�͏������X�L�b�v
			if (data[i][j].state != None) continue;

			//�R�X�g�v�Z
			data[i][j].Cost = data[nowData->IndexX][nowData->IndexY].Cost + 1;
			//����R�X�g�v�Z
			data[i][j].EstimatedCost = CalculationEstimatedCost(i, j, goalX, goalY);
			//�X�R�A�v�Z
			data[i][j].Score = data[i][j].Cost + data[i][j].EstimatedCost;

			if (minScore >= data[i][j].Score)
			{
				//�I�[�v����Ԃɂ���
				if (nextData != nullptr)
				{
					nextData->state = Open;
				}
				nextData = &data[i][j];
				nextData->findMasX = nowData->IndexX;
				nextData->findMasY = nowData->IndexY;
				minScore = nextData->Score;
			}
		}
	}

	if (nextData == nullptr) return;

	//�I�[�v����Ԃɂ���
	nextData->state = Open;
	//nextData->Cost += 1;
}

void CostIsKabe(NodeData nowData, NodeData goolData)
{
	//�Ԃɕǂ����邩�ǂ���
	/*for (int i = nowData.X; i < goolData.X; i++)
	{
		if()
	}*/
}

//void OpenProcess(int nowX, int nowY, int goalX, int goalY)
//{
//	for (int i = nowX - 1; i <= nowX + 1; i++)
//	{
//		for (int j = nowY - 1; j <= nowY + 1; j++)
//		{
//			//���Ă���ꏊ�������ꍇ�������X�L�b�v
//			if (i == nowX && j == nowY) continue;
//			//i��j��0��菬�����ꍇ�������X�L�b�v
//			if (i < 0 || j < 0) continue;
//			//i��j��5���傫���ꍇ�������X�L�b�v
//			if (i >= kSizeX || j >= kSizeY)continue;
//			//�ǔ��肳��Ă���
//			if (data[i][j].IsKabe) continue;
//			//�X�e�C�g��None�ȊO�̏ꍇ�͏������X�L�b�v
//			if (data[i][j].state != None) continue;
//
//			//�������珈�����J�n
//			//�X�e�C�g��Open�ɂ���
//			if (i == goalX && j == goalY)
//			{
//				data[i][j].state = None;
//			}
//			else
//			{
//				data[i][j].state = Open;
//			}
//			/*data[i][j].state = Open;*/
//
//			//�X�^�[�g�n�_�ݒ�
//			data[i][j].Cost = data[nowX][nowY].Cost + 1;
//			//����R�X�g�v�Z
//			data[i][j].EstimatedCost = CalculationEstimatedCost(i, j, goalX, goalY);
//			//�X�R�A�v�Z
//			data[i][j].Score = data[i][j].Cost + data[i][j].EstimatedCost;
//
//			data[i][j].m_rootX.clear();
//			data[i][j].m_rootY.clear();
//
//			data[i][j].m_rootX = data[nowX][nowY].m_rootX;
//			data[i][j].m_rootY = data[nowX][nowY].m_rootY;
//
//			data[i][j].m_rootX.push_back(i);
//			data[i][j].m_rootY.push_back(j);
//
//			if (i == goalX && j == goalY)
//			{
//				finalRootX.push_back(data[goalX][goalY].m_rootX);
//				finalRootY.push_back(data[goalX][goalY].m_rootY);
//			}
//		}
//	}
//}