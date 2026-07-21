#pragma once
class ResourceManager;
class SceneManager;
class FontManager;	
class SoundManager;

class SceneBase
{

public:

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void) = 0;

	//読み込み
	virtual void Load(void) = 0;
	//読み込み後の初期化
	virtual void LoadEnd(void) = 0;
	// 更新
	virtual void Update(void) = 0;
	// 描画
	virtual void Draw(void) = 0;
	// 解放
	virtual void Release(void) = 0;

protected:

	// リソース管理
	ResourceManager& resMng_;

	// シーン管理
	SceneManager& sceMng_;

	// フォント管理
	FontManager& fontMng_;

	// サウンド管理
	SoundManager& sonMng_;

	// BGMのハンドル
	int bgm_;
	// BGMの音量
	int volume_;

};
