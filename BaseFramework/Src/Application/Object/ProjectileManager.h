#pragma once

class ProjectileManager
{
public:
	enum class AmmoType
	{
		Meteor,
		MimicPlanet
	};

	struct Projectile
	{
		Math::Vector2 pos = Math::Vector2::Zero;
		Math::Vector2 velocity = Math::Vector2::Zero;
		Math::Vector2 size = Math::Vector2::Zero;
		std::shared_ptr<KdTexture> tex = nullptr;
		float angle = 0.0f;
		float radius = 24.0f;
		int attackPower = 1;
	};

	void Init();
	void Update();
	void DrawSprite();

	void Spawn(const Math::Vector2& pos, const Math::Vector2& direction, AmmoType ammoType);
	void CleanupExpiredProjectiles();

	std::vector<Projectile>& WorkProjectiles() { return m_projectiles; }

private:
	std::shared_ptr<KdTexture> m_spMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spMimicPlanetTex = nullptr;
	std::vector<Projectile> m_projectiles;

	Math::Vector2 m_meteorSize = { 64.0f, 64.0f };
	Math::Vector2 m_mimicPlanetSize = { 96.0f, 96.0f };
	float m_meteorSpeed = 15.0f;
	float m_mimicPlanetSpeed = 9.0f;
	float m_meteorRadius = 24.0f;
	float m_mimicPlanetRadius = 36.0f;
};
