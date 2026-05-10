#include "ResultScene.h"
#include "Application/main.h"
#include "Application/Scene/SceneManager.h"

#include <algorithm>

void ResultScene::Init(SceneManager* sceneManager)
{
	m_sceneManager = sceneManager;

	m_spBackgroundTex = std::make_shared<KdTexture>();
	m_spBackgroundTex->Load("Asset/Textures/zanting.png");
}

void ResultScene::ResetStats()
{
	m_kills = 0;
	m_miss = 0;
	m_damage = 0.0f;
	m_timeSeconds = 0.0f;
	m_resultFrame = 0.0f;
}

void ResultScene::AddKill(int value)
{
	m_kills += value;
}

void ResultScene::AddMiss(int value)
{
	m_miss += value;
}

void ResultScene::AddDamage(float value)
{
	m_damage += std::max(0.0f, value);
}

void ResultScene::AddTime(float seconds)
{
	m_timeSeconds += std::max(0.0f, seconds);
}

void ResultScene::Start()
{
	m_resultFrame = 0.0f;
}

void ResultScene::Update()
{
	m_resultFrame += Application::Instance().GetDeltaTime();
	if (m_resultFrame >= m_resultDuration && m_sceneManager)
	{
		m_sceneManager->ChangeScene(SceneManager::SceneType::StageSelect);
	}
}

void ResultScene::DrawSprite()
{
	if (m_spBackgroundTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spBackgroundTex.get(),
			0,
			0,
			1280,
			720);
	}

	DrawResultText();
}

void ResultScene::DrawResultText()
{
	const Math::Color red = Math::Color(1.0f, 0.12f, 0.08f, 1.0f);
	const Math::Color yellow = Math::Color(1.0f, 0.86f, 0.06f, 1.0f);
	const Math::Color green = Math::Color(0.12f, 1.0f, 0.36f, 1.0f);
	const Math::Color shadow = Math::Color(0.0f, 0.0f, 0.0f, 0.86f);

	const int minutes = static_cast<int>(m_timeSeconds) / 60;
	const int seconds = static_cast<int>(m_timeSeconds) % 60;

	KdShaderManager::Instance().m_spriteShader.DrawFont({ -222.0f, 128.0f }, &shadow, "Kills  %d", m_kills);
	KdShaderManager::Instance().m_spriteShader.DrawFont({ -225.0f, 132.0f }, &red, "Kills  %d", m_kills);

	KdShaderManager::Instance().m_spriteShader.DrawFont({ -222.0f, 58.0f }, &shadow, "Miss   %d", m_miss);
	KdShaderManager::Instance().m_spriteShader.DrawFont({ -225.0f, 62.0f }, &yellow, "Miss   %d", m_miss);

	KdShaderManager::Instance().m_spriteShader.DrawFont({ -222.0f, -12.0f }, &shadow, "Damage %.1f", m_damage);
	KdShaderManager::Instance().m_spriteShader.DrawFont({ -225.0f, -8.0f }, &green, "Damage %.1f", m_damage);

	KdShaderManager::Instance().m_spriteShader.DrawFont({ -222.0f, -82.0f }, &shadow, "Time   %02d:%02d", minutes, seconds);
	KdShaderManager::Instance().m_spriteShader.DrawFont({ -225.0f, -78.0f }, &red, "Time   %02d:%02d", minutes, seconds);
}
