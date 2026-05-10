#pragma once

class EnergySystem;
class EnemyManager;
class ExplosionManager;
class PlayerPlanet;
class ProjectileManager;
class Turret;

class CollisionSystem
{
public:
	void Update(ProjectileManager& projectileManager, EnemyManager& enemyManager, PlayerPlanet& playerPlanet, Turret& turret, EnergySystem& energySystem, ExplosionManager& explosionManager);

private:
	void HitProjectilesToEnemies(ProjectileManager& projectileManager, EnemyManager& enemyManager, EnergySystem& energySystem, ExplosionManager& explosionManager);
	void HitEnemiesToPlayerPlanet(EnemyManager& enemyManager, PlayerPlanet& playerPlanet, Turret& turret, ExplosionManager& explosionManager);
};
