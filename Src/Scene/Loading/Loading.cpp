#include <string>
#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/FontManager.h"
#include "../../Application.h"
#include "Loading.h"

// コンストラクタ
Loading::Loading()
	: pauseFont_(-1)
	, loadTimer_(0)
	, isLoading_(false)
	, isLoadEnd_(false)
{}

// デストラクタ
Loading::~Loading(){}

// 読み込み
void Loading::Load(void)
{
	// リソース取得
	auto& res = ResourceManager::GetInstance();
	res.Load(ResourceManager::SRC::FONT);

	// フォントハンドルの作成
	auto& font = FontManager::GetInstance();
	pauseFont_ = font.CreateMyFont(FONT_NAME, FONT_SIZE, FONT_THICKNESS);
}

// 更新
void Loading::Update(void)
{
	// 前フレームで立てた完了フラグを下ろす
	isLoadEnd_ = false;

	// ロード中でないときは処理しない
	if (!isLoading_) return;

	// ロード時間更新
	loadTimer_++;

	// 非同期読み込みがすべて完了し、
	// かつ最低表示時間を過ぎたらロード終了
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME)
	{
		isLoading_ = false;
		isLoadEnd_ = true;
	}
}

// 描画
void Loading::Draw(void)
{
	if (!isLoading_) return;

	int dotCount = (loadTimer_ / 20) % 4;
	std::wstring text = 
		std::wstring(LOADING_TEXT) + std::wstring(dotCount, L'.');

	const int x = Application::SCREEN_SIZE_X - FONT_ADJUST_X;
	const int y = Application::SCREEN_SIZE_Y - FONT_ADJUST_Y;

	DrawStringToHandle(
		x, y,
		text.c_str(),
		FONT_COLOR,
		pauseFont_);
}

// 解放
void Loading::Release(void)
{
	// フォントハンドル削除
	pauseFont_ = -1;
}

void Loading::StartLoading(void)
{
	// ロード画面の表示開始
	isLoading_ = true;
	loadTimer_ = 0;
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

// 同期読み込みに切り替える
void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}
