#include <algorithm>
#include <cmath>
#include <DxLib.h>
#include "../Manager/Common/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "BarUI.h"

BarUI::BarUI(
	int barImg,
	int frameImg,
	Vector2F pos,
	Vector2F barScl,
	float frameScl,
	float initRate)
	: UIBase(barImg, pos, barScl)
	, frameImg_(frameImg)
	, frameScl_(frameScl)
	, targetRate_(std::clamp(initRate, 0.0f, 1.0f))
	, displayRate_(std::clamp(initRate, 0.0f, 1.0f))
	, rateGetter_(nullptr)
{
	// 枠・バー画像と初期表示割合を初期化（ゲーム数値本体は保持しない）
}

void BarUI::Update(void)
{
	// モデルがバインドされていれば最新の割合を取得して目標割合を更新
	if (rateGetter_)
	{
		SetRate(rateGetter_());
	}

	// 表示割合を目標割合へ滑らかに補間させる
	float diff = targetRate_ - displayRate_;
	if (std::fabsf(diff) <= SNAP_EPSILON)
	{
		displayRate_ = targetRate_;
		return;
	}

	float t = (std::min)(1.0f, INTERP_SPEED * SceneManager::GetInstance().GetDeltaTime());
	displayRate_ = AsoUtility::Lerp(displayRate_, targetRate_, t);
}

void BarUI::Draw(void)
{
	// 非表示時は描画しない
	if (!active_)
	{
		return;
	}

	// 背景枠の描画
	if (frameImg_ >= 0)
	{
		DrawRotaGraphF(
			pos_.x,
			pos_.y,
			frameScl_,
			0.0f,
			frameImg_,
			true);
	}

	// バー画像未読み込み、またはスケールが不正な場合はバー描画をスキップ
	if (img_ < 0 || size_.x <= 0.0f || size_.y <= 0.0f)
	{
		return;
	}

	// バー画像の元サイズを取得
	Vector2 srcSize;
	GetGraphSize(img_, &srcSize.x, &srcSize.y);
	Vector2F imgSize = srcSize;

	float realHalfWidth = imgSize.x / size_.x;
	float realHalfHeight = imgSize.y / size_.y;

	float left = pos_.x - realHalfWidth;
	float top = pos_.y - realHalfHeight;
	float bottom = pos_.y + realHalfHeight;

	int srcWidth = static_cast<int>(imgSize.x * displayRate_);
	float currentBarRight = left + (realHalfWidth * AsoUtility::DOUBLE * displayRate_);

	// 表示割合に応じたバー本体の伸縮描画
	DrawRectExtendGraphF(
		left,
		top,
		currentBarRight,
		bottom,
		0,
		0,
		srcWidth,
		srcSize.y,
		img_,
		true);
}

void BarUI::BindRate(std::function<float(void)> rateGetter)
{
	// モデル側の割合取得関数を登録し、初回表示割合を即時同期する
	rateGetter_ = std::move(rateGetter);
	if (rateGetter_)
	{
		SetRate(rateGetter_());
		displayRate_ = targetRate_;
	}
}

void BarUI::SetRate(float rate)
{
	// 目標割合を 0.0f?1.0f の範囲に収めて設定する
	targetRate_ = std::clamp(rate, 0.0f, 1.0f);
}
