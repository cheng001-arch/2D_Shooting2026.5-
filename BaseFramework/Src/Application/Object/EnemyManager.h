#pragma once

class Enemy;
class PlayerPlanet;
class ProgressSystem;

class EnemyManager
{
public:
	void Init();
	void Reset();
	void SetProgressSystem(const std::shared_ptr<ProgressSystem>& progressSystem);
	void Update(const PlayerPlanet& playerPlanet);
	void DrawSprite();

	std::vector<std::shared_ptr<Enemy>>& WorkEnemies() { return m_enemies; }
	void NotifyEnemyDefeated(const Enemy& enemy);
	void NotifyEnemyCrashedIntoPlanet(const Enemy& enemy);

private:
	enum class EnemyType
	{
		Small,
		Medium,
		Large,
		Comet
	};

	void SpawnEnemy();
	void SpawnComet();
	void SetupEnemyByType(const std::shared_ptr<Enemy>& enemy, EnemyType type);

	std::shared_ptr<KdTexture> m_spSmallMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spMediumMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spLargeMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spCometTex = nullptr;
	std::shared_ptr<KdTexture> m_spGravityTrailTex = nullptr;
	std::weak_ptr<ProgressSystem> m_wpProgressSystem;
	std::vector<std::shared_ptr<Enemy>> m_enemies;

	float m_spawnTimer = 0.0f;
	float m_spawnInterval = 120.0f;
	int m_defeatCountForComet = 0;
	int m_pendingCometCount = 0;
	bool m_isPlayerPlanetDead = false;

	float m_smallMeteorSpeed = 3.5f;
	float m_mediumMeteorSpeed = 3.0f;
	float m_largeMeteorSpeed = 2.5f;
	float m_cometPathSpeed = 0.006f;
};
