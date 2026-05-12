#pragma once

class ExplosionManager
{
public:
	void Init();
	void Reset();
	void Update();
	void DrawSprite();
	void Spawn(const Math::Vector2& pos);
	void SpawnLarge(const Math::Vector2& pos, float size);
	void SpawnPlanetHit(const Math::Vector2& pos, float enemyRadius);

private:
	enum class ExplosionType
	{
		ProjectileHit,
		PlanetHit,
		PlanetHitAccent
	};

	struct Explosion
	{
		Math::Vector2 pos = Math::Vector2::Zero;
		Math::Vector2 drawSize = { 96.0f, 96.0f };
		ExplosionType type = ExplosionType::ProjectileHit;
		float frame = 0.0f;
	};

	std::shared_ptr<KdTexture> m_spProjectileHitTex = nullptr;
	std::shared_ptr<KdTexture> m_spPlanetHitTex = nullptr;
	std::shared_ptr<KdTexture> m_spPlanetHitAccentTex = nullptr;
	std::vector<Explosion> m_explosions;

	int m_frameWidth = 64;
	int m_frameHeight = 64;
	int m_frameCount = 9;
	int m_projectileHitRowIndex = 4;
	int m_planetHitRowIndex = 7;
	int m_planetHitAccentRowIndex = 0;
	float m_animFps = 20.0f;
	Math::Vector2 m_projectileHitDrawSize = { 96.0f, 96.0f };
};
