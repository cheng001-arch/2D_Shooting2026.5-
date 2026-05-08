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

	float GetAngle() const { return m_angle; }

private:
	Math::Vector2 GetMouseSpritePos() const;

	std::shared_ptr<KdTexture> m_spTex = nullptr;

	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_size = { 128.0f, 128.0f };

	float m_angle = 0.0f;
	float m_angleOffset = DirectX::XMConvertToRadians(-90.0f);
};
