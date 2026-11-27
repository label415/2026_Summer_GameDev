#include <string>
#include <DxLib.h>
#include "../../Manager/Common/ResourceManager.h"
#include "../../Application.h"
#include "Loading.h"

Loading::Loading(void)
	: loadTimer_(0)
	, isLoading_(false)
{
}

void Loading::Update(void)
{
	if (!isLoading_) return;

	loadTimer_++;

	// 読込中のものがなくなったら or 最低ロード時間経過
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME)
	{
		loadTimer_ = 0;
		isLoading_ = false;
	}
}

void Loading::Draw(void)
{
	if (!isLoading_) return;

	// ロード中のドットアニメーションを作成
	int dotCount = (loadTimer_ / DOT_ANIM_INTERVAL) % DOT_ANIM_CYCLE;
	std::wstring text = 
		std::wstring(LOADING_TEXT) + std::wstring(dotCount, L'.');

	// 描画位置を計算
	const Vector2F pos = {
		Application::SCREEN_SIZE.x - FONT_ADJUST.x,
		Application::SCREEN_SIZE.y - FONT_ADJUST.y
	};

	// ロード中の文字列を描画
	DrawStringFToHandle(
		pos.x, pos.y,
		text.c_str(),
		FONT_COLOR,
		ResourceManager::GetInstance().LoadFont(
			ResourceManager::SRC::FONT, FONT_SIZE, FONT_THICKNESS));
}

void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;

	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}