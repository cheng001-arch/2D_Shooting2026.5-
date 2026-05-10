#include "EnemyManager.h"
#include "Application/main.h"
#include "Application/Object/Enemy.h"
#include "Application/Object/PlayerPlanet.h"

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

	m_spGravityTrailTex = std::make_shared<KdTexture>();
	m_spGravityTrailTex->Load("Asset/Textures/weiba.png");
}

void EnemyManager::Update(const PlayerPlanet& playerPlanet)
{
	m_spawnTimer += Application::Instance().GetDeltaTime();

	if (m_spawnTimer >= m_spawnInterval)
	{
		m_spawnTimer -= m_spawnInterval;
		SpawnEnemy();
	}

	while (m_pendingCometCount > 0)
	{
		--m_pendingCometCount;
		SpawnComet();
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
	if (enemy.IsComet()) { return; }

	++m_defeatCountForComet;
	if (m_defeatCountForComet >= 5)
	{
		m_defeatCountForComet = 0;
		++m_pendingCometCount;
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

void EnemyManager::SetupEnemyByType(const std::shared_ptr<Enemy>& enemy, EnemyType type)
{
	if (!enemy) { return; }

	switch (type)
	{
	case EnemyType::Comet:
		enemy->SetTexture(m_spCometTex);
		enemy->SetStatus(
			3,
			0,
			50,
			0.0f,
			{ 120.0f, 120.0f },
			48.0f);
		break;
	case EnemyType::Medium:
		enemy->SetTexture(m_spMediumMeteorTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			2,
			2,
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
			4,
			4,
			m_largeMeteorSpeed,
			{ 320.0f, 320.0f },
			96.0f);
		break;
	case EnemyType::Small:
	default:
		enemy->SetTexture(m_spSmallMeteorTex);
		enemy->SetGravityTrailTexture(m_spGravityTrailTex);
		enemy->SetStatus(
			1,
			1,
			1,
			m_smallMeteorSpeed,
			{ 80.0f, 80.0f },
			32.0f);
		break;
	}
}
