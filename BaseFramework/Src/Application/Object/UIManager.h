#pragma once

class EnergySystem;
class HeatRay;
class PlayerPlanet;
class WeaponSystem;

class UIManager
{
public:
	void Init(
		const std::shared_ptr<PlayerPlanet>& playerPlanet,
		const std::shared_ptr<EnergySystem>& energySystem,
		const std::shared_ptr<WeaponSystem>& weaponSystem,
		const std::shared_ptr<HeatRay>& heatRay);
	void Update();
	void DrawSprite();
	void ShakeEnergyGauge();
	void SetStageNo(int stageNo);

private:
	Math::Vector2 GetMouseSpritePos() const;
	void DrawPlayerPlanetHp();
	void DrawEnergy();
	void DrawEnergyRing(const Math::Vector2& center, float energyRate);
	void DrawAmmoPreview();
	void DrawGameOver();
	void ShakePlayerPlanetHpGauge();
	void ShakeAmmoPreview();

	std::shared_ptr<KdTexture> m_spCrosshairTex = nullptr;
	std::shared_ptr<KdTexture> m_spPlanetHpTex = nullptr;
	std::shared_ptr<KdTexture> m_spEnergyTex = nullptr;
	std::shared_ptr<KdTexture> m_spGameOverTex = nullptr;
	std::shared_ptr<KdTexture> m_spAmmoFrameTex = nullptr;
	std::shared_ptr<KdTexture> m_spMeteorAmmoTex = nullptr;
	std::shared_ptr<KdTexture> m_spMimicPlanetAmmoTex = nullptr;
	std::shared_ptr<KdTexture> m_spHeatRayAmmoTex = nullptr;
	std::shared_ptr<KdTexture> m_spWeaponSwitchKeyTex = nullptr;
	std::weak_ptr<EnergySystem> m_wpEnergySystem;
	std::weak_ptr<PlayerPlanet> m_wpPlayerPlanet;
	std::weak_ptr<WeaponSystem> m_wpWeaponSystem;
	std::weak_ptr<HeatRay> m_wpHeatRay;

	float m_anime = 0.0f;
	int m_frameCount = 4;
	int m_frameWidth = 32;
	int m_frameHeight = 30;
	float m_animFps = 10.0f;
	Math::Vector2 m_drawSize = { 64.0f, 60.0f };

	int m_planetHpFrameWidth = 48;
	int m_planetHpFrameHeight = 11;
	Math::Vector2 m_planetHpDrawPos = { -610.0f, 325.0f };
	Math::Vector2 m_planetHpDrawSize = { 240.0f, 55.0f };
	int m_prevPlanetHp = -1;
	float m_planetHpShakeFrame = 0.0f;
	float m_planetHpShakeDuration = 36.0f;
	float m_planetHpShakePower = 34.0f;

	Math::Vector2 m_energyDrawPos = { 550.0f, -250.0f };
	Math::Vector2 m_energyDrawSize = { 142.0f, 142.0f };
	float m_energyRingRadius = 82.0f;
	int m_energyRingThickness = 5;
	float m_energyShakeFrame = 0.0f;
	float m_energyShakeDuration = 30.0f;
	float m_energyShakePower = 18.0f;
	int m_stageNo = 1;

	int m_ammoFrameWidth = 48;
	int m_ammoFrameHeight = 50;
	Math::Vector2 m_ammoDrawPos = { 120.0f, -205.0f };
	Math::Vector2 m_ammoDrawSize = { 96.0f, 100.0f };
	Math::Vector2 m_ammoIconDrawSize = { 58.0f, 58.0f };
	Math::Vector2 m_weaponSwitchKeyOffset = { 0.0f, -72.0f };
	Math::Vector2 m_weaponSwitchKeyDrawSize = { 72.0f, 48.0f };
	float m_ammoShakeFrame = 0.0f;
	float m_ammoShakeDuration = 36.0f;
	float m_ammoShakePower = 32.0f;
};
