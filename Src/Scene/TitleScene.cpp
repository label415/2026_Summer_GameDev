#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/SkyDome.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	auto& resMng_ = ResourceManager::GetInstance();

	// ‰æ‘œ“Ç‚Ýž‚Ý
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE).handleId_;
	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::PUSHSPACE).handleId_;


	// ’è“_ƒJƒƒ‰
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	skydome_ = new SkyDome();
	skydome_->Init();

	// ƒƒCƒ“˜f¯
	bigPlanet_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PIT_FALL_PLANET));
	bigPlanet_.scl = AsoUtility::VECTOR_ONE;
	bigPlanet_.quaRot = Quaternion::Identity();
	bigPlanet_.quaRotLocal = Quaternion::Identity();
	bigPlanet_.pos = AsoUtility::VECTOR_ZERO;
	bigPlanet_.Update();

	// ‹…‘Ì˜f¯
	spherPlanet_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::SPHERE_PLANET));
	spherPlanet_.scl = { 0.7f, 0.7f, 0.7f };
	spherPlanet_.quaRot = Quaternion::Identity();
	spherPlanet_.quaRotLocal = Quaternion::Identity();
	spherPlanet_.quaRotLocal =
		Quaternion::Mult(spherPlanet_.quaRotLocal,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(90.0f), AsoUtility::AXIS_X));
	spherPlanet_.pos = { -250.0f, -100.0f, -100.0f };
	spherPlanet_.Update();

	//ƒvƒŒƒCƒ„[
	player_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	player_.scl = { 0.4f, 0.4f, 0.4f };
	player_.quaRot = Quaternion::Identity();
	player_.quaRot =
		Quaternion::Mult(player_.quaRot,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(-90.0f), AsoUtility::AXIS_Y));
	player_.quaRotLocal = Quaternion::Identity();
	player_.quaRotLocal =
		Quaternion::Mult(player_.quaRotLocal,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));
	player_.pos = { -250.0f, -32.0f, -105.0f };
	player_.Update();

	anim_ = new AnimationController(player_.modelId);
	anim_->Add(static_cast<int>(ANIM_TYPE::RUN),
		30.0f, Application::PATH_MODEL + "Player/Run.mv1");

}

void TitleScene::Update(void)
{

	// ƒV[ƒ“‘JˆÚ
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	skydome_->Update();

	spherPlanet_.quaRot = Quaternion::Mult(spherPlanet_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-1.0f), AsoUtility::AXIS_Z));

	anim_->Play(static_cast<int>(ANIM_TYPE::RUN));

	spherPlanet_.Update();
	anim_->Update();
}

void TitleScene::Draw(void)
{
	skydome_->Draw();

	// ƒ‚ƒfƒ‹•`‰æ
	MV1DrawModel(bigPlanet_.modelId);
	MV1DrawModel(spherPlanet_.modelId);
	MV1DrawModel(player_.modelId);


	//UI•`‰æ
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 3,
		1.0f,
		0.0f,
		imgTitle_,
		TRUE);

	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		(Application::SCREEN_SIZE_Y -
			(Application::SCREEN_SIZE_Y / 3)),
		1.0f,
		0.0f,
		imgPushSpace_,
		TRUE);
}

void TitleScene::Release(void)
{
	skydome_->Release();
	delete skydome_;

	//ƒ‚ƒfƒ‹‰ð•ú
	MV1DeleteModel(bigPlanet_.modelId);
	MV1DeleteModel(spherPlanet_.modelId);
	MV1DeleteModel(player_.modelId);

	//UI‰ð•ú
	DeleteGraph(imgTitle_);
	DeleteGraph(imgPushSpace_);

	anim_->Release();
	delete anim_;
}
