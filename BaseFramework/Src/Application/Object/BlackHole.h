#pragma once

#include "Application/Object/ProjectileManager.h"

#include <unordered_map>

class EnergySystem;
class EnemyManager;
class UIManager;

class BlackHole
{
public:
	void Init(
		const std::shared_ptr<EnergySystem>& energySystem,
		const std::shared_ptr<UIManager>& uiManager,
		const std::shared_ptr<EnemyManager>& enemyManager,
		const std::shared_ptr<ProjectileManager>& projectileManager);
	void Reset();
	void SetStageNo(int stageNo);
	void Update();
	void DrawSprite();

	bool IsActive() const { return m_isActive; }

private:
	Math::Vector2 GetMouseSpritePos() const;
	void CreateBlackHole(const Math::Vector2& pos);
	void PullEnemies();
	void PullProjectiles();
	void ReleaseCapturedEnemies();

	std::weak_ptr<EnergySystem> m_wpEnergySystem;
	std::weak_ptr<UIManager> m_wpUIManager;
	std::weak_ptr<EnemyManager> m_wpEnemyManager;
	std::weak_ptr<ProjectileManager> m_wpProjectileManager;
	std::shared_ptr<KdTexture> m_spTex = nullptr;
	std::unordered_map<class Enemy*, float> m_enemyCenterTimers;
	std::unordered_map<ProjectileManager::Projectile*, float> m_projectileCenterTimers;

	Math::Vector2 m_pos = { 0.0f, 180.0f };
	Math::Vector2 m_drawSize = { 480.0f, 480.0f };
	float m_lifeFrame = 0.0f;
	float m_lifeLimitFrame = 240.0f;
	float m_needEnergy = 100.0f;
	float m_pullSpeed = 10.0f;
	float m_centerRadius = 32.0f;
	float m_absorbDelayFrame = 60.0f;
	float m_anime = 0.0f;
	float m_animFps = 1.0f / 0.05f;
	int m_frameWidth = 229;
	int m_frameHeight = 240;
	int m_frameColumns = 6;
	int m_frameCount = 24;
	bool m_isActive = false;
	bool m_prevRKey = false;
	bool m_isLocked = false;
};
