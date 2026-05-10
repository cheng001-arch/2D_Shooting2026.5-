#include "Enemy.h"
#include "Application/main.h"

#include <algorithm>
#include <cmath>

void Enemy::Init()
{
	m_drawType = eDrawTypeUI;
	SetPos(m_pos);
}

void Enemy::Update()
{
	m_isGravityAttracted = false;
	m_moveDir = { 0.0f, -1.0f };
	m_pos.y -= m_fallSpeed * Application::Instance().GetDeltaTime();
	SetPos(m_pos);

	if (m_pos.y < -500.0f)
	{
		m_isExpired = true;
	}
}

void Enemy::Update(const Math::Vector2& planetCenter, float planetRadius)
{
	if (m_isBlackHoleCaptured)
	{
		m_isGravityAttracted = false;
		SetPos(m_pos);
		return;
	}

	if (m_isComet)
	{
		m_isGravityAttracted = false;
		const Math::Vector2 prevPos = m_pos;
		m_cometT += m_cometPathSpeed * Application::Instance().GetDeltaTime();
		const float t = std::min(m_cometT, 1.0f);
		const float invT = 1.0f - t;

		m_pos =
			m_cometStart * (invT * invT) +
			m_cometControl * (2.0f * invT * t) +
			m_cometEnd * (t * t);

		const Math::Vector2 moveDir = m_pos - prevPos;
		if (moveDir.LengthSquared() > 0.001f)
		{
			m_angle = static_cast<float>(std::atan2(moveDir.y, moveDir.x)) + DirectX::XMConvertToRadians(135.0f);
		}

		SetPos(m_pos);

		if (m_cometT >= 1.0f)
		{
			m_isExpired = true;
		}
		return;
	}

	const Math::Vector2 toPlanet = planetCenter - m_pos;
	const float gravityRange = planetRadius + m_radius + 200.0f;
	if (toPlanet.LengthSquared() <= gravityRange * gravityRange && toPlanet.LengthSquared() > 0.001f)
	{
		Math::Vector2 gravityDir = toPlanet;
		gravityDir.Normalize();
		m_isGravityAttracted = true;
		m_moveDir = gravityDir;
		m_gravityTrailFrame += (m_gravityTrailAnimFps / 60.0f) * Application::Instance().GetDeltaTime();
		if (m_gravityTrailFrame >= static_cast<float>(m_gravityTrailFrameCount))
		{
			m_gravityTrailFrame = 0.0f;
		}
		m_pos += gravityDir * m_fallSpeed * Application::Instance().GetDeltaTime();
	}
	else
	{
		m_isGravityAttracted = false;
		m_moveDir = { 0.0f, -1.0f };
		m_pos.y -= m_fallSpeed * Application::Instance().GetDeltaTime();
	}

	SetPos(m_pos);

	if (m_pos.y < -500.0f)
	{
		m_isExpired = true;
	}
}

void Enemy::DrawSprite()
{
	if (!m_spTex) { return; }

	if (m_isGravityAttracted && m_spGravityTrailTex)
	{
		const Math::Vector2 trailDir = m_moveDir * -1.0f;
		const Math::Vector2 trailPos = m_pos + trailDir * (m_radius * 0.9f);
		const float trailAngle = static_cast<float>(std::atan2(trailDir.y, trailDir.x)) + DirectX::XMConvertToRadians(-90.0f);
		const int frame = std::clamp(static_cast<int>(m_gravityTrailFrame), 0, m_gravityTrailFrameCount - 1);
		const Math::Rectangle srcRect = {
			frame * m_gravityTrailFrameWidth,
			m_gravityTrailRowIndex * m_gravityTrailFrameHeight,
			m_gravityTrailFrameWidth,
			m_gravityTrailFrameHeight
		};
		const float trailScale = std::clamp(m_radius / 32.0f, 1.0f, 3.0f);
		const Math::Vector2 trailSize = { 72.0f * trailScale, 72.0f * trailScale };

		Math::Matrix trailMat =
			Math::Matrix::CreateRotationZ(trailAngle) *
			Math::Matrix::CreateTranslation(trailPos.x, trailPos.y, 0.0f);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(trailMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spGravityTrailTex.get(),
			0,
			0,
			static_cast<int>(trailSize.x),
			static_cast<int>(trailSize.y),
			&srcRect);
	}

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

void Enemy::SetPos(const Math::Vector2& pos)
{
	m_pos = pos;
	m_mWorld = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
}

void Enemy::SetPos(const Math::Vector3& pos)
{
	SetPos({ pos.x, pos.y });
}

void Enemy::SetStatus(float hp, int attackPower, int energyReward, float fallSpeed, const Math::Vector2& size, float radius)
{
	m_hp = hp;
	m_attackPower = attackPower;
	m_energyReward = energyReward;
	m_fallSpeed = fallSpeed;
	m_size = size;
	m_radius = radius;
}

void Enemy::SetCometPath(const Math::Vector2& start, const Math::Vector2& control, const Math::Vector2& end, float pathSpeed)
{
	m_isComet = true;
	m_cometStart = start;
	m_cometControl = control;
	m_cometEnd = end;
	m_cometPathSpeed = pathSpeed;
	m_cometT = 0.0f;
	SetPos(start);
}

void Enemy::Damage(float attackPower)
{
	m_hp -= attackPower;

	if (m_hp <= 0)
	{
		m_isExpired = true;
	}
}
