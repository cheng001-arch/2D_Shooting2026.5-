#include "Turret.h"
#include "Application/main.h"

#include <algorithm>
#include <cmath>
#include <random>

void Turret::Init()
{
	m_drawType = eDrawTypeUI;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/paotai3.png");

	m_spBaseTex = std::make_shared<KdTexture>();
	m_spBaseTex->Load("Asset/Textures/dizu.png");

	m_spDeathExplosionTex = std::make_shared<KdTexture>();
	m_spDeathExplosionTex->Load("Asset/Textures/baozha4.png");

	SetPos(m_pos);
}

void Turret::Update()
{
	if (m_deathStarted) { return; }

	const Math::Vector2 mousePos = GetMouseSpritePos();
	const Math::Vector2 toMouse = mousePos - m_pos;

	if (toMouse.LengthSquared() <= 0.001f) { return; }

	m_angle = static_cast<float>(std::atan2(toMouse.y, toMouse.x)) + m_angleOffset;
}

void Turret::DrawSprite()
{
	if (!m_spTex || !m_spBaseTex) { return; }

	UpdateDeathExplosion();
	if (m_debrisStarted)
	{
		m_deathFrame += Application::Instance().GetDeltaTime();
		DrawDebris();
		return;
	}

	Math::Vector2 drawPos = m_pos;
	float shakeAngle = 0.0f;
	if (m_damageShakeFrame > 0.0f)
	{
		const float shakeRate = m_damageShakeFrame / m_damageShakeDuration;
		shakeAngle = std::sin(m_damageShakeFrame * 1.8f) * DirectX::XMConvertToRadians(m_damageShakeAngle) * shakeRate;
		drawPos.x += std::sin(m_damageShakeFrame * 3.1f) * m_damageShakePower * shakeRate;
		drawPos.y += std::cos(m_damageShakeFrame * 4.0f) * m_damageShakePower * 0.5f * shakeRate;
		m_damageShakeFrame = std::max(0.0f, m_damageShakeFrame - Application::Instance().GetDeltaTime());
	}

	Math::Matrix baseMat =
		Math::Matrix::CreateRotationZ(shakeAngle) *
		Math::Matrix::CreateTranslation(drawPos.x + m_baseOffset.x, drawPos.y + m_baseOffset.y, 0.0f);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(baseMat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spBaseTex.get(),
		0,
		0,
		static_cast<int>(m_baseSize.x),
		static_cast<int>(m_baseSize.y));

	Math::Matrix mat =
		Math::Matrix::CreateRotationZ(m_angle + shakeAngle) *
		Math::Matrix::CreateTranslation(drawPos.x, drawPos.y, 0.0f);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTex.get(),
		0,
		0,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y));
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);

	DrawDeathExplosions();
}

void Turret::SetPos(const Math::Vector2& pos)
{
	m_pos = pos;
	m_mWorld = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0.0f);
}

void Turret::SetPos(const Math::Vector3& pos)
{
	SetPos({ pos.x, pos.y });
}

Math::Vector2 Turret::GetAimDir() const
{
	const float aimAngle = m_angle - m_angleOffset;

	return {
		std::cos(aimAngle),
		std::sin(aimAngle)
	};
}

Math::Vector2 Turret::GetMuzzlePos() const
{
	return m_pos + GetAimDir() * (m_size.y * 0.45f);
}

void Turret::ShakeDamage()
{
	m_damageShakeFrame = m_damageShakeDuration;
}

void Turret::BeginDeathExplosion()
{
	if (m_deathStarted) { return; }

	m_deathStarted = true;
	m_debrisStarted = false;
	m_deathFrame = 0.0f;
	m_deathExplosionSpawnFrame = 0.0f;
	m_deathExplosions.clear();
	m_debris.clear();
}

void Turret::ResetDestruction()
{
	m_deathStarted = false;
	m_debrisStarted = false;
	m_deathFrame = 0.0f;
	m_deathExplosionSpawnFrame = 0.0f;
	m_deathExplosions.clear();
	m_debris.clear();
}

void Turret::UpdateDeathExplosion()
{
	if (!m_deathStarted || m_debrisStarted) { return; }

	const float dt = Application::Instance().GetDeltaTime();
	m_deathFrame += dt;
	m_deathExplosionSpawnFrame += dt;

	static std::mt19937 randEngine{ std::random_device{}() };
	static std::uniform_real_distribution<float> offsetDist(-95.0f, 95.0f);
	static std::uniform_real_distribution<float> sizeDist(90.0f, 190.0f);
	static std::uniform_real_distribution<float> frameDist(0.0f, 7.99f);

	while (m_deathExplosionSpawnFrame >= m_deathExplosionSpawnInterval && m_deathFrame <= m_deathExplosionDuration)
	{
		m_deathExplosionSpawnFrame -= m_deathExplosionSpawnInterval;

		DeathExplosion explosion;
		explosion.pos = { m_pos.x + offsetDist(randEngine), m_pos.y + offsetDist(randEngine) };
		const float size = sizeDist(randEngine);
		explosion.size = { size, size };
		explosion.frame = frameDist(randEngine);
		m_deathExplosions.push_back(explosion);
	}

	for (DeathExplosion& explosion : m_deathExplosions)
	{
		explosion.frame += (m_deathExplosionAnimFps / 60.0f) * dt;
		if (explosion.frame >= static_cast<float>(m_deathExplosionFrameCount))
		{
			explosion.frame = std::fmod(explosion.frame, static_cast<float>(m_deathExplosionFrameCount));
		}
	}

	if (m_deathFrame >= m_deathExplosionDuration)
	{
		SetupDebris();
		m_deathExplosions.clear();
		m_debrisStarted = true;
	}
}

