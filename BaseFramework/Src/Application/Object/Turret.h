#pragma once

class Turret : public KdGameObject
{
public:
	Turret() {}
	~Turret() override {}

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	void SetPos(const Math::Vector2& pos);
	void SetPos(const Math::Vector3& pos) override;

	void SetSize(const Math::Vector2& size) { m_size = size; }
	void SetAngleOffsetDeg(float offsetDeg) { m_angleOffset = DirectX::XMConvertToRadians(offsetDeg); }
	void ShakeDamage();
	void BeginDeathExplosion();
	void ResetDestruction();
	bool IsDestroyed() const { return m_deathStarted; }

	float GetAngle() const { return m_angle; }
	Math::Vector2 GetPos2D() const { return m_pos; }
	Math::Vector2 GetAimDir() const;
	Math::Vector2 GetMuzzlePos() const;

private:
	struct DeathExplosion
	{
		Math::Vector2 pos = Math::Vector2::Zero;
		Math::Vector2 size = { 120.0f, 120.0f };
		float frame = 0.0f;
	};

	struct Debris
	{
		std::shared_ptr<KdTexture> tex = nullptr;
		Math::Rectangle srcRect = {};
		Math::Vector2 drawSize = Math::Vector2::Zero;
		Math::Vector2 baseOffset = Math::Vector2::Zero;
		Math::Vector2 velocity = Math::Vector2::Zero;
		float angle = 0.0f;
		float angularSpeed = 0.0f;
		float baseAngle = 0.0f;
	};

	Math::Vector2 GetMouseSpritePos() const;
	void UpdateDeathExplosion();
	void DrawDeathExplosions();
	void SetupDebris();
	void AddDebrisForTexture(const std::shared_ptr<KdTexture>& tex, const Math::Vector2& drawSize, const Math::Vector2& originOffset, float baseAngle);
	void DrawDebris();

	std::shared_ptr<KdTexture> m_spTex = nullptr;
	std::shared_ptr<KdTexture> m_spBaseTex = nullptr;
	std::shared_ptr<KdTexture> m_spDeathExplosionTex = nullptr;
	std::vector<DeathExplosion> m_deathExplosions;
	std::vector<Debris> m_debris;

	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_size = { 256.0f, 256.0f };
	Math::Vector2 m_baseSize = { 300.0f, 300.0f };
	Math::Vector2 m_baseOffset = { 0.0f, -34.0f };

	float m_angle = 0.0f;
	float m_angleOffset = DirectX::XMConvertToRadians(-90.0f);
	float m_damageShakeFrame = 0.0f;
	float m_damageShakeDuration = 36.0f;
	float m_damageShakeAngle = 10.0f;
	float m_damageShakePower = 28.0f;
	float m_deathFrame = 0.0f;
	float m_deathExplosionDuration = 180.0f;
	float m_deathExplosionSpawnFrame = 0.0f;
	float m_deathExplosionSpawnInterval = 6.0f;
	float m_deathExplosionAnimFps = 20.0f;
	int m_deathExplosionFrameWidth = 64;
	int m_deathExplosionFrameHeight = 64;
	int m_deathExplosionFrameCount = 8;
	int m_deathExplosionRowIndex = 7;
	bool m_deathStarted = false;
	bool m_debrisStarted = false;
};
