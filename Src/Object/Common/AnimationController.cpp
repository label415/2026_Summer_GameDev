#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "AnimationController.h"

AnimationController::AnimationController(int modelId)
	:
	modelId_(modelId),
	playType_(-1),
	playAnim_(),
	isLoop_(true)
{
	isStop_ = false;
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(int type, float speed, const std::wstring path)
{
	Animation animation;
	animation.model = MV1LoadModel(path.c_str());
	animation.animIndex = -1;

	Add(type, speed, animation);
}

void AnimationController::Add(int type, float speed, int handlId)
{
	Animation animation;
	animation.model = handlId;
	animation.animIndex = -1;

	Add(type, speed, animation);
}

void AnimationController::AddInFbx(int type, float speed, int animIndex)
{
	Animation animation;
	animation.model = -1;
	animation.animIndex = animIndex;

	Add(type, speed, animation);
}

void AnimationController::Play(int type, bool isLoop)
{

	if (playType_ == type)
	{
		// 同じアニメーションだったら再生を継続する
		return;
	}

	if (playType_ != -1)
	{
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId_, playAnim_.attachNo);
	}

	// アニメーション種別を変更
	playType_ = type;
	playAnim_ = animations_[type];

	// 初期化
	playAnim_.step = 0.0f;

	// モデルにアニメーションを付ける
	if (playAnim_.model == -1)
	{
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex);
	}
	else
	{
		// 別のモデルファイルからアニメーションをアタッチする
		// DxModelViewerを確認すること(大体0か1)
		int animIdx = 0;
		playAnim_.attachNo = MV1AttachAnim(modelId_, animIdx, playAnim_.model);
	}

	// アニメーション総時間の取得
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);

	// アニメーションループ
	isLoop_ = isLoop;

}

void AnimationController::Update(void)
{
	if (isStop_)return;
	// 経過時間の取得
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	if (SpecificLoop_)
	{
		// 1. 反転の判定（境界チェック）
		if (playAnim_.step >= SpEnd_)
		{
			playAnim_.step = SpEnd_;
			isReversing_ = true;  // 端に達したので「逆再生」にする
		}
		else if (playAnim_.step <= SpState_)
		{
			playAnim_.step = SpState_;
			isReversing_ = false; // 開始点に戻ったので「通常再生」にする
		}

		// 2. 現在の方向に基づいてステップを進める/戻す
		if (isReversing_)
		{
			playAnim_.step -= (deltaTime * playAnim_.speed);
		}
		else
		{
			playAnim_.step += (deltaTime * playAnim_.speed);
		}
	}
	else {

		// 再生
		playAnim_.step += (deltaTime * playAnim_.speed);

		// アニメーションが終了したら
		if (playAnim_.step > playAnim_.totalTime)
		{
			if (isLoop_)
			{
				// ループ再生
				playAnim_.step = 0.0f;
			}
			else
			{
				// ループしない
				playAnim_.step = playAnim_.totalTime;
			}
		}
	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);

}

void AnimationController::Release(void)
{

	// 外部FBXのモデル(アニメーション)解放
	for (const std::pair<int, Animation>& pair : animations_)
	{
		if (pair.second.model != -1)
		{
			MV1DeleteModel(pair.second.model);
		}
	}
	
	// 可変長配列をクリアする
	animations_.clear();
	
}

int AnimationController::GetPlayType(void) const
{
	return playType_;
}

int AnimationController::GetPlayType(int type) const
{
	auto it = animations_.find(type);
	if (it == animations_.end()) {
		return -1;
	}

	return playType_;
}

bool AnimationController::IsEnd(void) const
{
	bool ret = false;

	if (isLoop_)
	{
		// ループ設定されているなら、
		// 無条件で終了しないを返す
		return ret;
	}

	if (playAnim_.step >= playAnim_.totalTime)
	{
		// 再生時間を過ぎたらtrue
		return true;
	}

	return ret;

}

bool AnimationController::IsEnd(int type) const
{
	auto it = animations_.find(type);
	if (it == animations_.end()) {
		return false;
	}

	bool ret = false;

	if (isLoop_)
	{
		// ループ設定されているなら、
		// 無条件で終了しないを返す
		return ret;
	}

	if (playAnim_.step >= playAnim_.totalTime)
	{
		// 再生時間を過ぎたらtrue
		return true;
	}

	return ret;

}

const AnimationController::Animation& AnimationController::GetPlayAnim(void) const
{
	return playAnim_;
}

const AnimationController::Animation& AnimationController::GetPlayAnim(int type) const
{
	auto it = animations_.find(type);
	if (it == animations_.end()) {
		return Animation();
	}
	return playAnim_;
}

void AnimationController::SetSpecificTime(float state, float end, bool SpecificLoop)
{
	SpState_ = state;
	SpEnd_ = end;
	SpecificLoop_ = SpecificLoop;
}

void AnimationController::SetStateTime(float state)
{
	playAnim_.step = state;
}

void AnimationController::SetIsStopFlager(bool isStop)
{
	isStop_ = isStop;
}

void AnimationController::Add(int type, float speed, Animation& animation)
{
	animation.speed = speed;

	if (animations_.count(type) == 0)
	{
		// 追加
		animations_.emplace(type, animation);
	}
}
