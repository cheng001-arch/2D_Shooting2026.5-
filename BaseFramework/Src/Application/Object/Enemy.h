#pragma once

class Enemy : public KdGameObject
{
public:
	Enemy() {}
	~Enemy() override {}

	void Init() override;
	void Update() override;
	void Update(const Math::Vector2& planetCenter, float planetRadius);
	void DrawSprite() override;

	void SetPos(const Math::Vector2& pos);
	void SetPos(const Math::Vector3& pos) override;

	void SetTexture(const std::shared_ptr<KdTexture>& tex) { m_spTex = tex; }
	void SetGravityTrailTexture(const std::shared_ptr<KdTexture>& tex) { m_spGravityTrailTex = tex; }
	void SetStatus(float hp, int attackPower, int energyReward, float fallSpeed, const Math::Vector2& size, float radius);
	void SetCometPath(const Math::Vector2& start, const Math::Vector2& control, const Math::Vector2& end, float pathSpeed);
	void SetBlackHoleCaptured(bool captured) { m_isBlackHoleCaptured = captured; }
	void Damage(float attackPower);

	Math::Vector2 GetPos2D() const { return m_pos; }
	float GetRadius() const { return m_radius; }
	int GetAttackPower() const { return m_attackPower; }
	int GetEnergyReward() const { return m_energyReward; }
	bool IsComet() const { return m_isComet; }
	bool IsBlackHoleCaptured() const { return m_isBlackHoleCaptured; }

private:
	std::shared_ptr<KdTexture> m_spTex = nullptr;
	std::shared_ptr<KdTexture> m_spGravityTrailTex = nullptr;

	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_size = { 80.0f, 80.0f };
	Math::Vector2 m_moveDir = { 0.0f, -1.0f };
	Math::Vector2 m_cometStart = Math::Vector2::Zero;
	Math::Vector2 m_cometControl = Math::Vector2::Zero;
	Math::Vector2 m_cometEnd = Math::Vector2::Zero;

	float m_fallSpeed = 3.5f;
	float m_cometPathSpeed = 0.006f;
	float m_cometT = 0.0f;
	float m_angle = 0.0f;
	float m_radius = 32.0f;
	float m_hp = 1.0f;
	float m_gravityTrailFrame = 0.0f;
	float m_gravityTrailAnimFps = 20.0f;
	int m_gravityTrailFrameWidth = 64;
	int m_gravityTrailFrameHeight = 64;
	int m_gravityTrailFrameCount = 9;
	int m_gravityTrailRowIndex = 4;
	int m_attackPower = 1;
	int m_energyReward = 3;
	bool m_isComet = false;
	bool m_isBlackHoleCaptured = false;
	bool m_isGravityAttracted = false;
};
