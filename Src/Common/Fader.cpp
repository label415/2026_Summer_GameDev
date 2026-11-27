#include <DxLib.h>
#include "../Application.h"
#include "Fader.h"

Fader::Fader(void)
	: state_(STATE::NONE)
	, alpha_(0.0f)
	, isPreEnd_(true)
	, isEnd_(true)
{
	// 初期状態は無効・透明・終了済みで初期化
}

void Fader::SetFade(STATE state)
{
	// 新しいフェード状態を設定し、有効な遷移なら終了フラグを降ろす
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
}

void Fader::Update(void)
{
	// フェード中でなければ更新しない
	if (isEnd_)return;

	// 現在のフェード種別に応じて透過度を更新
	switch (state_)
	{
	case STATE::NONE:
		return;
	case STATE::FADE_OUT:
		// 徐々に不透明方向へ加算し、上限到達後1フレーム遅らせて終了判定
		alpha_ += SPEED_ALPHA;
		if (alpha_ > MAX_ALPHA)
		{
			// フェード終了
			alpha_ = MAX_ALPHA;
			if (isPreEnd_)
			{
				// 1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;
	case STATE::FADE_IN:
		// 徐々に透明方向へ減算し、下限到達後1フレーム遅らせて終了判定
		alpha_ -= SPEED_ALPHA;
		if (alpha_ < 0)
		{
			// フェード終了
			alpha_ = 0;
			if (isPreEnd_)
			{
				// 1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;
	default:
		return;
	}
}

void Fader::Draw(void)
{
	// フェード中でなければ描画しない
	if (state_ == STATE::NONE)return;

	// フェード画面暗転
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)alpha_);
	DrawBox(
		0, 0,
		Application::SCREEN_SIZE.x,
		Application::SCREEN_SIZE.y,
		FADE_COLOR, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
