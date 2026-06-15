#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/FontManager.h"
#include "../../Application.h"
#include "Loading.h"

// コンストラクタ
Loading::Loading()
	: pauseFont_(-1)
	, pos_(0, 0)
	, isLoading_(false)
	, loadTimer_(0)
{
}

// デストラクタ
Loading::~Loading()
{
}

// 初期化
void Loading::Init(void)
{
	loadTimer_ = 0;
	isLoading_ = false;
	pos_ = Vector2(0.0f, 0.0f);
}

// 読み込み
void Loading::Load(void)
{
	auto& res = ResourceManager::GetInstance();
	res.Load(ResourceManager::SRC::L_FONT);

	// フォントハンドルの作成
	auto& font = FontManager::GetInstance();
	pauseFont_ = font.CreateMyFont(L"KazukiReiwa", 56, 20);
}

// 更新
void Loading::Update(void)
{
	loadTimer_++;

	// 読込中のものがなくなったら or 最低ロード時間経過
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME)
	{
		// ロード終了
		Init();
	}
	// 読み込み中
	else
	{
		// ロード画面を動作させるならここに記述
	}
}

// 描画
void Loading::Draw(void)
{
	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X - 200,
		Application::SCREEN_SIZE_Y - 70,
		0xffffff,
		pauseFont_,
		L"Loading...");
}

// 解放
void Loading::Release(void)
{
	DeleteFontToHandle(pauseFont_);
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
