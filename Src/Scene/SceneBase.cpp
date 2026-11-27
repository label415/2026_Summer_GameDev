#include "../Manager/Common/ResourceManager.h"
#include "../Manager/Common/SceneManager.h"
#include "../Manager/Common/SoundManager.h"
#include "SceneBase.h"

SceneBase::SceneBase(void) 
	: resMng_(ResourceManager::GetInstance())
	, sceMng_(SceneManager::GetInstance())
	, sonMng_(SoundManager::GetInstance())
{
}