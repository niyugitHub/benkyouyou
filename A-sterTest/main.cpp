#include<iostream>
#include<DxLib.h>
#include <cassert>
#include<vector>

namespace
{
	constexpr int kSizeX = 10;
	constexpr int kSizeY = 10;
}

//ステイト状態
enum State
{
	None,
	Open,
	Closed,
};

//ノードデータの構造体
struct NodeData
{
	int IndexX;	//X座標
	int IndexY;	//Y座標
	int PosX;	//X座標
	int PosY;	//Y座標
	int Cost;	//コスト
	int EstimatedCost;	//推定コスト
	int Score;			//スコア
	bool IsKabe;		//壁かどうか

	State state;		//状態

	std::vector<int> m_rootX;//どのルートをたどってきたのか
	std::vector<int> m_rootY;//どのルートをたどってきたのか

	//ゴールから探索元をたどるための変数
	int findMasX = 0;	//そのマスを見つけてくれたマス
	int findMasY = 0;	//そのマスを見つけてくれたマス
};

//ルートデータの構造体
struct routeData
{
	int PosX;	//X座標
	int PosY;	//Y座標

	bool isPass = false;	//そこまでたどり着いたかどうか
	bool isGoal = false;	//ゴールマスかどうか
};

//プレイヤーの構造体
struct player
{
	int PosX;
	int PosY;

	bool isGoal = false;
};

//探索
bool RouteSearch(int startX, int startY, int goalX, int goalY);
//リセット
void ResetSearch();
//推定コスト計算
int CalculationEstimatedCost(int nowX, int nowY, int goalX, int goalY);
//プレイヤー初期化
void PlayerInit();
//プレイヤー更新
void PlayerUpdate();
//探索ルートクリア
void RouteClear();
//探索ルート更新
void RouteUpdate();
//スタートからゴールまでのノードデータクリア
void StartToGoalNodeDataClear();
//スタートからゴールまでのノードデータ作成
void StartToGoalNodeDataCreate();
//マップ描画
void MapDraw();
//スタート位置更新
void UpdateStart(int& startX, int& startY);
//ゴール位置更新
void UpdateGoal(int& goalX, int& goalY);
//オープン処理
void OpenProcess(NodeData* nowData, int goalX, int goalY);
//壁を用いたコスト計算
void CostIsKabe(NodeData nowData, NodeData goolData);


//グローバル変数
int masSize = 50;	//マップサイズ
int masCenter = masSize * 0.5f;	//マップサイズの半分のサイズ
int startX = 2;					//X軸のスタート位置
int startY = 3;					//Y軸のスタート位置
int goalX = 0;					//X軸のゴール位置
int goalY = 3;					//Y軸のゴール位置
std::vector<routeData> startToGoalRoute;	//スタートからゴールまでのルート
player Player;					//プレイヤー
NodeData data[kSizeX][kSizeY]{};//ノードデータ
//スタートからゴールまでのノードデータ
std::vector<NodeData> startToGoalData;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ウィンドウモード設定
	ChangeWindowMode(true);
	// ウィンドウモードのサイズ変更
	SetWindowSizeChangeEnableFlag(true);
	// ウィンドウ名設定
	SetMainWindowText(L"Game::kTitleText");
	// 画面サイズの設定
	SetGraphMode(640, 480, 32);
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);
	// XAudio を有効化
	SetEnableXAudioFlag(TRUE);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	//値を初期化
	ResetSearch();
	RouteClear();
	StartToGoalNodeDataClear();

	//探索を開始
	if (RouteSearch(startX, startY, goalX, goalY))
	{
		//スタートからゴールまでのノードデータ
		StartToGoalNodeDataCreate();

		//ルート更新
		RouteUpdate();
	}
	//プレイヤーの初期化
	PlayerInit();


	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();

		// 画面のクリア
		ClearDrawScreen();

		UpdateStart(startX, startY);
		UpdateGoal(goalX, goalY);

		if (CheckHitKey(KEY_INPUT_1))
		{
			//リセット
			ResetSearch();
			RouteClear();
			StartToGoalNodeDataClear();
			//探索を開始
			if (RouteSearch(startX, startY, goalX, goalY))
			{
				//スタートからゴールまでのノードデータ
				StartToGoalNodeDataCreate();

				//ルート更新
				RouteUpdate();
			}

			//プレイヤーの初期化
			PlayerInit();
		}

		//プレイヤー更新
		PlayerUpdate();
		
		//マップ描画
		MapDraw();

		//プレイヤー描画
		DrawCircle(Player.PosX, Player.PosY, 10, 0xffffff, true);

		// 裏画面を表画面と入れ替える
		ScreenFlip();

		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}
	}

	// DXライブラリ終了処理
	DxLib_End();

	return 0;
}

