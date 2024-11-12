#include<iostream>

namespace
{
	//データ化
	const struct kData
	{
		//データの名前
		const char* name;
		//体力
		int hp;
	};

	//データの種類
	enum kObjectKind
	{
		Player,		//プレイヤー
		Enemy,		//エネミー
		Shot,		//ショット
		Field,		//フィールド

		KindNum,	//データの数
	};

	//データ格納
	const kData data[KindNum] = 
	{
		//プレイヤー
		{ "Playerデータ", 100 },
		//エネミー
		{ "Enemyデータ", 50 },
		//ショット
		{ "Shotデータ", 10 },
		//フィールド
		{ "Fieldデータ", 1000 },
	};
}

//データ関数
kData GetData(kObjectKind kind)
{
	//データの種類によってデータを返す
	switch (kind)
	{
		case kObjectKind::Player:	
			return { "Playerデータ", 100 };
		case kObjectKind::Enemy:
			return { "Enemyデータ", 50 };
		case kObjectKind::Shot:		
			return { "Shotデータ", 10 };
		case kObjectKind::Field:		
			return { "Fieldデータ", 1000 };
		default:					
			return { "データがありません", 0 };
	}
}

int main()
{
	//////////////////////////////////////
	// 関数を使って条件分岐を減らす(分かりやすくする)
	//////////////////////////////////////
	//プレイヤーのデータを関数で取得
	std::cout << GetData(kObjectKind::Player).name << "体力:" << GetData(kObjectKind::Player).hp << std::endl;


	//////////////////////////////////////
	// データの種類によってデータを返す
	//////////////////////////////////////
	//プレイヤーのデータ元のデータから取得
	std::cout << data[kObjectKind::Player].name << "体力:" << data[kObjectKind::Player].hp << std::endl;

	return 0;
}