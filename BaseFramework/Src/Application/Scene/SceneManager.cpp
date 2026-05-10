#include "SceneManager.h"

void SceneManager::Init()
{
	m_currentScene = SceneType::Title;
	m_titleScene.Init(this);
	m_stageSelectScene.Init(this);
	m_pauseScene.Init(this);
	m_resultScene.Init(this);
}

void SceneManager::Update()
{
	switch (m_currentScene)
	{
	case SceneType::StageSelect:
		m_stageSelectScene.Update();
		break;
	case SceneType::Title:
		m_titleScene.Update();
		break;
	case SceneType::Game:
	{
		const bool escape = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
		if (escape && !m_prevGameEscape)
		{
			ChangeScene(SceneType::Pause);
		}
		m_prevGameEscape = escape;
		break;
	}
	case SceneType::Pause:
		m_pauseScene.Update();
		break;
	case SceneType::Result:
		m_resultScene.Update();
		break;
	default:
		break;
	}
}

void SceneManager::DrawSprite()
{
	switch (m_currentScene)
	{
	case SceneType::StageSelect:
		m_stageSelectScene.DrawSprite();
		break;
	case SceneType::Title:
		m_titleScene.DrawSprite();
		break;
	case SceneType::Game:
		break;
	case SceneType::Pause:
		m_pauseScene.DrawSprite();
		break;
	case SceneType::Result:
		m_resultScene.DrawSprite();
		break;
	default:
		break;
	}
}

void SceneManager::ChangeScene(SceneType nextScene)
{
	m_currentScene = nextScene;
	m_prevGameEscape = false;
}

void SceneManager::ResetResultStats()
{
	m_resultScene.ResetStats();
}

void SceneManager::AddResultKill(int value)
{
	m_resultScene.AddKill(value);
}

void SceneManager::AddResultMiss(int value)
{
	m_resultScene.AddMiss(value);
}

void SceneManager::AddResultDamage(float value)
{
	m_resultScene.AddDamage(value);
}

void SceneManager::AddResultTime(float seconds)
{
	m_resultScene.AddTime(seconds);
}

void SceneManager::StartResultScene()
{
	m_resultScene.Start();
	ChangeScene(SceneType::Result);
}
