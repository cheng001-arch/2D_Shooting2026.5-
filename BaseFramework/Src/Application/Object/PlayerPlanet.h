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

	Math::Vector2 GetSpritePos() const { return m_pos; }
	Math::Vector2 GetSize() const { return m_size; }
	Math::Vector2 GetTopPos() const;

private:
	std::shared_ptr<KdTexture> m_spTex = nullptr;

	Math::Vector2 m_pos = Math::Vector2::Zero;
	Math::Vector2 m_size = { 2400.0f, 2400.0f };
};
