#pragma once
#include <Windows.h>

class ImGuiWrapper
{

public:

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static ImGuiWrapper& GetInstance(void) { return *instance_; }

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放・インスタンス破棄
	void Destroy(void);

private:

	// WM_SYSCOMMAND下位4ビットマスク
	static constexpr WPARAM SYSCOMMAND_MASK = 0xfff0;

	// 静的インスタンス
	static ImGuiWrapper* instance_;

	// ウインドウプロージャ
    static LRESULT CALLBACK WndProc(
        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ImGuiWrapper(void) {}

	// コピーコンストラクタも同様
	ImGuiWrapper(const ImGuiWrapper&);

	// デストラクタも同様
	~ImGuiWrapper(void) {}

	// マウス入力の更新
	void UpdateInputMouse(void);

	// 新規フレームの開始準備
	void UpdateNewFrame(void);

};

