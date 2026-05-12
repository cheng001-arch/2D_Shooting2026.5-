#include "PlayerPlanet.h"
#include "Application/main.h"

#include <algorithm>
#include <cmath>
#include <random>

void PlayerPlanet::Init()
{
	m_drawType = eDrawTypeUI;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/diqiu.png");

	m_spDeathExplosionTex = std::make_shared<KdTexture>();
	m_spDeathExplosionTex->Load("Asset/Textures/baozha4.png");

	SetPos(m_pos);
}

void PlayerPlanet::DrawSprite()
{
	if (!m_spTex) { return; }

	UpdateDeathExplosion();
	if (m_debrisStarted)
	{
		m_deathFrame += Application::Instance().GetDeltaTime();
		DrawDebris();
		return;
	}

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

	DrawDeathExplosions();
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
	m_deathStarted = false;
	m_debrisStarted = false;
	m_deathFrame = 0.0f;
	m_deathExplosionSpawnFrame = 0.0f;
	m_deathExplosions.clear();
	m_debris.clear();
}

void PlayerPlanet::Damage(int damage)
{
	ShakeDamage();

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isExpired = true;
		BeginDeathExplosion();
	}
}

void PlayerPlanet::ShakeDamage()
{
	m_damageShakeFrame = m_damageShakeDuration;
}

void PlayerPlanet::BeginDeathExplosion()
{
	if (m_deathStarted) { return; }

	m_deathStarted = true;
	m_debrisStarted = false;
	m_deathFrame = 0.0f;
	m_deathExplosionSpawnFrame = 0.0f;
	m_deathExplosions.clear();
	m_debris.clear();
}

void PlayerPlanet::UpdateDeathExplosion()
{
	if (!m_deathStarted || m_debrisStarted) { return; }

	const float dt = Application::Instance().GetDeltaTime();
	m_deathFrame += dt;
	m_deathExplosionSpawnFrame += dt;

	static std::mt19937 randEngine{ std::random_device{}() };
	static std::uniform_real_distribution<float> xDist(-620.0f, 620.0f);
	static std::uniform_real_distribution<float> yDist(-360.0f, 70.0f);
	static std::uniform_real_distribution<float> sizeDist(150.0f, 320.0f);
	static std::uniform_real_distribution<float> frameDist(0.0f, 7.99f);

	while (m_deathExplosionSpawnFrame >= m_deathExplosionSpawnInterval && m_deathFrame <= m_deathExplosionDuration)
	{
		m_deathExplosionSpawnFrame -= m_deathExplosionSpawnInterval;

		DeathExplosion explosion;
		explosion.pos = { m_pos.x + xDist(randEngine), yDist(randEngine) };
		const float size = sizeDist(randEngine);
		explosion.size = { size, size };
		explosion.frame = frameDist(randEngine);
		m_deathExplosions.push_back(explosion);
	}

	for (DeathExplosion& explosion : m_deathExplosions)
	{
		explosion.frame += (m_deathExplosionAnimFps / 60.0f) * dt;
		if (explosion.frame >= static_cast<float>(m_deathExplosionFrameCount))
		{
			explosion.frame = std::fmod(explosion.frame, static_cast<float>(m_deathExplosionFrameCount));
		}
	}

	if (m_deathFrame >= m_deathExplosionDuration)
	{
		SetupDebris();
		m_deathExplosions.clear();
		m_debrisStarted = true;
	}
}

void PlayerPlanet::DrawDeathExplosions()
{
	if (!m_deathStarted || m_debrisStarted || !m_spDeathExplosionTex) { return; }

	for (const DeathExplosion& explosion : m_deathExplosions)
	{
		const int frame = std::clamp(static_cast<int>(explosion.frame), 0, m_deathExplosionFrameCount - 1);
		const Math::Rectangle srcRect = {
			frame * m_deathExplosionFrameWidth,
			m_deathExplosionRowIndex * m_deathExplosionFrameHeight,
			m_deathExplosionFrameWidth,
			m_deathExplosionFrameHeight
		};

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spDeathExplosionTex.get(),
			static_cast<int>(explosion.pos.x),
			static_cast<int>(explosion.pos.y),
			static_cast<int>(explosion.size.x),
			static_cast<int>(explosion.size.y),
			&srcRect);
	}
}

void PlayerPlanet::SetupDebris()
{
	if (!m_spTex || !m_debris.empty()) { return; }

	const int texW = static_cast<int>(m_spTex->GetWidth());
	const int texH = static_cast<int>(m_spTex->GetHeight());
	if (texW <= 0 || texH <= 0) { return; }

	const int cols = 4;
	const int rows = 3;
	const int pieceW = texW / cols;
	const int pieceH = texH / rows;
	const float scaleX = m_size.x / static_cast<float>(texW);
	const float scaleY = m_size.y / static_cast<float>(texH);

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			if ((row + col) % 4 == 0) { continue; }

			PlanetDebris debris;
			debris.srcRect = { col * pieceW, row * pieceH, pieceW, pieceH };
			debris.baseOffset = {
				(col * pieceW + pieceW * 0.5f - texW * 0.5f) * scaleX * 0.55f,
				(row * pieceH + pieceH * 0.5f - texH * 0.5f) * scaleY * 0.22f + 230.0f
			};
			debris.velocity = {
				(col - 1.5f) * 1.8f,
				(row - 1.0f) * 0.8f - 1.2f
			};
			debris.angle = DirectX::XMConvertToRadians(static_cast<float>((row * cols + col) * 17));
			debris.angularSpeed = DirectX::XMConvertToRadians((col % 2 == 0 ? -0.4f : 0.55f));
			m_debris.push_back(debris);
		}
	}
}

void PlayerPlanet::DrawDebris()
{
	if (!m_spTex) { return; }

	const int texW = static_cast<int>(m_spTex->GetWidth());
	const int texH = static_cast<int>(m_spTex->GetHeight());
	if (texW <= 0 || texH <= 0) { return; }

	const float scaleX = m_size.x / static_cast<float>(texW);
	const float scaleY = m_size.y / static_cast<float>(texH);
	const float breakFrame = std::max(0.0f, m_deathFrame - m_deathExplosionDuration);

	for (const PlanetDebris& debris : m_debris)
	{
		const Math::Vector2 drift = debris.velocity * breakFrame;
		const Math::Vector2 drawPos = m_pos + debris.baseOffset + drift;
		const float angle = DirectX::XMConvertToRadians(180.0f) + debris.angle + debris.angularSpeed * breakFrame;

		Math::Matrix mat =
			Math::Matrix::CreateRotationZ(angle) *
			Math::Matrix::CreateTranslation(drawPos.x, drawPos.y, 0.0f);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spTex.get(),
			0,
			0,
			static_cast<int>(debris.srcRect.width * scaleX * 0.58f),
			static_cast<int>(debris.srcRect.height * scaleY * 0.58f),
			&debris.srcRect);
	}

	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}
