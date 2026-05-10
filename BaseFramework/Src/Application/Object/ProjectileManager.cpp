#include "ProjectileManager.h"
#include "Application/main.h"

#include <algorithm>
#include <cmath>

void ProjectileManager::Init()
{
	m_spMeteorTex = std::make_shared<KdTexture>();
	m_spMeteorTex->Load("Asset/Textures/yunshi2.png");

	m_spMimicPlanetTex = std::make_shared<KdTexture>();
	m_spMimicPlanetTex->Load("Asset/Textures/nitaixingqiu.png");
}

void ProjectileManager::Update()
{
	const float dt = Application::Instance().GetDeltaTime();

	for (Projectile& projectile : m_projectiles)
	{
		projectile.pos += projectile.velocity * dt;
	}

	m_projectiles.erase(
		std::remove_if(
			m_projectiles.begin(),
			m_projectiles.end(),
			[](const Projectile& projectile)
			{
				return
					projectile.pos.x < -900.0f ||
					projectile.pos.x > 900.0f ||
					projectile.pos.y < -700.0f ||
					projectile.pos.y > 700.0f;
			}),
		m_projectiles.end());
}

void ProjectileManager::DrawSprite()
{
	for (const Projectile& projectile : m_projectiles)
	{
		if (!projectile.tex) { continue; }

		Math::Matrix mat =
			Math::Matrix::CreateRotationZ(projectile.angle) *
			Math::Matrix::CreateTranslation(projectile.pos.x, projectile.pos.y, 0.0f);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			projectile.tex.get(),
			0,
			0,
			static_cast<int>(projectile.size.x),
			static_cast<int>(projectile.size.y));
	}

	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void ProjectileManager::Spawn(const Math::Vector2& pos, const Math::Vector2& direction, AmmoType ammoType)
{
	if (direction.LengthSquared() <= 0.001f) { return; }

	Math::Vector2 normalizedDir = direction;
	normalizedDir.Normalize();

	Projectile projectile;
	projectile.pos = pos;
	projectile.angle = static_cast<float>(std::atan2(normalizedDir.y, normalizedDir.x)) + DirectX::XMConvertToRadians(-90.0f);

	switch (ammoType)
	{
	case AmmoType::MimicPlanet:
		projectile.tex = m_spMimicPlanetTex;
		projectile.size = m_mimicPlanetSize;
		projectile.velocity = normalizedDir * m_mimicPlanetSpeed;
		projectile.radius = m_mimicPlanetRadius;
		projectile.attackPower = 4;
		break;
	case AmmoType::Meteor:
	default:
		projectile.tex = m_spMeteorTex;
		projectile.size = m_meteorSize;
		projectile.velocity = normalizedDir * m_meteorSpeed;
		projectile.radius = m_meteorRadius;
		projectile.attackPower = 1;
		break;
	}

	m_projectiles.push_back(projectile);
}

void ProjectileManager::CleanupExpiredProjectiles()
{
	m_projectiles.erase(
		std::remove_if(
			m_projectiles.begin(),
			m_projectiles.end(),
			[](const Projectile& projectile)
			{
				return projectile.attackPower <= 0;
			}),
		m_projectiles.end());
}
