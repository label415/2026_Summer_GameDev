#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/FontManager.h"
#include "../Manager/SoundManager.h"
#include "SceneBase.h"

SceneBase::SceneBase(void) 
	: 
	resMng_(ResourceManager::GetInstance()),
	sceMng_(SceneManager::GetInstance()),
	fontMng_(FontManager::GetInstance()),
	sonMng_(SoundManager::GetInstance())
{
}

SceneBase::~SceneBase(void)
{
}