bool RouteSearch(int startX, int startY, int goalX, int goalY)
{
	//スタート地点もしくはゴール地点が壁の場合assert処理
	assert(!data[startX][startY].IsKabe);
	assert(!data[goalX][goalY].IsKabe);

	//現在見ているマス
	int lookMasX = startX;
	int lookMasY = startY;

	//スタート地点設定
	data[lookMasX][lookMasY].Cost = 0;
	//推定コスト計算
	data[lookMasX][lookMasY].EstimatedCost = CalculationEstimatedCost(lookMasX, lookMasY, goalX, goalY);
	//スコア計算
	data[lookMasX][lookMasY].Score = data[lookMasX][lookMasY].Cost + data[lookMasX][lookMasY].EstimatedCost;
	
	data[lookMasX][lookMasY].state = Open;

	/*data[lookMasX][lookMasY].m_rootX.push_back(lookMasX);
	data[lookMasX][lookMasY].m_rootY.push_back(lookMasY);*/

	//一番小さいスコアを格納(最初はスタート地点に設定)
	NodeData* minScoreData = &data[lookMasX][lookMasY];

	OpenProcess(minScoreData, goalX, goalY);

	for(int k = 0; k < 10000; k++)
	{
		int minScore = 100;
		//マップ作成
		for (int i = 0; i < kSizeX; i++)
		{
			for (int j = 0; j < kSizeY; j++)
			{
				//壁の場合は処理を飛ばす
				if (data[i][j].IsKabe) continue;
				//オープン状態の時処理を実行
				if (data[i][j].state == Open && minScore > data[i][j].Score)
				{
					minScoreData = &data[i][j];
					minScore = minScoreData->Score;
				}
			}
		}
		OpenProcess(minScoreData, goalX, goalY);

		//ゴールが見つかったらtrueを返す
		if (data[goalX][goalY].state == Closed)
		{
			return true;
		}
	}

	//ここまで来たらゴールが見つからなかったとき
	return false;
}

void ResetSearch()
{
	//値を初期化
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

	//壁設定
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

	//スタートとゴールは壁じゃなくする
	data[startX][startY].IsKabe = false;
	data[goalX][goalY].IsKabe = false;
}

int CalculationEstimatedCost(int nowX, int nowY, int goalX, int goalY)
{
	int scoreX = std::abs(goalX - nowX);
	int scoreY = std::abs(goalY - nowY);

	//大きいほうの値をスコアに入れる
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
		//通ったマスなら処理を飛ばす
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

		//プレイヤーの座標と指定のマスのXY座標の差が
		//ともに10以下の場合
		if (fabs(difPosX) < 10 && fabs(difPosY) < 10)
		{
			startToGoalRoute[i].isPass = true;

			//ゴールマスであった場合プレイヤーもゴール状態にする
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

		//ゴールマスだった場合
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
	//マップ作成
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


	//次に見るデータ
	NodeData* nextData = nullptr;
	int minScore = 100;
	for (int i = nowData->IndexX - 1; i <= nowData->IndexX + 1; i++)
	{
		for (int j = nowData->IndexY - 1; j <= nowData->IndexY + 1; j++)
		{
			//見ている場所が同じ場合処理をスキップ
			if (i == nowData->IndexX && j == nowData->IndexY) continue;
			//iかjが0より小さい場合処理をスキップ
			if (i < 0 || j < 0) continue;
			//iかjが5より大きい場合処理をスキップ
			if (i >= kSizeX || j >= kSizeY)continue;
			//壁判定されている
			if (data[i][j].IsKabe) continue;
			//ステイトがNone以外の場合は処理をスキップ
			if (data[i][j].state != None) continue;

			//コスト計算
			data[i][j].Cost = data[nowData->IndexX][nowData->IndexY].Cost + 1;
			//推定コスト計算
			data[i][j].EstimatedCost = CalculationEstimatedCost(i, j, goalX, goalY);
			//スコア計算
			data[i][j].Score = data[i][j].Cost + data[i][j].EstimatedCost;

			if (minScore >= data[i][j].Score)
			{
				//オープン状態にする
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

	//オープン状態にする
	nextData->state = Open;
	//nextData->Cost += 1;
}

void CostIsKabe(NodeData nowData, NodeData goolData)
{
	//間に壁があるかどうか
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
//			//見ている場所が同じ場合処理をスキップ
//			if (i == nowX && j == nowY) continue;
//			//iかjが0より小さい場合処理をスキップ
//			if (i < 0 || j < 0) continue;
//			//iかjが5より大きい場合処理をスキップ
//			if (i >= kSizeX || j >= kSizeY)continue;
//			//壁判定されている
//			if (data[i][j].IsKabe) continue;
//			//ステイトがNone以外の場合は処理をスキップ
//			if (data[i][j].state != None) continue;
//
//			//ここから処理を開始
//			//ステイトをOpenにする
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
//			//スタート地点設定
//			data[i][j].Cost = data[nowX][nowY].Cost + 1;
//			//推定コスト計算
//			data[i][j].EstimatedCost = CalculationEstimatedCost(i, j, goalX, goalY);
//			//スコア計算
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