#pragma once

#include "Bullet.h"

class Player
{
public:
	void Init();
	void Update();
	void DrawSprite() const;

private:
	void Move();
	void Shot();

	Math::Vector2 m_pos = Math::Vector2(640.0f, 360.0f);
	float m_angleDeg = 0.0f;
	float m_moveSpeed = 5.0f;
	float m_turnSpeedDeg = 3.0f;

	std::vector<Bullet> m_bullets;
	std::shared_ptr<KdTexture> m_spTex;
};