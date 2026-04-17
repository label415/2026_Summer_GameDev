#include <string>
#include <fstream>
#include "../../../../Application.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../Common/Collider/ColliderBase.h"
#include "EnemyRat.h"
#include "EnemyRobot.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(void)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	// エネミーのデータ読み込み
	LoadCsvData();
}

void EnemyManager::Update(void)
{
	for (auto& enemy : enemys_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	for (auto& enemy : enemys_)
	{
		enemy->Draw();
	}
}

void EnemyManager::Release(void)
{
	for (auto& enemy : enemys_)
	{
		enemy->Release();
	}
}

void EnemyManager::AddHitCollider(const ColliderBase* hitCollider)
{
	for (auto& enemy : enemys_)
	{
		enemy->AddHitCollider(hitCollider);
	}
}

void EnemyManager::LoadCsvData(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream(Application::PATH_CSV + L"EnemyData.csv");
	if (!ifs)
	{
		// エラーが発生
		return;
	}
	// ファイルを１行ずつ読み込む
	std::string line;// 1行の文字情報
	std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割
	bool isHeader = true;
	while (getline(ifs, line))
	{
		if (isHeader)
		{
			isHeader = false;
			continue;
		}
		// １行をカンマ区切りで分割
		strSplit = AsoUtility::Split(line, ',');
		EnemyBase* enemy = nullptr;
		// 構造体に合わせて読込データを格納
		EnemyBase::EnemyData data = EnemyBase::EnemyData();
		int idx1 = 0;
		// ID
		data.id = stoi(strSplit[idx1++]);
		// 種別
		data.type = static_cast<EnemyBase::TYPE>(stoi(strSplit[idx1++]));
		// HP
		data.hp = stoi(strSplit[idx1++]);
		// 初期座標
		data.defaultPos =
		{
		stof(strSplit[idx1++]),
		stof(strSplit[idx1++]),
		stof(strSplit[idx1++])
		};
		//移動可能範囲
		data.moveRadius = stoi(strSplit[idx1++]);

		// エネミー生成
		Create(data);
	}
	ifs.close();
}


EnemyBase* EnemyManager::Create(const EnemyBase::EnemyData& data)
{
	EnemyBase* enemy = nullptr;

	switch (data.type)
	{
	case EnemyBase::TYPE::RAT:
		enemy = new EnemyRat(data);
		break;
	case EnemyBase::TYPE::ROBOT:
		enemy = new EnemyRobot(data);
		break;
	default:
		break;
	}

	if(enemy != nullptr)
	{
		enemy->Init();
		enemys_.push_back(enemy);
	}


	return enemy;
}
