#pragma once
#include <map>
#include <memory>
#include <vector>
#include "../../Template/Vector2Template.h"

class UIBase;

class UiManager
{
public:

	// コンストラクタ
	UiManager(void) = default;

	// デストラクタ
	~UiManager(void) = default;

	// UIを追加登録する
	void Add(std::unique_ptr<UIBase> ui);

	// 識別ID付きでUIを追加登録する
	void Add(int id, std::unique_ptr<UIBase> ui);

	// 登録中の全UIを更新する
	void Update(void);

	// 登録中の全UIを描画する
	void Draw(void);

	// 登録中の全UIを解放・破棄する
	void Release(void);

	// 指定IDのUIの有効・無効を設定する
	void SetActive(int id, bool active);

	// 指定IDのUIの座標を設定する
	void SetPos(int id, Vector2F pos);

	// 指定IDのUIの画像ハンドルを設定する
	void SetImg(int id, int img);

	// 指定IDの画像UIの透過度を設定する
	void SetAlpha(int id, int alpha);

private:

	// 指定IDのUIを取得する
	UIBase* FindUI(int id) const;

	// 管理中の全UI
	std::vector<std::unique_ptr<UIBase>> uis_;

	// 識別IDからUIを引くための参照マップ
	std::map<int, UIBase*> idMap_;

};
