#include "ProgressSystem.h"
#include "Application/main.h"

#include <algorithm>

namespace
{
float EaseOutCubic(float t)
{
	t = std::clamp(t, 0.0f, 1.0f);
	const float inv = 1.0f - t;
	return 1.0f - inv * inv * inv;
}
}

void ProgressSystem::Init()
{
	m_spYesTex = std::make_shared<KdTexture>();
	m_spYesTex->Load("Asset/Textures/yes.png");
}

void ProgressSystem::Reset()
{
	m_progress = 0;
	m_completeWaitFrame = 0.0f;
	m_yesAnimFrame = 0.0f;
	m_yesAnimStarted = false;
}

void ProgressSystem::Update()
{
	if ((GetAsyncKeyState('P') & 0x8000) && !IsComplete())
	{
		SetProgress(95);
	}

	if (!IsComplete()) { return; }

	if (m_completeWaitFrame < m_completeWaitDuration)
	{
		m_completeWaitFrame += Application::Instance().GetDeltaTime();
		return;
	}

	m_yesAnimStarted = true;
	m_yesAnimFrame = std::min(m_yesAnimFrame + Application::Instance().GetDeltaTime(), m_yesAnimDuration);
}

void ProgressSystem::DrawSprite()
{
	DrawProgressGauge();
	DrawClearEffect();
}

void ProgressSystem::AddEnemyProgress(int value)
{
	if (value <= 0 || IsComplete()) { return; }

	SetProgress(m_progress + value);
}

void ProgressSystem::SetProgress(int value)
{
	m_progress = std::clamp(value, 0, m_maxProgress);
}

void ProgressSystem::DrawProgressGauge()
{
	const Math::Color frameColor = Math::Color(0.05f, 0.05f, 0.08f, 0.92f);
	const Math::Color emptyColor = Math::Color(0.16f, 0.16f, 0.18f, 0.86f);
	const Math::Color redColor = Math::Color(1.0f, 0.12f, 0.08f, 1.0f);
	const Math::Color yellowColor = Math::Color(1.0f, 0.86f, 0.06f, 1.0f);
	const Math::Color blueColor = Math::Color(0.12f, 0.44f, 1.0f, 1.0f);
	const Math::Color shadowColor = Math::Color(0.0f, 0.0f, 0.0f, 0.9f);

	KdShaderManager::Instance().m_spriteShader.DrawBox(
		static_cast<int>(m_gaugePos.x),
		static_cast<int>(m_gaugePos.y),
		static_cast<int>(m_gaugeSize.x * 0.5f) + 5,
		static_cast<int>(m_gaugeSize.y * 0.5f) + 5,
		&frameColor,
		true);

	KdShaderManager::Instance().m_spriteShader.DrawBox(
		static_cast<int>(m_gaugePos.x),
		static_cast<int>(m_gaugePos.y),
		static_cast<int>(m_gaugeSize.x * 0.5f),
		static_cast<int>(m_gaugeSize.y * 0.5f),
		&emptyColor,
		true);

	const float progressRate = static_cast<float>(m_progress) / static_cast<float>(m_maxProgress);
	const int fillWidth = static_cast<int>(m_gaugeSize.x * progressRate);
	const int segmentCount = 24;
	const int segmentWidth = static_cast<int>(m_gaugeSize.x / segmentCount);
	const int left = static_cast<int>(m_gaugePos.x - m_gaugeSize.x * 0.5f);
	const int centerY = static_cast<int>(m_gaugePos.y);
	const int halfH = static_cast<int>(m_gaugeSize.y * 0.5f);

	for (int i = 0; i < segmentCount; ++i)
	{
		const int segLeft = left + i * segmentWidth;
		const int segRight = i == segmentCount - 1 ? left + static_cast<int>(m_gaugeSize.x) : segLeft + segmentWidth;
		const int visibleRight = std::min(left + fillWidth, segRight);
		if (visibleRight <= segLeft) { break; }

		const int drawW = visibleRight - segLeft;
		const int drawCenterX = segLeft + drawW / 2;
		const Math::Color* color = &redColor;
		if (i % 3 == 1)
		{
			color = &yellowColor;
		}
		else if (i % 3 == 2)
		{
			color = &blueColor;
		}

		KdShaderManager::Instance().m_spriteShader.DrawBox(
			drawCenterX,
			centerY,
			drawW / 2,
			halfH,
			color,
			true);
	}

	KdShaderManager::Instance().m_spriteShader.DrawFont(
		{ m_gaugePos.x - 30.0f, m_gaugePos.y - 17.0f },
		&shadowColor,
		"%d%%",
		m_progress);
	KdShaderManager::Instance().m_spriteShader.DrawFont(
		{ m_gaugePos.x - 32.0f, m_gaugePos.y - 19.0f },
		&kWhiteColor,
		"%d%%",
		m_progress);
}

void ProgressSystem::DrawClearEffect()
{
	if (!m_yesAnimStarted || !m_spYesTex) { return; }

	const int texW = static_cast<int>(m_spYesTex->GetInfo().Width);
	const int texH = static_cast<int>(m_spYesTex->GetInfo().Height);
	if (texW <= 0 || texH <= 0) { return; }

	const int halfW = texW / 2;
	const Math::Rectangle leftSrc = { 0, 0, halfW, texH };
	const Math::Rectangle rightSrc = { halfW, 0, texW - halfW, texH };

	const float rate = EaseOutCubic(m_yesAnimFrame / m_yesAnimDuration);
	const float leftStartX = -640.0f - static_cast<float>(halfW);
	const float rightStartX = 640.0f + static_cast<float>(texW - halfW);
	const float leftEndX = -static_cast<float>(texW) * 0.25f;
	const float rightEndX = static_cast<float>(texW) * 0.25f;
	const int leftX = static_cast<int>(leftStartX + (leftEndX - leftStartX) * rate);
	const int rightX = static_cast<int>(rightStartX + (rightEndX - rightStartX) * rate);

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spYesTex.get(),
		leftX,
		0,
		halfW,
		texH,
		&leftSrc);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spYesTex.get(),
		rightX,
		0,
		texW - halfW,
		texH,
		&rightSrc);
}
