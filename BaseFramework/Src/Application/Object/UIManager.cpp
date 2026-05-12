#include "UIManager.h"
#include "Application/main.h"
#include "Application/Object/EnergySystem.h"
#include "Application/Object/HeatRay.h"
#include "Application/Object/PlayerPlanet.h"
#include "Application/Object/WeaponSystem.h"

void UIManager::Init(
	const std::shared_ptr<PlayerPlanet>& playerPlanet,
	const std::shared_ptr<EnergySystem>& energySystem,
	const std::shared_ptr<WeaponSystem>& weaponSystem,
	const std::shared_ptr<HeatRay>& heatRay)
{
	m_wpPlayerPlanet = playerPlanet;
	m_wpEnergySystem = energySystem;
	m_wpWeaponSystem = weaponSystem;
	m_wpHeatRay = heatRay;

	m_spCrosshairTex = std::make_shared<KdTexture>();
	m_spCrosshairTex->Load("Asset/Textures/miaozhun.png");

	m_spPlanetHpTex = std::make_shared<KdTexture>();
	m_spPlanetHpTex->Load("Asset/Textures/diqiuxueliang.png");

	m_spEnergyTex = std::make_shared<KdTexture>();
	m_spEnergyTex->Load("Asset/Textures/yuzhounengliang.png");

	m_spGameOverTex = std::make_shared<KdTexture>();
	m_spGameOverTex->Load("Asset/Textures/gameover1.png");

	m_spAmmoFrameTex = std::make_shared<KdTexture>();
	m_spAmmoFrameTex->Load("Asset/Textures/danyao.png");

	m_spMeteorAmmoTex = std::make_shared<KdTexture>();
	m_spMeteorAmmoTex->Load("Asset/Textures/yunshi2.png");

	m_spMimicPlanetAmmoTex = std::make_shared<KdTexture>();
	m_spMimicPlanetAmmoTex->Load("Asset/Textures/nitaixingqiu.png");

	m_spHeatRayAmmoTex = std::make_shared<KdTexture>();
	m_spHeatRayAmmoTex->Load("Asset/Textures/lizi.png");

	if (playerPlanet)
	{
		m_prevPlanetHp = playerPlanet->GetHp();
	}
}

void UIManager::Update()
{
	m_anime += (m_animFps / 60.0f) * Application::Instance().GetDeltaTime();
	m_planetHpShakeFrame = std::max(0.0f, m_planetHpShakeFrame - Application::Instance().GetDeltaTime());
	m_ammoShakeFrame = std::max(0.0f, m_ammoShakeFrame - Application::Instance().GetDeltaTime());
	m_energyShakeFrame = std::max(0.0f, m_energyShakeFrame - Application::Instance().GetDeltaTime());

	const std::shared_ptr<PlayerPlanet> playerPlanet = m_wpPlayerPlanet.lock();
	if (playerPlanet)
	{
		const int hp = playerPlanet->GetHp();
		if (m_prevPlanetHp >= 0 && hp < m_prevPlanetHp)
		{
			ShakePlayerPlanetHpGauge();
			ShakeAmmoPreview();
		}
		m_prevPlanetHp = hp;
	}

	if (m_anime >= static_cast<float>(m_frameCount))
	{
		m_anime = 0.0f;
	}
}

void UIManager::ShakeEnergyGauge()
{
	m_energyShakeFrame = m_energyShakeDuration;
}

void UIManager::ShakePlayerPlanetHpGauge()
{
	m_planetHpShakeFrame = m_planetHpShakeDuration;
}

void UIManager::ShakeAmmoPreview()
{
	m_ammoShakeFrame = m_ammoShakeDuration;
}

void UIManager::DrawSprite()
{
	DrawPlayerPlanetHp();
	DrawEnergy();
	DrawAmmoPreview();

	if (!m_spCrosshairTex) { return; }

	const int frame = static_cast<int>(m_anime) % m_frameCount;
	const Math::Rectangle srcRect = {
		frame * m_frameWidth,
		0,
		m_frameWidth,
		m_frameHeight
	};
	const Math::Vector2 mousePos = GetMouseSpritePos();

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spCrosshairTex.get(),
		static_cast<int>(mousePos.x),
		static_cast<int>(mousePos.y),
		static_cast<int>(m_drawSize.x),
		static_cast<int>(m_drawSize.y),
		&srcRect);

	DrawGameOver();
}

