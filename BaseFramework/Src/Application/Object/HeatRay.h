#pragma once

#include <unordered_map>

class EnemyManager;
class EnergySystem;
class Turret;

class HeatRay
{
public:
	void Init(
		const std::shared_ptr<Turret>& turret,
		const std::shared_ptr<EnemyManager>& enemyManager,
		const std::shared_ptr<EnergySystem>& energySystem);
	void Reset();
	void Update(bool wantsFire);
	void DrawSprite();

	float GetHeat() const { return m_heat; }
	float GetMaxHeat() const { return m_maxHeat; }
	bool IsOverheated() const { return m_isOverheated; }
	bool IsFiring() const { return m_isFiring; }

private:
	void UpdateHeat(bool canFire);
	void HitEnemies();
	bool IsEnemyOnRay(const Math::Vector2& enemyPos, float enemyRadius) const;

	std::shared_ptr<KdTexture> m_spTex = nullptr;
	std::weak_ptr<Turret> m_wpTurret;
	std::weak_ptr<EnemyManager> m_wpEnemyManager;
	std::weak_ptr<EnergySystem> m_wpEnergySystem;
	std::unordered_map<class Enemy*, float> m_damageTimers;

	Math::Vector2 m_muzzlePos = Math::Vector2::Zero;
	Math::Vector2 m_direction = { 0.0f, 1.0f };

	float m_heat = 0.0f;
	float m_maxHeat = 100.0f;
	float m_fireChargeFrame = 0.0f;
	float m_fireChargeInterval = 30.0f;
	float m_heatUpPerFrame = 100.0f / 300.0f;
	float m_coolDownPerFrame = 100.0f / 120.0f;
	float m_overheatCoolFrame = 0.0f;
	float m_overheatCoolInterval = 240.0f;

	float m_rayLength = 1100.0f;
	float m_currentRayLength = 0.0f;
	float m_dissolveStartLength = 0.0f;
	float m_rayExtendSpeed = 36.0f;
	float m_rayDissolveSpeed = 48.0f;
	float m_rayWidth = 42.0f;
	float m_particleSize = 48.0f;
	float m_particleSpacing = 36.0f;
	float m_particleScroll = 0.0f;
	float m_particleScrollSpeed = 42.0f;
	float m_damageInterval = 6.0f;
	float m_damagePower = 0.45f;

	bool m_isFiring = false;
	bool m_isOverheated = false;
	bool m_isDissolving = false;
};
