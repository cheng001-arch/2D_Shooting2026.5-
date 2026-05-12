#include "ExplosionManager.h"
#include "Application/main.h"

#include <algorithm>

void ExplosionManager::Init()
{
	m_spProjectileHitTex = std::make_shared<KdTexture>();
	m_spProjectileHitTex->Load("Asset/Textures/baozha.png");

	m_spPlanetHitTex = std::make_shared<KdTexture>();
	m_spPlanetHitTex->Load("Asset/Textures/baozha2.png");

	m_spPlanetHitAccentTex = std::make_shared<KdTexture>();
	m_spPlanetHitAccentTex->Load("Asset/Textures/baozha3.png");
}

void ExplosionManager::Reset()
{
	m_explosions.clear();
}

void ExplosionManager::Update()
{
	const float addFrame = m_animFps / 60.0f * Application::Instance().GetDeltaTime();

	for (Explosion& explosion : m_explosions)
	{
		explosion.frame += addFrame;
	}

	m_explosions.erase(
		std::remove_if(
			m_explosions.begin(),
			m_explosions.end(),
			[this](const Explosion& explosion)
			{
				return explosion.frame >= static_cast<float>(m_frameCount);
			}),
		m_explosions.end());
}

void ExplosionManager::DrawSprite()
{
	for (const Explosion& explosion : m_explosions)
	{
		std::shared_ptr<KdTexture> tex = m_spProjectileHitTex;
		int row = m_projectileHitRowIndex;

		if (explosion.type == ExplosionType::PlanetHit)
		{
			tex = m_spPlanetHitTex;
			row = m_planetHitRowIndex;
		}
		else if (explosion.type == ExplosionType::PlanetHitAccent)
		{
			tex = m_spPlanetHitAccentTex;
			row = m_planetHitAccentRowIndex;
		}

		if (!tex) { continue; }

		const int frame = std::clamp(static_cast<int>(explosion.frame), 0, m_frameCount - 1);
		const int col = frame % m_frameCount;
		const Math::Rectangle srcRect = {
			col * m_frameWidth,
			row * m_frameHeight,
			m_frameWidth,
			m_frameHeight
		};

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			tex.get(),
			static_cast<int>(explosion.pos.x),
			static_cast<int>(explosion.pos.y),
			static_cast<int>(explosion.drawSize.x),
			static_cast<int>(explosion.drawSize.y),
			&srcRect);
	}
}

void ExplosionManager::Spawn(const Math::Vector2& pos)
{
	Explosion explosion;
	explosion.pos = pos;
	explosion.drawSize = m_projectileHitDrawSize;
	explosion.type = ExplosionType::ProjectileHit;
	m_explosions.push_back(explosion);
}

void ExplosionManager::SpawnLarge(const Math::Vector2& pos, float size)
{
	Explosion explosion;
	explosion.pos = pos;
	explosion.drawSize = { size, size };
	explosion.type = ExplosionType::ProjectileHit;
	m_explosions.push_back(explosion);
}

void ExplosionManager::SpawnPlanetHit(const Math::Vector2& pos, float enemyRadius)
{
	const float size = std::clamp(enemyRadius * 3.0f, 96.0f, 320.0f);

	Explosion explosion;
	explosion.pos = pos;
	explosion.drawSize = { size, size };
	explosion.type = ExplosionType::PlanetHit;
	m_explosions.push_back(explosion);

	Explosion accent;
	accent.pos = pos;
	accent.drawSize = { size * 1.15f, size * 1.15f };
	accent.type = ExplosionType::PlanetHitAccent;
	m_explosions.push_back(accent);
}
