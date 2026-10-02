#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/FontManager.h"
#include "../../Application.h"
#include "Loading.h"

// コンストラクタ
Loading::Loading()
	: pauseFont_(-1)
	, loadTimer_(0)
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
	if (!IsLoading())return;

	// ロード時間更新
	loadTimer_++;
}

// 描画
void Loading::Draw(void)
{
	if (!IsLoading())return;

	int dotCount = (loadTimer_ / 20) % 4;
    std::wstring dots(dotCount, L'.');

	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X - FONT_ADJUST_X,
		Application::SCREEN_SIZE_Y - FONT_ADJUST_Y,
		FONT_COLOR,
		pauseFont_,
		(LOADING + dots_).c_str()
	);
}

// 解放
void Loading::Release(void)
{
	// フォントのハンドル解放
	DeleteFontToHandle(pauseFont_);
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	// ロード時間リセット
	loadTimer_ = 0;

	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

// 同期読み込みに切り替える
void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}
