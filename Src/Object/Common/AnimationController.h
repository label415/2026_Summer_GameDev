#pragma once
#include <string>
#include <map>

class AnimationController
{

public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

	// コンストラクタ
	AnimationController(int modelId);

	// デストラクタ
	~AnimationController(void);

	// 外部FBXからアニメーション追加
	void Add(int type, float speed, const std::wstring path);
	void Add(int type, float speed, int handlId);
	
	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);

	// アニメーション再生
	void Play(int type, bool isLoop = true);

	// 更新
	void Update(void);

	// 解放
	void Release(void);

	// 再生中のアニメーション
	int GetPlayType(void) const;
	int GetPlayType(int type) const;

	// 再生終了
	bool IsEnd(void) const;
	bool IsEnd(int type) const;

	// 再生中のアニメーション情報を取得
	const Animation& GetPlayAnim(void) const;
	const Animation& GetPlayAnim(int type) const;

	// 特定の時間をループするアニメーションを再生
	void SetSpecificTime(float state, float end, bool SpecificLoop);

	// アニメーションを始める時間を設定
	void SetStateTime(float state);

	// アニメーションを止めるフラグを設定
	void SetIsStopFlager(bool isStop);
private:

	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// 再生中のアニメーション
	int playType_;
	Animation playAnim_;

	//特定の開始時間
	float SpState_;

	//特定の終了時間
	float SpEnd_;

	//特定の時間をループする
	bool SpecificLoop_;

	// アニメーションをループするかしないか
	bool isLoop_;

	// アニメーション追加の共通処理
	void Add(int type, float speed, Animation& animation);

	// アニメーションの再生を開始する共通処理
	bool isReversing_;

	// アニメーションを止めるフラグ
	bool isStop_;

};
