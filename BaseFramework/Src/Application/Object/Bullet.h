#pragma once

class Bullet
{
public:
	void Init(const Math::Vector2& startPos, float angleDeg);
	void Update();
	void DrawSprite() const;

	bool IsExpired() const { return m_isExpired; }

private:
	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_dir = Math::Vector2::UnitY;
	float m_angleDeg = 0.0f;
	float m_speed = 10.0f;
	bool m_isExpired = false;

	std::shared_ptr<KdTexture> m_spTex;
};