#include "Pch.h"
#include "Bullet.h"

namespace
{
	constexpr float kBulletLifeBorder = 1200.0f;
}

void Bullet::Init(const Math::Vector2& startPos, float angleDeg)
{
	m_pos = startPos;
	m_angleDeg = angleDeg;

	const float rad = DirectX::XMConvertToRadians(angleDeg);
	m_dir = { sinf(rad), -cosf(rad) };

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/bullet.png");
}

void Bullet::Update()
{
	if (m_isExpired) { return; }

	m_pos += m_dir * m_speed;

	if (fabsf(m_pos.x) > kBulletLifeBorder || fabsf(m_pos.y) > kBulletLifeBorder)
	{
		m_isExpired = true;
	}
}

void Bullet::DrawSprite() const
{
	if (!m_spTex) { return; }

	Math::Matrix mat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_angleDeg));
	mat *= Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex.get(), 0, 0, 32, 32);
}