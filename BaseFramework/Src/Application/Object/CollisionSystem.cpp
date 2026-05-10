#include "CollisionSystem.h"
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
			enemy->Damage(static_cast<float>(projectile.attackPower));
			if (wasAlive && enemy->IsExpired())
			{
				enemyManager.NotifyEnemyDefeated(*enemy);
				energySystem.AddEnergy(static_cast<float>(enemy->GetEnergyReward()));
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
		playerPlanet.Damage(enemy->GetAttackPower());
		turret.ShakeDamage();
		enemy->Damage(999);
	}
}
