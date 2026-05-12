#pragma once

class PlayerPlanet : public KdGameObject
{
public:
	PlayerPlanet() {}
	~PlayerPlanet() override {}

	void Init() override;
	void DrawSprite() override;

	void SetPos(const Math::Vector2& pos);
	void SetPos(const Math::Vector3& pos) override;

	void SetSize(const Math::Vector2& size) { m_size = size; }
	void ResetHp();
	void Damage(int damage);

	Math::Vector2 GetSpritePos() const { return m_pos; }
	Math::Vector2 GetSize() const { return m_size; }
	Math::Vector2 GetTopPos() const;
	float GetRadius() const { return m_size.x * 0.28f; }
	int GetHp() const { return m_hp; }

private:
	struct DeathExplosion
	{
		Math::Vector2 pos = Math::Vector2::Zero;
		Math::Vector2 size = { 180.0f, 180.0f };
		float frame = 0.0f;
	};

	struct PlanetDebris
	{
		Math::Rectangle srcRect = {};
		Math::Vector2 baseOffset = Math::Vector2::Zero;
		Math::Vector2 velocity = Math::Vector2::Zero;
		float angle = 0.0f;
		float angularSpeed = 0.0f;
	};

	void ShakeDamage();
	void BeginDeathExplosion();
	void UpdateDeathExplosion();
	void DrawDeathExplosions();
	void SetupDebris();
	void DrawDebris();

	std::shared_ptr<KdTexture> m_spTex = nullptr;
	std::shared_ptr<KdTexture> m_spDeathExplosionTex = nullptr;
	std::vector<DeathExplosion> m_deathExplosions;
	std::vector<PlanetDebris> m_debris;

	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_size = { 2400.0f, 2400.0f };

	float m_damageShakeFrame = 0.0f;
	float m_damageShakeDuration = 36.0f;
	float m_damageShakeAngle = 6.0f;
	float m_damageShakePowerX = 35.0f;
	float m_damageShakePowerY = 14.0f;
	float m_deathFrame = 0.0f;
	float m_deathExplosionDuration = 180.0f;
	float m_deathExplosionSpawnFrame = 0.0f;
	float m_deathExplosionSpawnInterval = 5.0f;
	float m_deathExplosionAnimFps = 20.0f;
	int m_deathExplosionFrameWidth = 64;
	int m_deathExplosionFrameHeight = 64;
	int m_deathExplosionFrameCount = 8;
	int m_deathExplosionRowIndex = 7;
	int m_hp = 20;
	bool m_deathStarted = false;
	bool m_debrisStarted = false;
};
