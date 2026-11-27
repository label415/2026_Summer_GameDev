#include <string>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "InputManager.h"

void InputManager::Init(void)
{
	// DirectInput による入力取得を有効化
	SetUseDirectInputFlag(true);

	// ゲームで使用したいキーを、
	// 事前にここで登録しておいてください
	Add(KEY_INPUT_SPACE);
	Add(KEY_INPUT_N);
	Add(KEY_INPUT_Z);

	Add(KEY_INPUT_LEFT);
	Add(KEY_INPUT_RIGHT);
	Add(KEY_INPUT_UP);
	Add(KEY_INPUT_DOWN);

	Add(KEY_INPUT_W);
	Add(KEY_INPUT_A);
	Add(KEY_INPUT_S);
	Add(KEY_INPUT_D);
	Add(KEY_INPUT_L);
	Add(KEY_INPUT_R);
	Add(KEY_INPUT_1);

	Add(KEY_INPUT_RSHIFT);
	Add(KEY_INPUT_LSHIFT);

	Add(KEY_INPUT_BACKSLASH);
	Add(KEY_INPUT_ESCAPE);

	// マウスボタン情報の初期登録用構造体
	InputManager::MouseInfo info;

	// 左クリック情報を生成して登録
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_LEFT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// 右クリック情報を生成して登録
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_RIGHT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// 中央クリック情報を生成して登録
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_MIDDLE;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// マウスカーソルを画面中央に配置し、移動量と表示設定を初期化
	constexpr Vector2 center = Application::HALF_SCREEN_SIZE;
	SetMousePoint(center.x, center.y);
	mousePos_ = center;
	mouseMoveDist_ = AsoUtility::VECTOR2_ZERO;
	SetMouseDispFlag(isMouseFlag_);
}