void UIManager::DrawAmmoPreview()
{
	const std::shared_ptr<WeaponSystem> weaponSystem = m_wpWeaponSystem.lock();
	if (!m_spAmmoFrameTex || !m_spMeteorAmmoTex || !m_spMimicPlanetAmmoTex || !weaponSystem) { return; }

	int frame = 0;
	std::shared_ptr<KdTexture> iconTex = m_spMeteorAmmoTex;

	if (weaponSystem->GetCurrentWeapon() == WeaponSystem::WeaponType::MimicPlanet)
	{
		const int ammo = weaponSystem->GetMimicPlanetAmmo();
		iconTex = m_spMimicPlanetAmmoTex;
		const int maxAmmo = std::max(1, weaponSystem->GetMimicPlanetMaxAmmo());
		frame = std::clamp(maxAmmo - ammo, 0, 4);
	}
	else if (weaponSystem->GetCurrentWeapon() == WeaponSystem::WeaponType::HeatRay)
	{
		iconTex = m_spHeatRayAmmoTex;

		const std::shared_ptr<HeatRay> heatRay = m_wpHeatRay.lock();
		if (heatRay)
		{
			const float heatRate = std::clamp(heatRay->GetHeat() / heatRay->GetMaxHeat(), 0.0f, 1.0f);
			frame = static_cast<int>(heatRate * 4.0f);
		}
	}

	const Math::Rectangle srcRect = {
		frame * m_ammoFrameWidth,
		0,
		m_ammoFrameWidth,
		m_ammoFrameHeight
	};

	Math::Vector2 drawPos = m_ammoDrawPos;
	if (m_ammoShakeFrame > 0.0f)
	{
		const float shakeRate = m_ammoShakeFrame / m_ammoShakeDuration;
		drawPos.x += std::sin(m_ammoShakeFrame * 3.8f) * m_ammoShakePower * shakeRate;
		drawPos.y += std::cos(m_ammoShakeFrame * 4.7f) * m_ammoShakePower * 0.55f * shakeRate;
	}

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spAmmoFrameTex.get(),
		static_cast<int>(drawPos.x),
		static_cast<int>(drawPos.y),
		static_cast<int>(m_ammoDrawSize.x),
		static_cast<int>(m_ammoDrawSize.y),
		&srcRect);

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		iconTex.get(),
		static_cast<int>(drawPos.x),
		static_cast<int>(drawPos.y),
		static_cast<int>(m_ammoIconDrawSize.x),
		static_cast<int>(m_ammoIconDrawSize.y));
}

void UIManager::DrawEnergy()
{
	const std::shared_ptr<EnergySystem> energySystem = m_wpEnergySystem.lock();
	if (!m_spEnergyTex || !energySystem) { return; }

	const float energy = energySystem->GetEnergy();
	const float energyRate = std::clamp(energy / energySystem->GetMaxEnergy(), 0.0f, 1.0f);
	const int drawHeight = static_cast<int>(m_energyDrawSize.y * energyRate);
	const int srcHeight = static_cast<int>(m_energyFrameHeight * energyRate);

	if (drawHeight <= 0 || srcHeight <= 0) { return; }

	const Math::Rectangle srcRect = {
		0,
		m_energyFrameHeight - srcHeight,
		m_energyFrameWidth,
		srcHeight
	};

	Math::Vector2 drawPos = m_energyDrawPos;
	if (m_energyShakeFrame > 0.0f)
	{
		const float shakeRate = m_energyShakeFrame / m_energyShakeDuration;
		drawPos.x += std::sin(m_energyShakeFrame * 2.7f) * m_energyShakePower * shakeRate;
		drawPos.y += std::cos(m_energyShakeFrame * 3.6f) * m_energyShakePower * 0.6f * shakeRate;
	}

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spEnergyTex.get(),
		static_cast<int>(drawPos.x),
		static_cast<int>(drawPos.y),
		static_cast<int>(m_energyDrawSize.x),
		drawHeight,
		&srcRect,
		&kWhiteColor,
		{ 0.0f, 0.0f });
}

void UIManager::DrawPlayerPlanetHp()
{
	const std::shared_ptr<PlayerPlanet> playerPlanet = m_wpPlayerPlanet.lock();
	if (!m_spPlanetHpTex || !playerPlanet) { return; }

	const int hp = playerPlanet->GetHp();
	int frame = 0;

	if (hp <= 0)
	{
		frame = 4;
	}
	else if (hp <= 5)
	{
		frame = 3;
	}
	else if (hp <= 10)
	{
		frame = 2;
	}
	else if (hp <= 15)
	{
		frame = 1;
	}

	const Math::Rectangle srcRect = {
		frame * m_planetHpFrameWidth,
		0,
		m_planetHpFrameWidth,
		m_planetHpFrameHeight
	};

	Math::Vector2 drawPos = m_planetHpDrawPos;
	if (m_planetHpShakeFrame > 0.0f)
	{
		const float shakeRate = m_planetHpShakeFrame / m_planetHpShakeDuration;
		drawPos.x += std::sin(m_planetHpShakeFrame * 3.4f) * m_planetHpShakePower * shakeRate;
		drawPos.y += std::cos(m_planetHpShakeFrame * 4.2f) * m_planetHpShakePower * 0.45f * shakeRate;
	}

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spPlanetHpTex.get(),
		static_cast<int>(drawPos.x),
		static_cast<int>(drawPos.y),
		static_cast<int>(m_planetHpDrawSize.x),
		static_cast<int>(m_planetHpDrawSize.y),
		&srcRect,
		&kWhiteColor,
		{ 0.0f, 0.5f });
}

void UIManager::DrawGameOver()
{
	const std::shared_ptr<PlayerPlanet> playerPlanet = m_wpPlayerPlanet.lock();
	if (!m_spGameOverTex || !playerPlanet || playerPlanet->GetHp() > 0) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spGameOverTex.get(),
		0,
		0,
		720,
		720);
}

Math::Vector2 UIManager::GetMouseSpritePos() const
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
