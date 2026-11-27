#include <DxLib.h>
#include "../Manager/Common/ResourceManager.h"
#include "../Application.h"
#include "UIRecovery.h"

UIRecovery::UIRecovery(int initCnt)
	: ImgUI(
		ResourceManager::GetInstance().Load(ResourceManager::SRC::UI_ITEMBOX).handleId_,
		Vector2F(
			Application::SCREEN_SIZE.x / UI_POS_ADJ.x,
			Application::SCREEN_SIZE.y / UI_POS_ADJ.y),
		BOX_SIZE)
	, displayCount_(initCnt)
	, cntfont_(std::to_wstring(initCnt))
	, bottleImg_(nullptr)
	, font_(-1)
	, countGetter_(nullptr)
{
	// リソース管理から回復瓶画像とフォントハンドルを取得する
	auto& resMng = ResourceManager::GetInstance();
	int bottleHandle = resMng.Load(ResourceManager::SRC::UI_RECOVERY_BOTTLE).handleId_;
	font_ = resMng.LoadFont(ResourceManager::SRC::FONT, FONT_SIZE, FONT_SIZE);

	// 回復瓶用の画像UIを生成
	bottleImg_ = std::make_unique<ImgUI>(
		bottleHandle,
		pos_,
		BOTTLE_SIZE);
}

void UIRecovery::Update(void)
{
	// モデルがバインドされていれば最新の個数を表示へ反映する
	if (countGetter_)
	{
		SetBottleCnt(countGetter_());
	}
}

void UIRecovery::Draw(void)
{
	// 非表示時は描画しない
	if (!active_)
	{
		return;
	}

	// アイテムボックスを描画
	ImgUI::Draw();

	// 回復瓶を描画
	if (bottleImg_)
	{
		bottleImg_->Draw();
	}

	// 回復瓶の数を描画
	DrawFormatStringFToHandle(
		Application::SCREEN_SIZE.x / FONT_POS_ADJ.x,
		Application::SCREEN_SIZE.y / FONT_POS_ADJ.y,
		FONT_COLOR,
		font_,
		cntfont_.c_str());
}

void UIRecovery::Release(void)
{
	// 子UIの解放とフォントハンドル参照のリセット
	if (bottleImg_)
	{
		bottleImg_->Release();
	}
	font_ = -1;
}

void UIRecovery::BindBottleCnt(std::function<int(void)> countGetter)
{
	// モデル側の個数取得関数を登録し、表示を即時同期する
	countGetter_ = std::move(countGetter);
	if (countGetter_)
	{
		SetBottleCnt(countGetter_());
	}
}

void UIRecovery::SetBottleCnt(int cnt)
{
	// 表示値に変化があった場合のみ描画用文字列を再生成する
	if (displayCount_ == cnt)
	{
		return;
	}
	displayCount_ = cnt;
	cntfont_ = std::to_wstring(displayCount_);
}
