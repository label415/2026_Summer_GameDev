#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/FontManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void) : SceneBase() {}
TitleScene::~TitleScene(void) {}

void TitleScene::Load(void)
{
    bgm_ = resMng_.Load(ResourceManager::SRC::TITLE_BGM).handleId_;
    SoundManager::GetInstance().PlayBGM(bgm_, 50);

    resMng_.Load(ResourceManager::SRC::FONT);
    pauseFont_ = fontMng_.GetInstance().CreateMyFont(L"KazukiReiwa", FONT_SIZE, FONT_THICKNESS);
    titleImg_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;
    selectImg_ = resMng_.Load(ResourceManager::SRC::TITLE_SELECT).handleId_;

    selectIndex_ = 0;
    isHovered = true;

    // UIコライダー生成
    for (int i = 0; i < LIST_MAX; ++i)
    {
        float posY = (Application::SCREEN_SIZE_Y / 1.65f + 70.0f * i);
        uiBoxs_[i] = std::make_unique<ColliderBox2D>(
            ColliderBase2D::TAG::UI,
            Vector2F(Application::SCREEN_SIZE_X / 2 - 150.0f, posY),
            300.0f, 60.0f);
    }
}

void TitleScene::LoadEnd(void) {}

void TitleScene::Update(void)
{
    auto& ins = InputManager::GetInstance();

    // マウス判定
    Vector2 mPos = ins.GetMousePos();
    for (int i = 0; i < LIST_MAX; ++i)
    {
        if (uiBoxs_[i] && uiBoxs_[i]->Contains(mPos.x,mPos.y))
        {
            ins.SetMouseFlage(true);
            selectIndex_ = i;
            isHovered = true;
            break;
        }
    }

    // 2. パッド入力（上下でインデックス増減）
    auto pad = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    float stickZ = ins.GetDirectionXZAKey(pad.AKeyLX, pad.AKeyLY).z;
    int dirY = (stickZ > 0.5f) ? -1 : (stickZ < -0.5f) ? 1 : 0;

    if (dirY != 0)
    {
        ins.SetMouseFlage(false);
        if (!isStickInput_)
        {
            selectIndex_ = (selectIndex_ + dirY + LIST_MAX) % LIST_MAX;
            isStickInput_ = isHovered = true;
        }
    }
    else
    {
        isStickInput_ = false;
    }

    // 3. ボックスの選択状態更新
    for (int i = 0; i < LIST_MAX; ++i)
    {
        if (uiBoxs_[i]) uiBoxs_[i]->SetValid(isHovered && (selectIndex_ == i));
    }

    // 4. シーン遷移
    bool isDecide = ins.IsTrgMouseLeft() || ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
    if (isDecide && isHovered)
    {
        if (selectIndex_ == static_cast<int>(LIST::始める))   sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        if (selectIndex_ == static_cast<int>(LIST::ゲーム終了)) Application::GetInstance().SetIsEnd(true);
    }
}

void TitleScene::Draw(void)
{
    // タイトル画像
    DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 0.8f, 0.0f, titleImg_, true);

    // カーソル画像
    if (isHovered && uiBoxs_[selectIndex_])
    {
        float centerY = uiBoxs_[selectIndex_]->Top() + (uiBoxs_[selectIndex_]->Bottom() - uiBoxs_[selectIndex_]->Top()) * 0.5f;
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2, static_cast<int>(centerY), 0.2f, 0.0f, selectImg_, true);
    }

    // 文字とコライダー描画（1つのループに統合）
    for (int i = 0; i < LIST_MAX; ++i)
    {
        int strW = GetDrawStringWidthToHandle(pasueList_[i].c_str(), -1, pauseFont_);
        int posX = (Application::SCREEN_SIZE_X - strW) / 2;
        int posY = static_cast<int>(Application::SCREEN_SIZE_Y / 1.6f) + (70 * i);

        DrawFormatStringToHandle(posX, posY, 0xffffff, pauseFont_, pasueList_[i].c_str());

        if (uiBoxs_[i]) uiBoxs_[i]->Draw();
    }
}

void TitleScene::Release(void)
{
    DeleteFontToHandle(pauseFont_);
}