void Turret::DrawDeathExplosions()
{
	if (!m_deathStarted || m_debrisStarted || !m_spDeathExplosionTex) { return; }

	for (const DeathExplosion& explosion : m_deathExplosions)
	{
		const int frame = std::clamp(static_cast<int>(explosion.frame), 0, m_deathExplosionFrameCount - 1);
		const Math::Rectangle srcRect = {
			frame * m_deathExplosionFrameWidth,
			m_deathExplosionRowIndex * m_deathExplosionFrameHeight,
			m_deathExplosionFrameWidth,
			m_deathExplosionFrameHeight
		};

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spDeathExplosionTex.get(),
			static_cast<int>(explosion.pos.x),
			static_cast<int>(explosion.pos.y),
			static_cast<int>(explosion.size.x),
			static_cast<int>(explosion.size.y),
			&srcRect);
	}
}

void Turret::SetupDebris()
{
	if (!m_spTex || !m_spBaseTex || !m_debris.empty()) { return; }

	AddDebrisForTexture(m_spBaseTex, m_baseSize, m_baseOffset, 0.0f);
	AddDebrisForTexture(m_spTex, m_size, Math::Vector2::Zero, m_angle);
}

void Turret::AddDebrisForTexture(const std::shared_ptr<KdTexture>& tex, const Math::Vector2& drawSize, const Math::Vector2& originOffset, float baseAngle)
{
	if (!tex) { return; }

	const int texW = static_cast<int>(tex->GetWidth());
	const int texH = static_cast<int>(tex->GetHeight());
	if (texW <= 0 || texH <= 0) { return; }

	const int cols = 3;
	const int rows = 3;
	const int pieceW = texW / cols;
	const int pieceH = texH / rows;
	const float scaleX = drawSize.x / static_cast<float>(texW);
	const float scaleY = drawSize.y / static_cast<float>(texH);

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			Debris debris;
			debris.tex = tex;
			debris.srcRect = { col * pieceW, row * pieceH, pieceW, pieceH };
			debris.drawSize = drawSize;
			debris.baseOffset = {
				originOffset.x + (col * pieceW + pieceW * 0.5f - texW * 0.5f) * scaleX,
				originOffset.y + (row * pieceH + pieceH * 0.5f - texH * 0.5f) * scaleY
			};
			debris.velocity = {
				(col - 1.0f) * 2.4f,
				(row - 1.0f) * 1.8f + 0.8f
			};
			debris.angle = DirectX::XMConvertToRadians(static_cast<float>((row * cols + col) * 23));
			debris.angularSpeed = DirectX::XMConvertToRadians(col % 2 == 0 ? -0.9f : 0.75f);
			debris.baseAngle = baseAngle;
			m_debris.push_back(debris);
		}
	}
}

void Turret::DrawDebris()
{
	const float breakFrame = std::max(0.0f, m_deathFrame - m_deathExplosionDuration);

	for (const Debris& debris : m_debris)
	{
		if (!debris.tex) { continue; }

		const int texW = static_cast<int>(debris.tex->GetWidth());
		const int texH = static_cast<int>(debris.tex->GetHeight());
		if (texW <= 0 || texH <= 0) { continue; }

		const float scaleX = debris.drawSize.x / static_cast<float>(texW);
		const float scaleY = debris.drawSize.y / static_cast<float>(texH);
		const Math::Vector2 drawPos = m_pos + debris.baseOffset + debris.velocity * breakFrame;
		const float angle = debris.baseAngle + debris.angle + debris.angularSpeed * breakFrame;

		Math::Matrix mat =
			Math::Matrix::CreateRotationZ(angle) *
			Math::Matrix::CreateTranslation(drawPos.x, drawPos.y, 0.0f);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			debris.tex.get(),
			0,
			0,
			static_cast<int>(debris.srcRect.width * scaleX),
			static_cast<int>(debris.srcRect.height * scaleY),
			&debris.srcRect);
	}

	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

Math::Vector2 Turret::GetMouseSpritePos() const
{
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	RECT clientRect{};
	GetClientRect(Application::Instance().GetWindowHandle(), &clientRect);

	const float clientW = static_cast<float>(clientRect.right - clientRect.left);
	const float clientH = static_cast<float>(clientRect.bottom - clientRect.top);

	return {
		static_cast<float>(mousePos.x) - clientW * 0.5f,
		clientH * 0.5f - static_cast<float>(mousePos.y)
	};
}
