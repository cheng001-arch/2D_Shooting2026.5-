#include "Pch.h"
#include "Player.h"

void Player::Init()
{
	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/player.png");
}

void Player::Update()
{
	Move();
	Shot();

	for (Bullet& bullet : m_bullets)
	{
		bullet.Update();
	}

	m_bullets.erase(
		std::remove_if(m_bullets.begin(), m_bullets.end(), [](const Bullet& b) { return b.IsExpired(); }),
		m_bullets.end());
}

void Player::DrawSprite() const
{
	if (m_spTex)
	{
		Math::Matrix mat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_angleDeg));
		mat *= Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex.get(), 0, 0, 80, 80);
	}

	for (const Bullet& bullet : m_bullets)
	{
		bullet.DrawSprite();
	}
}

void Player::Move()
{
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_angleDeg -= m_turnSpeedDeg;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_angleDeg += m_turnSpeedDeg;
	}

	const float rad = DirectX::XMConvertToRadians(m_angleDeg);
	Math::Vector2 forward = { sinf(rad), -cosf(rad) };

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pos += forward * m_moveSpeed;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pos -= forward * m_moveSpeed;
	}
}

void Player::Shot()
{
	if (!(GetAsyncKeyState('J') & 0x0001)) { return; }

	Bullet newBullet;
	newBullet.Init(m_pos, m_angleDeg);
	m_bullets.push_back(newBullet);
}
