#pragma once
#include <vector>
#include <map>
#include <Dxlib.h>
#include "../../Template/Singleton.h"
#include "../../Template/Vector2Template.h"

class InputManager : public Singleton<InputManager>
{
	// Singleton 基底クラスからコンストラクタ・デストラクタへアクセスできるようにする
	friend class Singleton<InputManager>;
public:

	// アナログキーの最大値
	static constexpr float AKEY_VAL_MAX = 1000.0f;

	// アナログキーの入力受付しきい値(0.0～1.0)
	static constexpr float THRESHOLD = 0.35f;

	// ゲームコントローラーの認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class JOYPAD_NO
	{
		KEY_PAD1,			// キー入力とパッド１入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		INPUT_KEY = 4096	// キー入力
	};

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		OTHER = 0,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,
		MAX
	};

	// ゲームコントローラーボタン
	enum class JOYPAD_BTN
	{
		LEFT = 0,
		RIGHT,
		TOP,
		DOWN,
		START,
		R_TRIGGER,
		L_TRIGGER,
		MAX
	};

	// ゲームコントローラーの入力情報
	struct JOYPAD_IN_STATE
	{
		unsigned char ButtonsOld[static_cast<int>(JOYPAD_BTN::MAX)]; // 1フレーム前のボタン入力値
		unsigned char ButtonsNew[static_cast<int>(JOYPAD_BTN::MAX)]; // 現フレームのボタン入力値
		bool IsOld[static_cast<int>(JOYPAD_BTN::MAX)];               // 1フレーム前の押下状態
		bool IsNew[static_cast<int>(JOYPAD_BTN::MAX)];               // 現フレームの押下状態
		bool IsTrgDown[static_cast<int>(JOYPAD_BTN::MAX)];           // 現フレームで押下されたか
		bool IsTrgUp[static_cast<int>(JOYPAD_BTN::MAX)];             // 現フレームで離されたか
		int AKeyLX; // 左スティックX入力値
		int AKeyLY; // 左スティックY入力値
		int AKeyRX; // 右スティックX入力値
		int AKeyRY; // 右スティックY入力値
	};

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 解放（配列の解放）
	void Release(void);

	// 判定を行うキーを追加
	void Add(int key);

	// 判定を行うキーをクリア
	void Clear(void);

	// キーの押下判定
	bool IsNew(int key) const { return Find(key).keyNew; }

	// キーの押下判定(押しっぱなしはNG)
	bool IsTrgDown(int key) const { return Find(key).keyTrgDown; }

	// キーを離した時の判定
	bool IsTrgUp(int key) const { return Find(key).keyTrgUp; }

	// マウス座標の取得
	Vector2 GetMousePos(void) const { return mousePos_; }

	// マウスのクリック状態を取得(MOUSE_INPUT_LEFT、RIGHT)
	int GetMouse(void) const { return mouseInput_; }

	// マウスが左クリックされたか
	bool IsClickMouseLeft(void) const { return mouseInput_ == MOUSE_INPUT_LEFT; }

	// マウスが右クリックされたか
	bool IsClickMouseRight(void) const { return mouseInput_ == MOUSE_INPUT_RIGHT; }

	// マウスが左クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseLeft(void) const { return FindMouse(MOUSE_INPUT_LEFT).keyTrgDown; }

	// マウスが右クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseRight(void) const { return FindMouse(MOUSE_INPUT_RIGHT).keyTrgDown; }

	// マウスが中央クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseMiddle(void) const { return FindMouse(MOUSE_INPUT_MIDDLE).keyTrgDown; }

	// マウス回転量取得
	float GetMouseWheelRot(void) const { return mouseWheelRotVolF_; }

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);

	// ボタンが押されているか判定
	bool IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const { return padInfos_[static_cast<int>(no)].IsNew[static_cast<int>(btn)]; }

	// ボタンが押された瞬間か判定
	bool IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const { return padInfos_[static_cast<int>(no)].IsTrgDown[static_cast<int>(btn)]; }

	// ボタンが離された瞬間か判定
	bool IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const { return padInfos_[static_cast<int>(no)].IsTrgUp[static_cast<int>(btn)]; }

	// アナログキーの入力値から方向(正規化済み)を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY) const;

	// マウスカーソルの移動量を取得する
	Vector2 GetMousePosDistance(void) const { return mouseMoveDist_; }

	// マウスカーソルの表示設定
	void SetMouseFlag(bool isFlag);

	// マウスカーソルの表示フラグ取得
	bool GetMouseFlag(void) const { return isMouseFlag_; }

private:

	// キー情報
	struct Info
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// マウス
	struct MouseInfo
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;

	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;

	// キー情報
	std::map<int, InputManager::Info> keyInfos_;
	InputManager::Info infoEmpty_;

	// マウス情報
	std::map<int, InputManager::MouseInfo> mouseInfos_;
	InputManager::MouseInfo mouseInfoEmpty_;

	// マウスカーソルの位置
	Vector2 mousePos_;
	// マウスカーソルの移動量
	Vector2 mouseMoveDist_;
	// マウスホイールの回転量
	float mouseWheelRotVolF_;
	// マウスボタンの入力状態
	int mouseInput_;
	// マウスの有効フラグ
	bool isMouseFlag_;

	// パッド情報の最大管理数
	static constexpr int PAD_INFO_MAX = 5;

	// DirectInput物理ボタン番号(XBOX ONE)
	static constexpr int DINPUT_XBOX_BTN_A = 0;
	static constexpr int DINPUT_XBOX_BTN_B = 1;
	static constexpr int DINPUT_XBOX_BTN_X = 2;
	static constexpr int DINPUT_XBOX_BTN_Y = 3;
	static constexpr int DINPUT_XBOX_BTN_START = 6;

	// DirectInput物理ボタン番号(DualShock4 / DualSense)
	static constexpr int DINPUT_PS_BTN_SQUARE = 0;
	static constexpr int DINPUT_PS_BTN_CROSS = 1;
	static constexpr int DINPUT_PS_BTN_CIRCLE = 2;
	static constexpr int DINPUT_PS_BTN_TRIANGLE = 3;
	static constexpr int DINPUT_PS_BTN_L2 = 6;
	static constexpr int DINPUT_PS_BTN_R2 = 7;

	// パッド情報
	JOYPAD_IN_STATE padInfos_[PAD_INFO_MAX];

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	InputManager(void);

	// コピーコンストラクタを禁止
	InputManager(const InputManager& instance) = delete;

	// デストラクタもprivateにして外部破棄を防止
	~InputManager(void) override = default;

	// 配列の中からキー情報を取得する
	const InputManager::Info& Find(int key) const;

	// 配列の中からマウス情報を取得する
	const InputManager::MouseInfo& FindMouse(int key) const;

	// 接続されたコントローラの種別を取得する
	JOYPAD_TYPE GetJPadType(JOYPAD_NO no) const { return static_cast<InputManager::JOYPAD_TYPE>(GetJoypadType(static_cast<int>(no))); }

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no);

	// コントローラの入力情報を更新する
	void SetJPadInState(JOYPAD_NO jpNo);

};