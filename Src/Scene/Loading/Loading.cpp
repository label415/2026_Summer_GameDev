#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/FontManager.h"
#include "../../Application.h"
#include "Loading.h"

// コンストラクタ
Loading::Loading()
	: pauseFont_(-1)
	, loadTimer_(0)
{
}

// デストラクタ
Loading::~Loading(){}

// 読み込み
void Loading::Load(void)
{
	// フォントハンドルの作成
	auto& font = FontManager::GetInstance();
	pauseFont_ = font.CreateMyFont(L"KazukiReiwa", 56, 20);
}

// 更新
void Loading::Update(void)
{
	if (!IsEnd()) {
		loadTimer_ ++;
	}
}

// 描画
void Loading::Draw(void)
{
	// ロード中のドットの数を計算
	int dotCount = (loadTimer_ / 20) % 4;
	std::wstring dots(dotCount, L'.');

	// ロード中の文字列を描画
	DrawFormatStringToHandle(
		Application::SCREEN_SIZE_X - 200,
		Application::SCREEN_SIZE_Y - 70,
		0xffffff,
		pauseFont_,
		(L"Loading" + dots).c_str()
	);
}

// 解放
void Loading::Release(void)
{
	// フォントハンドルの解放
	DeleteFontToHandle(pauseFont_);
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	// ロードタイマーをリセット
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
