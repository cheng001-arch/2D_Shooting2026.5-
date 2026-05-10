#include "PlayerPlanet.h"
#include "Application/main.h"

#include <algorithm>
#include <cmath>

void PlayerPlanet::Init()
{
	m_drawType = eDrawTypeUI;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/diqiu.png");

	SetPos(m_pos);
}

void PlayerPlanet::DrawSprite()
{
	if (!m_spTex) { return; }

	Math::Vector2 drawPos = m_pos;
	float shakeAngle = 0.0f;
	if (m_damageShakeFrame > 0.0f)
	{
		const float shakeRate = m_damageShakeFrame / m_damageShakeDuration;
		shakeAngle = std::sin(m_damageShakeFrame * 1.4f) * DirectX::XMConvertToRadians(m_damageShakeAngle) * shakeRate;
		drawPos.x += std::sin(m_damageShakeFrame * 2.5f) * m_damageShakePowerX * shakeRate;
		drawPos.y += std::cos(m_damageShakeFrame * 3.2f) * m_damageShakePowerY * shakeRate;
		m_damageShakeFrame = std::max(0.0f, m_damageShakeFrame - Application::Instance().GetDeltaTime());
	}

	Math::Matrix mat =
		Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(180.0f) + shakeAngle) *
		Math::Matrix::CreateTranslation(drawPos.x, drawPos.y, 0.0f);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTex.get(),
		0,
		0,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y));
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void PlayerPlanet::SetPos(const Math::Vector2& pos)
{
	m_pos = pos;
	m_mWorld = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
}

void PlayerPlanet::SetPos(const Math::Vector3& pos)
{
	SetPos({ pos.x, pos.y });
}

Math::Vector2 PlayerPlanet::GetTopPos() const
{
	return { m_pos.x, m_pos.y + m_size.y * 0.5f };
}

void PlayerPlanet::ResetHp()
{
	m_hp = 20;
	m_isExpired = false;
}

void PlayerPlanet::Damage(int damage)
{
	ShakeDamage();

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isExpired = true;
	}
}

void PlayerPlanet::ShakeDamage()
{
	m_damageShakeFrame = m_damageShakeDuration;
}