void InputManager::Update(void)
{
	// 登録済みキーボードキーの押下状態・トリガ状態を更新
	for (auto& p : keyInfos_)
	{
		// 前フレームの状態を退避
		p.second.keyOld = p.second.keyNew;
		// 現フレームの押下状態を取得
		p.second.keyNew = CheckHitKey(p.second.key);
		// 押した瞬間・離した瞬間の判定を更新
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// マウスのボタン入力とホイール回転量を取得
	mouseInput_ = GetMouseInput();
	mouseWheelRotVolF_ = GetMouseWheelRotVolF();

	// マウス表示中は現在座標のみ取得し、非表示時は中央からの差分を算出して中央へ戻す
	if (isMouseFlag_)
	{
		GetMousePoint(&mousePos_.x, &mousePos_.y);
	}
	else
	{
		constexpr Vector2 center = Application::HALF_SCREEN_SIZE;
		Vector2 currentPos = AsoUtility::VECTOR2_ZERO;
		GetMousePoint(&currentPos.x, &currentPos.y);
		mouseMoveDist_ = { currentPos.x - center.x, currentPos.y - center.y };
		SetMousePoint(center.x, center.y);
		mousePos_ = center;
	}

	// 登録済みマウスボタンの押下状態・トリガ状態を更新
	for (auto& p : mouseInfos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = mouseInput_ == p.second.key;
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// 各接続スロットのゲームパッド入力情報を更新
	SetJPadInState(JOYPAD_NO::KEY_PAD1);
	SetJPadInState(JOYPAD_NO::PAD1);
	SetJPadInState(JOYPAD_NO::PAD2);
	SetJPadInState(JOYPAD_NO::PAD3);
	SetJPadInState(JOYPAD_NO::PAD4);
}

void InputManager::Release(void)
{
	// キー管理マップの配列を解放
	keyInfos_.clear();

	// マウス管理マップの配列を解放
	mouseInfos_.clear();

	// パッド情報配列を初期状態へリセット
	for (auto& pad : padInfos_)
	{
		pad = JOYPAD_IN_STATE();
	}
}

void InputManager::Add(int key)
{
	// 指定キーの入力状態構造体を初期化してマップへ登録
	InputManager::Info info = InputManager::Info();
	info.key = key;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	keyInfos_.emplace(key, info);
}

void InputManager::Clear(void)
{
	// 登録済みキー情報マップを全消去
	keyInfos_.clear();
}

InputManager::InputManager(void)
	: keyInfos_()
	, mouseInfos_()
	, infoEmpty_()
	, mouseInfoEmpty_()
	, mousePos_()
	, mouseMoveDist_()
	, mouseWheelRotVolF_(0.0f)
	, mouseInput_(-1)
	, padInfos_()
	, joyDInState_()
	, joyXInState_()
	, isMouseFlag_(true)
{
	// 各メンバ変数を初期化リストで既定値に設定
}

const InputManager::Info& InputManager::Find(int key) const
{
	// 指定キーをマップから検索し、存在すればその情報を返す
	auto it = keyInfos_.find(key);
	if (it != keyInfos_.end())
	{
		return it->second;
	}

	// 未登録キーの場合は空のデフォルト情報を返す
	return infoEmpty_;
}

const InputManager::MouseInfo& InputManager::FindMouse(int key) const
{
	// 指定マウスボタンをマップから検索し、存在すればその情報を返す
	auto it = mouseInfos_.find(key);
	if (it != mouseInfos_.end())
	{
		return it->second;
	}

	// 未登録ボタンの場合は空のデフォルト情報を返す
	return mouseInfoEmpty_;
}

DINPUT_JOYSTATE InputManager::GetJPadDInputState(JOYPAD_NO no)
{
	// DirectInput 経由でコントローラ状態を取得
	GetJoypadDirectInputState(static_cast<int>(no), &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE InputManager::GetJPadXInputState(JOYPAD_NO no)
{
	// XInput 経由でコントローラ状態を取得
	GetJoypadXInputState(static_cast<int>(no), &joyXInState_);
	return joyXInState_;
}

void InputManager::SetJPadInState(JOYPAD_NO jpNo)
{
	// 対象パッドスロット番号と最新・現在の入力状態を取得
	int no = static_cast<int>(jpNo);
	auto stateNew = GetJPadInputState(jpNo);
	auto& stateNow = padInfos_[no];

	// 全ボタン分の前回値・現在値・トリガ判定を更新
	int max = static_cast<int>(JOYPAD_BTN::MAX);
	for (int i = 0; i < max; i++)
	{
		stateNow.ButtonsOld[i] = stateNow.ButtonsNew[i];
		stateNow.ButtonsNew[i] = stateNew.ButtonsNew[i];

		stateNow.IsOld[i] = stateNow.IsNew[i];
		//stateNow.IsNew[i] = stateNow.ButtonsNew[i] == 128 || stateNow.ButtonsNew[i] == 255;
		stateNow.IsNew[i] = stateNow.ButtonsNew[i] > 0;

		stateNow.IsTrgDown[i] = stateNow.IsNew[i] && !stateNow.IsOld[i];
		stateNow.IsTrgUp[i] = !stateNow.IsNew[i] && stateNow.IsOld[i];
	}

	// 左右アナログスティックの入力値を反映
	stateNow.AKeyLX = stateNew.AKeyLX;
	stateNow.AKeyLY = stateNew.AKeyLY;
	stateNow.AKeyRX = stateNew.AKeyRX;
	stateNow.AKeyRY = stateNew.AKeyRY;
}

void InputManager::SetMouseFlag(bool isFlag)
{
	// 既に同じ表示設定であれば何もしない
	if (isMouseFlag_ == isFlag)return;

	// 表示フラグと移動量を更新
	isMouseFlag_ = isFlag;
	mouseMoveDist_ = AsoUtility::VECTOR2_ZERO;

	// マウスカーソル非表示時は画面中央へ基準座標をリセット
	if (!isMouseFlag_)
	{
		constexpr Vector2 center = Application::HALF_SCREEN_SIZE;
		SetMousePoint(center.x, center.y);
		mousePos_ = center;
	}

	// DxLib 側へカーソル表示設定を反映
	SetMouseDispFlag(isMouseFlag_);
}

InputManager::JOYPAD_IN_STATE InputManager::GetJPadInputState(JOYPAD_NO no)
{
	// 返却用の入力情報構造体を初期化
	JOYPAD_IN_STATE ret = JOYPAD_IN_STATE();

	// 接続されているコントローラ種別を判定してマッピングを分岐
	auto type = GetJPadType(no);
	
	switch (type)
	{
	case InputManager::JOYPAD_TYPE::OTHER:
		break;
	case InputManager::JOYPAD_TYPE::XBOX_360:
	{
	}
		break;
	case InputManager::JOYPAD_TYPE::XBOX_ONE:
	{
		// Xbox One コントローラの DirectInput / XInput 状態を取得
		auto d = GetJPadDInputState(no);
		auto x = GetJPadXInputState(no);

		int idx;

		//   Y
		// X   B
		//   A

		// 各ボタン入力を共通ボタンIDへ格納
		idx = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_XBOX_BTN_Y];// Y

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_XBOX_BTN_X];// X

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_XBOX_BTN_B];// B

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_XBOX_BTN_A];// A

		idx = static_cast<int>(JOYPAD_BTN::START);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_XBOX_BTN_START];// START

		idx = static_cast<int>(JOYPAD_BTN::R_TRIGGER);
		ret.ButtonsNew[idx] = x.RightTrigger;// R_TRIGGER

		idx = static_cast<int>(JOYPAD_BTN::L_TRIGGER);
		ret.ButtonsNew[idx] = x.LeftTrigger; // L_TRIGGER

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		
		// 右スティック
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;

	}
		break;
	case InputManager::JOYPAD_TYPE::DUAL_SHOCK_4:
	case InputManager::JOYPAD_TYPE::DUAL_SENSE:
	{
		// PlayStation 系コントローラの DirectInput 状態を取得
		auto d = GetJPadDInputState(no);
		int idx;

		//   △
		// □  〇
		//   ×

		// 各ボタン入力を共通ボタンIDへ格納
		idx = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_PS_BTN_TRIANGLE];// △

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_PS_BTN_SQUARE];// □

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_PS_BTN_CIRCLE];// 〇

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_PS_BTN_CROSS];// ×

		idx = static_cast<int>(JOYPAD_BTN::R_TRIGGER);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_PS_BTN_R2];// R_TRIGGER

		idx = static_cast<int>(JOYPAD_BTN::L_TRIGGER);
		ret.ButtonsNew[idx] = d.Buttons[DINPUT_PS_BTN_L2]; // L_TRIGGER

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		
		// 右スティック
		ret.AKeyRX = d.Z;
		ret.AKeyRY = d.Rz;

	}
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_L:
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_R:
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_PRO_CTRL:
		break;
	case InputManager::JOYPAD_TYPE::MAX:
		break;
	}

	return ret;
}

