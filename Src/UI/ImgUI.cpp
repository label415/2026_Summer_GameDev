#include <algorithm>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "ImgUI.h"

ImgUI::ImgUI(int img, Vector2F pos, Vector2F size, float angle, int alpha)
	: UIBase(img, pos, size)
	, angle_(angle)
	, alpha_(std::clamp(alpha, 0, MAX_ALPHA))
{
	// 画像ハンドル・座標・大きさ・回転角・透過度を初期化
}

void ImgUI::Draw(void)
{
	// 非表示・画像未読み込み・完全透明時は描画しない
	if (!active_ || img_ < 0 || alpha_ <= 0)
	{
		return;
	}

	// 画像の元サイズを取得して中心座標を算出
	Vector2F imgSize;
	GetGraphSizeF(img_, &imgSize.x, &imgSize.y);

	// 透過指定がある場合はアルファブレンドを有効化する
	bool useAlpha = (alpha_ < MAX_ALPHA);
	if (useAlpha)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);
	}

	// 指定座標・スケール・回転角で中心基準描画を行う
	DrawRotaGraph3F(
		pos_.x,
		pos_.y,
		imgSize.x * AsoUtility::HALF,
		imgSize.y * AsoUtility::HALF,
		static_cast<double>(size_.x),
		static_cast<double>(size_.y),
		static_cast<double>(angle_),
		img_,
		true);

	// ブレンド設定を元に戻す
	if (useAlpha)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void ImgUI::SetAlpha(int alpha)
{
	// 透過度を 0～255 の範囲に制限して設定する
	alpha_ = std::clamp(alpha, 0, MAX_ALPHA);
}
