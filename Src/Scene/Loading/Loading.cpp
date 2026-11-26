#include <string>
#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/FontManager.h"
#include "../../Application.h"
#include "Loading.h"

// コンストラクタ
Loading::Loading()
	: loadingFont_(-1)
	, loadTimer_(0)
	, isLoading_(false)
{
}

// 読み込み
void Loading::Load(void)
{
	// リソース取得
	auto& res = ResourceManager::GetInstance();
	res.Load(ResourceManager::SRC::FONT);

	// フォントハンドルの作成
	auto& font = FontManager::GetInstance();
	loadingFont_ = font.CreateMyFont(
		FONT_NAME, static_cast<int>(FONT_SIZE), static_cast<int>(FONT_THICKNESS));
}

// 更新
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

// 描画
void Loading::Draw(void)
{
	if (!isLoading_) return;

	// ロード中のドットアニメーションを作成
	int dotCount = (loadTimer_ / DOT_ANIM_INTERVAL) % DOT_ANIM_CYCLE;
	std::wstring text = 
		std::wstring(LOADING_TEXT) + std::wstring(dotCount, L'.');

	// 描画位置を計算
	const int x = static_cast<int>(Application::SCREEN_SIZE_X - FONT_ADJUST_X);
	const int y = static_cast<int>(Application::SCREEN_SIZE_Y - FONT_ADJUST_Y);

	// ロード中の文字列を描画
	DrawStringToHandle(
		x, y,
		text.c_str(),
		FONT_COLOR,
		loadingFont_);
}

// 解放
void Loading::Release(void)
{
	loadingFont_ = -1;
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;

	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

// 同期読み込みに切り替える
void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}