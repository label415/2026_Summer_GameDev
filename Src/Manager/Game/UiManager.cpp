#include "../../UI/UIBase.h"
#include "../../UI/ImgUI.h"
#include "UiManager.h"

void UiManager::Add(std::unique_ptr<UIBase> ui)
{
	// 有効なUIインスタンスを管理配列へ追加する
	if (!ui)
	{
		return;
	}
	uis_.push_back(std::move(ui));
}

void UiManager::Add(int id, std::unique_ptr<UIBase> ui)
{
	// 識別IDマップへ参照を登録した上で管理配列へ所有権を移す
	if (!ui)
	{
		return;
	}
	idMap_[id] = ui.get();
	uis_.push_back(std::move(ui));
}

void UiManager::Update(void)
{
	// 管理している全UIを一括更新する
	for (const auto& ui : uis_)
	{
		if (ui)
		{
			ui->Update();
		}
	}
}

void UiManager::Draw(void)
{
	// 管理している全UIを登録順に一括描画する
	for (const auto& ui : uis_)
	{
		if (ui)
		{
			ui->Draw();
		}
	}
}

void UiManager::Release(void)
{
	// 管理している全UIの解放処理を実行してからコンテナを空にする
	for (const auto& ui : uis_)
	{
		if (ui)
		{
			ui->Release();
		}
	}
	idMap_.clear();
	uis_.clear();
}

void UiManager::SetActive(int id, bool active)
{
	// 指定IDのUIが存在すれば表示有効状態を切り替える
	UIBase* ui = FindUI(id);
	if (ui)
	{
		ui->SetActive(active);
	}
}

void UiManager::SetPos(int id, Vector2F pos)
{
	// 指定IDのUIが存在すれば描画座標を更新する
	UIBase* ui = FindUI(id);
	if (ui)
	{
		ui->SetPos(pos);
	}
}

void UiManager::SetImg(int id, int img)
{
	// 指定IDのUIが存在すれば画像ハンドルを差し替える
	UIBase* ui = FindUI(id);
	if (ui)
	{
		ui->SetImg(img);
	}
}

void UiManager::SetAlpha(int id, int alpha)
{
	// 指定IDのUIが ImgUI であれば透過度を更新する
	ImgUI* imgUi = dynamic_cast<ImgUI*>(FindUI(id));
	if (imgUi)
	{
		imgUi->SetAlpha(alpha);
	}
}

UIBase* UiManager::FindUI(int id) const
{
	// IDマップからUIを検索して返す
	auto it = idMap_.find(id);
	if (it == idMap_.end())
	{
		return nullptr;
	}
	return it->second;
}
