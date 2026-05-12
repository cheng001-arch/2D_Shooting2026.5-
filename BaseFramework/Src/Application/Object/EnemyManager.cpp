#include "EnemyManager.h"
#include "Application/main.h"
#include "Application/Object/Enemy.h"
#include "Application/Object/PlayerPlanet.h"
#include "Application/Object/ProgressSystem.h"

#include <algorithm>
#include <random>

void EnemyManager::Init()
{
	m_spSmallMeteorTex = std::make_shared<KdTexture>();
	m_spSmallMeteorTex->Load("Asset/Textures/yunshi2.png");

	m_spMediumMeteorTex = std::make_shared<KdTexture>();
	m_spMediumMeteorTex->Load("Asset/Textures/yunshi1.png");

	m_spLargeMeteorTex = std::make_shared<KdTexture>();
	m_spLargeMeteorTex->Load("Asset/Textures/yunshi2.png");

	m_spCometTex = std::make_shared<KdTexture>();
	m_spCometTex->Load("Asset/Textures/huixing.png");

	m_spStage3CrystalTex = std::make_shared<KdTexture>();
	m_spStage3CrystalTex->Load("Asset/Textures/zishuijingxingqiu2.png");

	m_spStage4CrystalTex = std::make_shared<KdTexture>();
	m_spStage4CrystalTex->Load("Asset/Textures/zishuijingxingqiu.png");

	m_spStage8SpecialTex = std::make_shared<KdTexture>();
	m_spStage8SpecialTex->Load("Asset/Textures/lansexingqiu.png");

	m_spGravityTrailTex = std::make_shared<KdTexture>();
	m_spGravityTrailTex->Load("Asset/Textures/weiba.png");
}

void EnemyManager::Reset()
{
	m_enemies.clear();
	m_spawnTimer = 0.0f;
	m_defeatCountForComet = 0;
	m_pendingCometCount = 0;
	m_stage3CrystalSpawnCount = 0;
	m_stage8SpecialSpawnCount = 0;
	m_isPlayerPlanetDead = false;
}

void EnemyManager::SetStageNo(int stageNo)
{
	m_stageNo = stageNo;
	m_isSmallMeteorOnlyStage = m_stageNo == 1;
	m_isSmallAndMediumMeteorStage = m_stageNo == 2;
	m_isStage3CrystalStage = m_stageNo == 3;
	m_isStage4CrystalStage = m_stageNo == 4;
	m_isNoCometStage = m_stageNo <= 3;
	m_isCometBonusStage = m_stageNo == 5;
	m_isStage8SpecialStage = m_stageNo == 8;
}

void EnemyManager::SetProgressSystem(const std::shared_ptr<ProgressSystem>& progressSystem)
{
	m_wpProgressSystem = progressSystem;
}

void EnemyManager::Update(const PlayerPlanet& playerPlanet)
{
	m_isPlayerPlanetDead = playerPlanet.GetHp() <= 0;

	const std::shared_ptr<ProgressSystem> progressSystem = m_wpProgressSystem.lock();
	const bool canSpawnEnemy = !m_isPlayerPlanetDead && (!progressSystem || !progressSystem->IsComplete());
	if (canSpawnEnemy)
	{
		m_spawnTimer += Application::Instance().GetDeltaTime();

		if (m_isCometBonusStage)
		{
			while (m_spawnTimer >= m_cometBonusSpawnInterval)
			{
				m_spawnTimer -= m_cometBonusSpawnInterval;
				SpawnComet();
			}
		}
		else if (m_spawnTimer >= GetCurrentSpawnInterval())
		{
			m_spawnTimer -= GetCurrentSpawnInterval();
			SpawnEnemy();

			while (!m_isNoCometStage && m_pendingCometCount > 0)
			{
				--m_pendingCometCount;
				SpawnComet();
			}
		}

		TrySpawnStage8SpecialEnemy();
		TrySpawnStage3CrystalEnemy();
	}
	else
	{
		m_pendingCometCount = 0;
	}

	for (const std::shared_ptr<Enemy>& enemy : m_enemies)
	{
		if (enemy)
		{
			enemy->Update(playerPlanet.GetSpritePos(), playerPlanet.GetRadius());
		}
	}

	m_enemies.erase(
		std::remove_if(
			m_enemies.begin(),
			m_enemies.end(),
			[](const std::shared_ptr<Enemy>& enemy)
			{
				return !enemy || enemy->IsExpired();
			}),
		m_enemies.end());
}

