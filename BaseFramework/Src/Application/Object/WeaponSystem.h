#pragma once

class ProjectileManager;
class HeatRay;
class Turret;

class WeaponSystem
{
public:
	void Init(
		const std::shared_ptr<Turret>& turret,
		const std::shared_ptr<ProjectileManager>& projectileManager,
		const std::shared_ptr<HeatRay>& heatRay);
	void Update();

	enum class WeaponType
	{
		Meteor,
		MimicPlanet,
		HeatRay
	};

	WeaponType GetCurrentWeapon() const { return m_currentWeapon; }
	int GetMimicPlanetAmmo() const { return m_mimicPlanetAmmo; }
	int GetMimicPlanetMaxAmmo() const { return m_mimicPlanetMaxAmmo; }

private:
	Math::Vector2 GetMouseSpritePos() const;
	void UpdateMimicPlanetAmmo();
	void ChangeWeapon();

	std::weak_ptr<Turret> m_wpTurret;
	std::weak_ptr<ProjectileManager> m_wpProjectileManager;
	std::weak_ptr<HeatRay> m_wpHeatRay;

	WeaponType m_currentWeapon = WeaponType::Meteor;
	bool m_prevLeftClick = false;
	bool m_prevEKey = false;

	float m_meteorShootInterval = 18.0f;
	float m_meteorShootCoolFrame = 0.0f;

	int m_mimicPlanetAmmo = 3;
	int m_mimicPlanetMaxAmmo = 3;
	float m_mimicPlanetRecoveryFrame = 0.0f;
	float m_mimicPlanetRecoveryInterval = 180.0f;
};