VECTOR InputManager::GetDirectionXZAKey(int aKeyX, int aKeyY) const
{
	VECTOR ret = { 0.0f, 0.0f, 0.0f };

	// スティックの個々の入力値は、
	// -1000.0f ～ 1000.0f の範囲で返ってくるが、
	// X:1000.0f、Y:1000.0fになることは無い(1000と500くらいが最大)
	
	// スティックの入力値を -1.0 ～ 1.0 に正規化
	float dirX = static_cast<float>(aKeyX) / AKEY_VAL_MAX;
	float dirZ = static_cast<float>(aKeyY) / AKEY_VAL_MAX;

	// ピタゴラスの定理でニュートラル状態からの長さベクトルにする
	// ( 円形のデッドゾーンになる )

	// 平方根により、おおよその最大値が1.0となる
	float len = sqrtf(dirX * dirX + dirZ * dirZ);
	if (len < THRESHOLD)
	{
		// (0.0f, 0.0f, 0.0f)
		return ret;
	}

	// デッドゾーン境界からに再スケーリング(可変デッドゾーン)
	// ( しきい値 0.35 の場合は、 0.0 ～ 0.65 / 0.65 になる )
	float scale = (len - THRESHOLD) / (1.0f - THRESHOLD);
	dirX = (dirX / len) * scale;
	dirZ = (dirZ / len) * scale;

	// Zは前に倒すとマイナス値が返ってくるので反転
	ret = VNorm({ dirX, 0.0f, -dirZ });

	return ret;
}
