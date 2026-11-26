#pragma once

class Fader
{
public:

	// 状態
	enum class STATE
	{
		NONE,
		FADE_OUT,	// 徐々に暗転
		FADE_IN		// 徐々に明転
	};

	// コンストラクタ
	Fader(void);

	// デストラクタ
	~Fader(void) {}

	// 状態の取得
	STATE GetState(void) const { return state_; }

	// フェード処理が終了しているか
	bool IsEnd(void) const { return isEnd_; }

	// 指定フェードを開始する
	void SetFade(STATE state);

	// 初期化
	void Init(void) {}

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

private:

	// 透明度最大値
	static constexpr float MAX_ALPHA = 255.0f;

	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 15.0f;

	// フェード描画色
	static constexpr int FADE_COLOR = 0x000000;

	// 状態
	STATE state_;

	// 透明度
	float alpha_;

	// フェード処理の開始判定
	bool isPreEnd_;

	// フェード処理の終了判定
	bool isEnd_;

};
