#include "ResultScene.h"
#include "Application/main.h"
#include "Application/Scene/SceneManager.h"

#include <algorithm>

namespace
{
float EaseOutCubic(float t)
{
	t = std::clamp(t, 0.0f, 1.0f);
	const float inv = 1.0f - t;
	return 1.0f - inv * inv * inv;
}

float EaseInCubic(float t)
{
	t = std::clamp(t, 0.0f, 1.0f);
	return t * t * t;
}
}

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

	const Math::Vector2 killsPos = GetAnimatedLinePos(0, { -225.0f, 132.0f });
	const Math::Vector2 missPos = GetAnimatedLinePos(1, { -225.0f, 62.0f });
	const Math::Vector2 damagePos = GetAnimatedLinePos(2, { -225.0f, -8.0f });
	const Math::Vector2 timePos = GetAnimatedLinePos(3, { -225.0f, -78.0f });

	KdShaderManager::Instance().m_spriteShader.DrawFont({ killsPos.x + 3.0f, killsPos.y - 4.0f }, &shadow, "Kills  %d", m_kills);
	KdShaderManager::Instance().m_spriteShader.DrawFont(killsPos, &red, "Kills  %d", m_kills);

	KdShaderManager::Instance().m_spriteShader.DrawFont({ missPos.x + 3.0f, missPos.y - 4.0f }, &shadow, "Miss   %d", m_miss);
	KdShaderManager::Instance().m_spriteShader.DrawFont(missPos, &yellow, "Miss   %d", m_miss);

	KdShaderManager::Instance().m_spriteShader.DrawFont({ damagePos.x + 3.0f, damagePos.y - 4.0f }, &shadow, "Damage %.1f", m_damage);
	KdShaderManager::Instance().m_spriteShader.DrawFont(damagePos, &green, "Damage %.1f", m_damage);

	KdShaderManager::Instance().m_spriteShader.DrawFont({ timePos.x + 3.0f, timePos.y - 4.0f }, &shadow, "Time   %02d:%02d", minutes, seconds);
	KdShaderManager::Instance().m_spriteShader.DrawFont(timePos, &red, "Time   %02d:%02d", minutes, seconds);
}

Math::Vector2 ResultScene::GetAnimatedLinePos(int index, const Math::Vector2& basePos) const
{
	const float enterStart = 18.0f + static_cast<float>(index) * 22.0f;
	const float enterDuration = 48.0f;
	const float settleEnd = m_resultDuration - 110.0f;
	const float exitStart = m_resultDuration - 92.0f + static_cast<float>(index) * 18.0f;
	const float exitDuration = 44.0f;
	const float startX = 760.0f;
	const float settleX = basePos.x + 24.0f;
	const float exitX = -860.0f;

	Math::Vector2 pos = basePos;

	if (m_resultFrame < enterStart)
	{
		pos.x = startX;
		return pos;
	}

	const float enterRate = (m_resultFrame - enterStart) / enterDuration;
	if (enterRate < 1.0f)
	{
		const float t = EaseOutCubic(enterRate);
		pos.x = startX + (settleX - startX) * t;
		return pos;
	}

	if (m_resultFrame < exitStart)
	{
		const float holdDuration = std::max(1.0f, settleEnd - (enterStart + enterDuration));
		const float holdRate = std::clamp((m_resultFrame - enterStart - enterDuration) / holdDuration, 0.0f, 1.0f);
		pos.x = settleX + (basePos.x - settleX) * EaseOutCubic(holdRate);
		return pos;
	}

	const float exitRate = (m_resultFrame - exitStart) / exitDuration;
	const float t = EaseInCubic(exitRate);
	pos.x = basePos.x + (exitX - basePos.x) * t;
	return pos;
}
