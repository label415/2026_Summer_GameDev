#include <DxLib.h>
#include "../Application.h"
#include "Fader.h"

Fader::Fader(void)
	:
	state_(STATE::NONE),
	alpha_(0.0f),
	isPreEnd_(true),
	isEnd_(true)
{}

Fader::~Fader(void){}

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
}

void Fader::Init(void){}

void Fader::Update(void)
{
	// フェード中でなければ更新しない
	if (isEnd_)return;

	switch (state_)
	{
	case STATE::NONE:
		return;
	case STATE::FADE_OUT:
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
		Application::SCREEN_SIZE_X,
		Application::SCREEN_SIZE_Y,
		0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
