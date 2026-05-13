#pragma once

#include <unordered_map>

class EnemyManager;
class EnergySystem;
class ExplosionManager;
class Turret;

class HeatRay
{
public:
	void Init(
		const std::shared_ptr<Turret>& turret,
		const std::shared_ptr<EnemyManager>& enemyManager,
		const std::shared_ptr<EnergySystem>& energySystem,
		const std::shared_ptr<ExplosionManager>& explosionManager);
	void Reset();
	void Update(bool wantsFire);
	void DrawSprite();

	float GetHeat() const { return m_heat; }
	float GetMaxHeat() const { return m_maxHeat; }
	bool IsOverheated() const { return m_isOverheated; }
	bool IsFiring() const { return m_isFiring; }

private:
	struct HitEffect
	{
		Math::Vector2 pos = Math::Vector2::Zero;
		float frame = 0.0f;
	};

	void UpdateHeat(bool canFire);
	void HitEnemies();
	void TriggerStage3CrystalBurst(class Enemy& crystalEnemy, EnemyManager& enemyManager, EnergySystem& energySystem, ExplosionManager& explosionManager);
	void SpawnHitEffect(const Math::Vector2& pos);
	void UpdateHitEffects();
	void DrawHitEffects();
	void DrawChargeEffect();
	bool IsEnemyOnRay(const Math::Vector2& enemyPos, float enemyRadius) const;

	std::shared_ptr<KdTexture> m_spTex = nullptr;
	std::shared_ptr<KdTexture> m_spHitEffectTex = nullptr;
	std::weak_ptr<Turret> m_wpTurret;
	std::weak_ptr<EnemyManager> m_wpEnemyManager;
	std::weak_ptr<EnergySystem> m_wpEnergySystem;
	std::weak_ptr<ExplosionManager> m_wpExplosionManager;
	std::unordered_map<class Enemy*, float> m_damageTimers;
	std::vector<HitEffect> m_hitEffects;

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
	float m_chargeSpinFrame = 0.0f;
	float m_chargeSpinSpeed = 0.18f;
	float m_chargeStartRadius = 74.0f;
	float m_chargeEndRadius = 18.0f;
	float m_chargeParticleSize = 34.0f;
	float m_damageInterval = 6.0f;
	float m_damagePower = 0.45f;
	float m_hitEffectAnimFps = 25.0f;
	float m_hitEffectDrawSize = 96.0f;
	int m_hitEffectFrameWidth = 64;
	int m_hitEffectFrameHeight = 64;
	int m_hitEffectFrameCount = 18;
	int m_hitEffectRowIndex = 7;

	bool m_isFiring = false;
	bool m_isOverheated = false;
	bool m_isDissolving = false;
};
