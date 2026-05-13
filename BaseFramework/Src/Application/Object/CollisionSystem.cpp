#include "CollisionSystem.h"
#include "Application/main.h"
#include "Application/Object/EnergySystem.h"
#include "Application/Object/Enemy.h"
#include "Application/Object/EnemyManager.h"
#include "Application/Object/ExplosionManager.h"
#include "Application/Object/PlayerPlanet.h"
#include "Application/Object/ProjectileManager.h"
#include "Application/Object/Turret.h"

void CollisionSystem::Update(ProjectileManager& projectileManager, EnemyManager& enemyManager, PlayerPlanet& playerPlanet, Turret& turret, EnergySystem& energySystem, ExplosionManager& explosionManager)
{
	HitProjectilesToEnemies(projectileManager, enemyManager, energySystem, explosionManager);
	HitEnemiesToPlayerPlanet(enemyManager, playerPlanet, turret, explosionManager);
	projectileManager.CleanupExpiredProjectiles();
}

void CollisionSystem::HitProjectilesToEnemies(ProjectileManager& projectileManager, EnemyManager& enemyManager, EnergySystem& energySystem, ExplosionManager& explosionManager)
{
	std::vector<ProjectileManager::Projectile>& projectiles = projectileManager.WorkProjectiles();
	std::vector<std::shared_ptr<Enemy>>& enemies = enemyManager.WorkEnemies();

	for (ProjectileManager::Projectile& projectile : projectiles)
	{
		if (projectile.attackPower <= 0) { continue; }

		for (const std::shared_ptr<Enemy>& enemy : enemies)
		{
			if (!enemy || enemy->IsExpired()) { continue; }

			const Math::Vector2 toEnemy = enemy->GetPos2D() - projectile.pos;
			const float hitRadius = projectile.radius + enemy->GetRadius();

			if (toEnemy.LengthSquared() > hitRadius * hitRadius) { continue; }

			const Math::Vector2 explosionPos = (projectile.pos + enemy->GetPos2D()) * 0.5f;
			explosionManager.Spawn(explosionPos);

			const bool wasAlive = !enemy->IsExpired();
			Application::Instance().AddResultDamage(static_cast<float>(projectile.attackPower));
			enemy->Damage(static_cast<float>(projectile.attackPower));
			if (wasAlive && enemy->IsExpired())
			{
				Application::Instance().AddResultKill();
				enemyManager.NotifyEnemyDefeated(*enemy);
				energySystem.AddEnergy(static_cast<float>(enemy->GetEnergyReward()));
				if (enemy->CanTriggerFullScreenBurst())
				{
					TriggerStage3CrystalBurst(*enemy, enemyManager, energySystem, explosionManager);
				}
				if (enemy->CanTriggerFullScreenSlowBurst())
				{
					constexpr float kSlowMultiplier = 0.25f;
					constexpr float kSlowDurationFrame = 300.0f;
					explosionManager.SpawnStage8SpecialBurst(enemy->GetPos2D());
					enemyManager.SlowAllEnemies(enemy.get(), kSlowMultiplier, kSlowDurationFrame);
				}
			}
			projectile.attackPower = 0;
			break;
		}
	}
}

void CollisionSystem::HitEnemiesToPlayerPlanet(EnemyManager& enemyManager, PlayerPlanet& playerPlanet, Turret& turret, ExplosionManager& explosionManager)
{
	std::vector<std::shared_ptr<Enemy>>& enemies = enemyManager.WorkEnemies();

	for (const std::shared_ptr<Enemy>& enemy : enemies)
	{
		if (!enemy || enemy->IsExpired()) { continue; }
		if (enemy->IsComet()) { continue; }

		const Math::Vector2 toPlanet = playerPlanet.GetSpritePos() - enemy->GetPos2D();
		const float hitRadius = playerPlanet.GetRadius() + enemy->GetRadius();

		if (toPlanet.LengthSquared() > hitRadius * hitRadius) { continue; }

		explosionManager.SpawnPlanetHit(enemy->GetPos2D(), enemy->GetRadius());
		enemyManager.NotifyEnemyCrashedIntoPlanet(*enemy);
		Application::Instance().AddResultMiss();
		playerPlanet.Damage(enemy->GetAttackPower());
		turret.ShakeDamage();
		enemy->Damage(999);
	}
}

void CollisionSystem::TriggerStage3CrystalBurst(Enemy& crystalEnemy, EnemyManager& enemyManager, EnergySystem& energySystem, ExplosionManager& explosionManager)
{
	constexpr float kBurstDamage = 20.0f;
	constexpr float kBurstExplosionSize = 560.0f;

	explosionManager.SpawnLarge(crystalEnemy.GetPos2D(), kBurstExplosionSize);

	for (const std::shared_ptr<Enemy>& enemy : enemyManager.WorkEnemies())
	{
		if (!enemy || enemy.get() == &crystalEnemy || enemy->IsExpired()) { continue; }

		const bool wasAlive = !enemy->IsExpired();
		Application::Instance().AddResultDamage(kBurstDamage);
		enemy->Damage(kBurstDamage);

		if (wasAlive && enemy->IsExpired())
		{
			if (enemy->CanTriggerFullScreenSlowBurst())
			{
				constexpr float kSlowMultiplier = 0.25f;
				constexpr float kSlowDurationFrame = 300.0f;
				explosionManager.SpawnStage8SpecialBurst(enemy->GetPos2D());
				enemyManager.SlowAllEnemies(enemy.get(), kSlowMultiplier, kSlowDurationFrame);
			}
			else
			{
				explosionManager.Spawn(enemy->GetPos2D());
			}
			Application::Instance().AddResultKill();
			enemyManager.NotifyEnemyDefeated(*enemy);
			energySystem.AddEnergy(static_cast<float>(enemy->GetEnergyReward()));
		}
	}
}
