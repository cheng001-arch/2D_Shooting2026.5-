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
	m_stage8SpecialSpawnCount = 0;
	m_isPlayerPlanetDead = false;
}

void EnemyManager::SetStageNo(int stageNo)
{
	m_stageNo = stageNo;
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
		else if (m_spawnTimer >= m_spawnInterval)
		{
			m_spawnTimer -= m_spawnInterval;
			SpawnEnemy();

			while (m_pendingCometCount > 0)
			{
				--m_pendingCometCount;
				SpawnComet();
			}
		}

		TrySpawnStage8SpecialEnemy();
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

	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();
	enemy->Init();
	SetupEnemyByType(enemy, static_cast<EnemyType>(typeDist(randEngine)));
	enemy->SetPos({ spawnXDist(randEngine), 430.0f });

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

	switch (type)
	{
	case EnemyType::Comet:
		enemy->SetTexture(m_spCometTex);
		enemy->SetStatus(
			3,
			0,
			70,
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