void EnemyManager::NotifyEnemyDefeated(const Enemy& enemy)
{
	if (!m_isPlayerPlanetDead)
	{
		if (const std::shared_ptr<ProgressSystem> progressSystem = m_wpProgressSystem.lock())
		{
			if (enemy.IsComet() && m_isCometBonusStage)
			{
				progressSystem->AddEnemyProgress(m_cometBonusProgress);
			}
			else if (!enemy.IsComet())
			{
				progressSystem->AddEnemyProgress(enemy.GetEnergyReward());
			}
		}
	}

	if (enemy.IsComet()) { return; }
	if (m_isNoCometStage) { return; }

	++m_defeatCountForComet;
	if (m_defeatCountForComet >= 5)
	{
		m_defeatCountForComet = 0;
		++m_pendingCometCount;
	}
}

void EnemyManager::NotifyEnemyCrashedIntoPlanet(const Enemy& enemy)
{
	if (enemy.IsComet()) { return; }
	if (m_isPlayerPlanetDead) { return; }

	if (const std::shared_ptr<ProgressSystem> progressSystem = m_wpProgressSystem.lock())
	{
		progressSystem->AddEnemyProgress(enemy.GetEnergyReward());
	}
}

void EnemyManager::DrawSprite()
{
	for (const std::shared_ptr<Enemy>& enemy : m_enemies)
	{
		if (enemy)
		{
			enemy->DrawSprite();
		}
	}
}

void EnemyManager::SpawnEnemy()
{
	static std::mt19937 randEngine{ std::random_device{}() };
	static std::uniform_real_distribution<float> spawnXDist(-600.0f, 600.0f);
	static std::uniform_int_distribution<int> typeDist(0, 2);
	static std::uniform_int_distribution<int> stage2TypeDist(0, 1);
	static std::uniform_int_distribution<int> stage4TypeDist(0, 3);

	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();
	enemy->Init();
	EnemyType type = static_cast<EnemyType>(typeDist(randEngine));
	if (m_isSmallMeteorOnlyStage)
	{
		type = EnemyType::Small;
	}
	else if (m_isSmallAndMediumMeteorStage)
	{
		type = stage2TypeDist(randEngine) == 0 ? EnemyType::Small : EnemyType::Medium;
	}
	else if (m_isStage3CrystalStage)
	{
		type = static_cast<EnemyType>(typeDist(randEngine));
	}
	else if (m_isStage4CrystalStage)
	{
		const std::shared_ptr<ProgressSystem> progressSystem = m_wpProgressSystem.lock();
		if (progressSystem && progressSystem->GetProgress() > 50)
		{
			const int stage4Type = stage4TypeDist(randEngine);
			type = stage4Type == 3 ? EnemyType::Stage4Crystal : static_cast<EnemyType>(stage4Type);
		}
	}
	SetupEnemyByType(enemy, type);
	enemy->SetPos({ spawnXDist(randEngine), 430.0f });

	m_enemies.push_back(enemy);
}

void EnemyManager::SpawnStage3CrystalEnemy()
{
	static std::mt19937 randEngine{ std::random_device{}() };
	static std::uniform_real_distribution<float> spawnXDist(-480.0f, 480.0f);

	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();
	enemy->Init();
	SetupEnemyByType(enemy, EnemyType::Stage3Crystal);
	enemy->SetPos({ spawnXDist(randEngine), 470.0f });

	m_enemies.push_back(enemy);
}

void EnemyManager::SpawnComet()
{
	static std::mt19937 randEngine{ std::random_device{}() };
	static std::uniform_int_distribution<int> sideDist(0, 1);
	static std::uniform_real_distribution<float> startYDist(100.0f, 340.0f);
	static std::uniform_real_distribution<float> endYDist(-260.0f, 260.0f);
	static std::uniform_real_distribution<float> controlYDist(-340.0f, 340.0f);

	const bool fromLeft = sideDist(randEngine) == 0;
	const float startX = fromLeft ? -760.0f : 760.0f;
	const float endX = fromLeft ? 760.0f : -760.0f;
	const float controlX = 0.0f;

	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();
	enemy->Init();
	SetupEnemyByType(enemy, EnemyType::Comet);
	enemy->SetCometPath(
		{ startX, startYDist(randEngine) },
		{ controlX, controlYDist(randEngine) },
		{ endX, endYDist(randEngine) },
		m_cometPathSpeed);

	m_enemies.push_back(enemy);
}

