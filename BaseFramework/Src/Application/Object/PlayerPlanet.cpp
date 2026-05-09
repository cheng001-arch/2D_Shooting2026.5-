#include "PlayerPlanet.h"

void PlayerPlanet::Init()
{
	m_drawType = eDrawTypeUI;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/diqiu.png");

	SetPos(m_pos);
}

void PlayerPlanet::DrawSprite()
{
	if (!m_spTex) { return; }

	Math::Matrix mat =
		Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(180.0f)) *
		Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTex.get(),
		0,
		0,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y));
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void PlayerPlanet::SetPos(const Math::Vector2& pos)
{
	m_pos = pos;
	m_mWorld = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
}

void PlayerPlanet::SetPos(const Math::Vector3& pos)
{
	SetPos({ pos.x, pos.y });
}

Math::Vector2 PlayerPlanet::GetTopPos() const
{
	return { m_pos.x, m_pos.y + m_size.y * 0.5f };
}
