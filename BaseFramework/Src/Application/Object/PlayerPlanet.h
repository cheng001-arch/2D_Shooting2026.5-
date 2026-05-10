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
	void ShakeDamage();

	std::shared_ptr<KdTexture> m_spTex = nullptr;

	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_size = { 2400.0f, 2400.0f };

	float m_damageShakeFrame = 0.0f;
	float m_damageShakeDuration = 36.0f;
	float m_damageShakeAngle = 6.0f;
	float m_damageShakePowerX = 35.0f;
	float m_damageShakePowerY = 14.0f;
	int m_hp = 20;
};
