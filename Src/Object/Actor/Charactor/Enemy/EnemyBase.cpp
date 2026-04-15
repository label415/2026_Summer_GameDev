#include "EnemyBase.h"

EnemyBase::EnemyBase(const EnemyBase::EnemyData& data)
	:
	CharactorBase(),
	type_(data.type),
	hp_(data.hp),
	defaultPos_(data.defaultPos),
	moveRadius_(data.moveRadius)
{
	// 初期座標の設定
	transform_.pos = defaultPos_;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Draw(void)
{
	CharactorBase::Draw();
#ifdef _DEBUG
	// 移動可能範囲のデバッグ描画
	/*DrawSphere3D(defaultPos_, moveRadius_, 16, 0x000099, 0x000099, false);*/
#endif // _DEBUG
}

void EnemyBase::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

bool EnemyBase::InMovableRange(void) const
{
	bool ret = false;
	// 初期位置からの距離
	float dis = static_cast<float>(
		AsoUtility::SqrMagnitude(defaultPos_, transform_.pos));
	// 指定距離判定
	if (dis < moveRadius_ * moveRadius_)
	{
		return true;
	}
	return ret;
}