void EnemyManager::SpawnStage8SpecialEnemy()
{
	static std::mt19937 randEngine{ std::random_device{}() };
	static std::uniform_real_distribution<float> spawnXDist(-480.0f, 480.0f);

	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();
	enemy->Init();
	SetupEnemyByType(enemy, EnemyType::Stage8Special);
	enemy->SetPos({ spawnXDist(randEngine), 470.0f });

	m_enemies.push_back(enemy);
}

void EnemyManager::TrySpawnStage3CrystalEnemy()
{
	if (!m_isStage3CrystalStage || m_stage3CrystalSpawnCount >= 3) { return; }

	const std::shared_ptr<ProgressSystem> progressSystem = m_wpProgressSystem.lock();
	if (!progressSystem) { return; }

	const int spawnThresholds[3] = { 25, 50, 75 };
	while (m_stage3CrystalSpawnCount < 3 &&
		progressSystem->GetProgress() >= spawnThresholds[m_stage3CrystalSpawnCount])
	{
		SpawnStage3CrystalEnemy();
		++m_stage3CrystalSpawnCount;
	}
}

void EnemyManager::TrySpawnStage8SpecialEnemy()
{
	if (!m_isStage8SpecialStage || m_stage8SpecialSpawnCount >= 2) { return; }

	const std::shared_ptr<ProgressSystem> progressSystem = m_wpProgressSystem.lock();
	if (!progressSystem) { return; }

	const int spawnThresholds[2] = { 30, 70 };
	while (m_stage8SpecialSpawnCount < 2 &&
		progressSystem->GetProgress() >= spawnThresholds[m_stage8SpecialSpawnCount])
	{
		SpawnStage8SpecialEnemy();
		++m_stage8SpecialSpawnCount;
	}
}

void EnemyManager::SetupEnemyByType(const std::shared_ptr<Enemy>& enemy, EnemyType type)
{
	if (!enemy) { return; }

	const int attackBonus = GetStageAttackBonus();
	enemy->SetStage3Crystal(type == EnemyType::Stage3Crystal);
	enemy->SetStage4Crystal(type == EnemyType::Stage4Crystal);

	switch (type)
	{
	case EnemyType::Comet:
		enemy->SetTexture(m_spCometTex);
		enemy->SetStatus(
			3,
			0,
			m_isCometBonusStage ? 20 : 70,
			0.0f,
			{ 120.0f, 120.0f },
			48.0f);
		break;
	case EnemyType::Medium:
		enemy->SetTexture(m_spMediumMeteorTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			2,
			2 + attackBonus,
			2,
			m_mediumMeteorSpeed,
			{ 120.0f, 120.0f },
			48.0f);
		break;
	case EnemyType::Large:
		enemy->SetTexture(m_spLargeMeteorTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			8,
			4 + attackBonus,
			4,
			m_largeMeteorSpeed,
			{ 320.0f, 320.0f },
			96.0f);
		break;
	case EnemyType::Stage3Crystal:
		enemy->SetTexture(m_spStage3CrystalTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			10,
			4 + attackBonus,
			4,
			m_stage3CrystalSpeed,
			{ 360.0f, 360.0f },
			108.0f);
		break;
	case EnemyType::Stage4Crystal:
		enemy->SetTexture(m_spStage4CrystalTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			15,
			8 + attackBonus,
			4,
			m_stage4CrystalSpeed,
			{ 380.0f, 380.0f },
			114.0f);
		break;
	case EnemyType::Stage8Special:
		enemy->SetTexture(m_spStage8SpecialTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			12,
			4 + attackBonus,
			8,
			m_stage8SpecialSpeed,
			{ 420.0f, 420.0f },
			130.0f);
		break;
	case EnemyType::Small:
	default:
		enemy->SetTexture(m_spSmallMeteorTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			1,
			1 + attackBonus,
			1,
			m_smallMeteorSpeed,
			{ 80.0f, 80.0f },
			32.0f);
		break;
	}
}

int EnemyManager::GetStageAttackBonus() const
{
	return std::max(0, m_stageNo - 1);
}

float EnemyManager::GetCurrentSpawnInterval() const
{
	if (m_isSmallMeteorOnlyStage) { return m_stage1SpawnInterval; }
	if (m_isSmallAndMediumMeteorStage) { return m_stage2SpawnInterval; }
	return m_spawnInterval;
}
