#pragma once

class Enemy;
class PlayerPlanet;
class ProgressSystem;

class EnemyManager
{
public:
	void Init();
	void Reset();
	void SetStageNo(int stageNo);
	void SetProgressSystem(const std::shared_ptr<ProgressSystem>& progressSystem);
	void Update(const PlayerPlanet& playerPlanet);
	void DrawSprite();

	std::vector<std::shared_ptr<Enemy>>& WorkEnemies() { return m_enemies; }
	void NotifyEnemyDefeated(const Enemy& enemy);
	void NotifyEnemyCrashedIntoPlanet(const Enemy& enemy);
	void SlowAllEnemies(const Enemy* sourceEnemy, float multiplier, float durationFrame);

private:
	enum class EnemyType
	{
		Small,
		Medium,
		Large,
		Comet,
		Stage3Crystal,
		Stage4Crystal,
		Stage8Special
	};

	void SpawnEnemy();
	void SpawnComet();
	void SpawnStage3CrystalEnemy();
	void TrySpawnStage3CrystalEnemy();
	void SpawnStage8SpecialEnemy();
	void TrySpawnStage8SpecialEnemy();
	void SetupEnemyByType(const std::shared_ptr<Enemy>& enemy, EnemyType type);
	int GetStageAttackBonus() const;
	float GetCurrentSpawnInterval() const;

	std::shared_ptr<KdTexture> m_spSmallMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spMediumMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spLargeMeteorTex = nullptr;
	std::shared_ptr<KdTexture> m_spCometTex = nullptr;
	std::shared_ptr<KdTexture> m_spStage3CrystalTex = nullptr;
	std::shared_ptr<KdTexture> m_spStage4CrystalTex = nullptr;
	std::shared_ptr<KdTexture> m_spStage8SpecialTex = nullptr;
	std::shared_ptr<KdTexture> m_spGravityTrailTex = nullptr;
	std::weak_ptr<ProgressSystem> m_wpProgressSystem;
	std::vector<std::shared_ptr<Enemy>> m_enemies;

	float m_spawnTimer = 0.0f;
	float m_spawnInterval = 120.0f;
	float m_stage1SpawnInterval = 45.0f;
	float m_stage2SpawnInterval = 60.0f;
	int m_defeatCountForComet = 0;
	int m_pendingCometCount = 0;
	int m_stageNo = 1;
	int m_stage3CrystalSpawnCount = 0;
	int m_stage8SpecialSpawnCount = 0;
	bool m_isPlayerPlanetDead = false;
	bool m_isSmallMeteorOnlyStage = true;
	bool m_isSmallAndMediumMeteorStage = false;
	bool m_isStage3CrystalStage = false;
	bool m_isStage4CrystalStage = false;
	bool m_isNoCometStage = true;
	bool m_isCometBonusStage = false;
	bool m_isStage8SpecialStage = false;

	float m_smallMeteorSpeed = 3.5f;
	float m_mediumMeteorSpeed = 3.0f;
	float m_largeMeteorSpeed = 2.5f;
	float m_stage3CrystalSpeed = 2.0f;
	float m_stage4CrystalSpeed = 0.7f;
	float m_stage8SpecialSpeed = 0.8f;
	float m_cometPathSpeed = 0.006f;
	float m_cometBonusSpawnInterval = 45.0f;
	int m_cometBonusProgress = 5;
};
