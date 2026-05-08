#include "Turret.h"
#include "Application/main.h"

#include <cmath>

void Turret::Init()
{
	m_drawType = eDrawTypeUI;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/paotai3.png");

	SetPos(m_pos);
}

void Turret::Update()
{
	const Math::Vector2 mousePos = GetMouseSpritePos();
	const Math::Vector2 toMouse = mousePos - m_pos;

	if (toMouse.LengthSquared() <= 0.001f) { return; }

	m_angle = static_cast<float>(std::atan2(toMouse.y, toMouse.x)) + m_angleOffset;
}

void Turret::DrawSprite()
{
	if (!m_spTex) { return; }

	Math::Matrix mat =
		Math::Matrix::CreateRotationZ(m_angle) *
		Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTex.get(),
		0,
		0,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y));
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void Turret::SetPos(const Math::Vector2& pos)
{
	m_pos = pos;
	m_mWorld = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
}

void Turret::SetPos(const Math::Vector3& pos)
{
	SetPos({ pos.x, pos.y });
}

Math::Vector2 Turret::GetMouseSpritePos() const
{
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	RECT clientRect{};
	GetClientRect(Application::Instance().GetWindowHandle(), &clientRect);

	const float clientW = static_cast<float>(clientRect.right - clientRect.left);
	const float clientH = static_cast<float>(clientRect.bottom - clientRect.top);

	return {
		static_cast<float>(mousePos.x) - clientW * 0.5f,
		clientH * 0.5f - static_cast<float>(mousePos.y)
	};
}
