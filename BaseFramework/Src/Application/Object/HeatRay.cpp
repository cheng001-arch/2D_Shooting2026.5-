#include "HeatRay.h"
#include "Application/main.h"
#include "Application/Object/EnergySystem.h"
#include "Application/Object/Enemy.h"
#include "Application/Object/EnemyManager.h"
#include "Application/Object/ExplosionManager.h"
#include "Application/Object/Turret.h"

#include <algorithm>
#include <cmath>

void HeatRay::Init(
	const std::shared_ptr<Turret>& turret,
	const std::shared_ptr<EnemyManager>& enemyManager,
	const std::shared_ptr<EnergySystem>& energySystem,
	const std::shared_ptr<ExplosionManager>& explosionManager)
{
	m_wpTurret = turret;
	m_wpEnemyManager = enemyManager;
	m_wpEnergySystem = energySystem;
	m_wpExplosionManager = explosionManager;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/lizi.png");

	m_spHitEffectTex = std::make_shared<KdTexture>();
	m_spHitEffectTex->Load("Asset/Textures/rerongqiangxiaoguo.png");
}

void HeatRay::Reset()
{
	m_damageTimers.clear();
	m_hitEffects.clear();
	m_heat = 0.0f;
	m_fireChargeFrame = 0.0f;
	m_overheatCoolFrame = 0.0f;
	m_currentRayLength = 0.0f;
	m_dissolveStartLength = 0.0f;
	m_particleScroll = 0.0f;
	m_isFiring = false;
	m_isOverheated = false;
	m_isDissolving = false;
}

void HeatRay::Update(bool wantsFire)
{
	const float dt = Application::Instance().GetDeltaTime();
	UpdateHitEffects();

	const bool canCharge = wantsFire && !m_isOverheated;
	if (canCharge)
	{
		m_fireChargeFrame = std::min(m_fireChargeInterval, m_fireChargeFrame + dt);
	}
	else
	{
		m_fireChargeFrame = 0.0f;
	}

	const bool canFire = canCharge && m_fireChargeFrame >= m_fireChargeInterval;
	UpdateHeat(canFire);

	const std::shared_ptr<Turret> turret = m_wpTurret.lock();
	if (!turret)
	{
		m_isFiring = false;
		m_isDissolving = false;
		m_currentRayLength = 0.0f;
		return;
	}

	m_muzzlePos = turret->GetMuzzlePos();
	m_direction = turret->GetAimDir();
	if (m_direction.LengthSquared() <= 0.001f)
	{
		m_direction = { 0.0f, 1.0f };
	}
	m_direction.Normalize();

	if (m_isFiring)
	{
		m_isDissolving = false;
		m_dissolveStartLength = 0.0f;

		m_currentRayLength = std::min(
			m_rayLength,
			m_currentRayLength + m_rayExtendSpeed * Application::Instance().GetDeltaTime());

		m_particleScroll += m_particleScrollSpeed * Application::Instance().GetDeltaTime();
		if (m_particleScroll >= m_particleSpacing)
		{
			m_particleScroll = std::fmod(m_particleScroll, m_particleSpacing);
		}

		HitEnemies();
	}
	else
	{
		m_damageTimers.clear();

		if (m_currentRayLength > 0.0f)
		{
			m_isDissolving = true;
			m_dissolveStartLength += m_rayDissolveSpeed * Application::Instance().GetDeltaTime();

			if (m_dissolveStartLength >= m_currentRayLength)
			{
				m_isDissolving = false;
				m_currentRayLength = 0.0f;
				m_dissolveStartLength = 0.0f;
			}
		}
	}
}

void HeatRay::DrawSprite()
{
	if ((!m_isFiring && !m_isDissolving) || !m_spTex)
	{
		DrawHitEffects();
		return;
	}

	const float angle = static_cast<float>(std::atan2(m_direction.y, m_direction.x)) + DirectX::XMConvertToRadians(-90.0f);
	const float startDistance = m_isDissolving ? m_dissolveStartLength : 0.0f;
	for (float distance = startDistance + m_particleScroll; distance <= m_currentRayLength; distance += m_particleSpacing)
	{
		const Math::Vector2 particlePos = m_muzzlePos + m_direction * distance;

		Math::Matrix mat =
			Math::Matrix::CreateRotationZ(angle) *
			Math::Matrix::CreateTranslation(particlePos.x, particlePos.y, 0.0f);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spTex.get(),
			0,
			0,
			static_cast<int>(m_particleSize),
			static_cast<int>(m_particleSize));
	}

	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
	DrawHitEffects();
}

void HeatRay::UpdateHeat(bool canFire)
{
	const float dt = Application::Instance().GetDeltaTime();

	if (m_isOverheated)
	{
		m_isFiring = false;
		m_overheatCoolFrame += dt;

		if (m_overheatCoolFrame >= m_overheatCoolInterval)
		{
			m_isOverheated = false;
			m_overheatCoolFrame = 0.0f;
			m_heat = 0.0f;
		}
		return;
	}

	m_isFiring = canFire;

	if (m_isFiring)
	{
		m_heat += m_heatUpPerFrame * dt;
		if (m_heat >= m_maxHeat)
		{
			m_heat = m_maxHeat;
			m_isOverheated = true;
			m_isFiring = false;
			m_overheatCoolFrame = 0.0f;
		}
		return;
	}

	m_heat = std::max(0.0f, m_heat - m_coolDownPerFrame * dt);
}

void HeatRay::HitEnemies()
{
	const std::shared_ptr<EnemyManager> enemyManager = m_wpEnemyManager.lock();
	const std::shared_ptr<EnergySystem> energySystem = m_wpEnergySystem.lock();
	const std::shared_ptr<ExplosionManager> explosionManager = m_wpExplosionManager.lock();
	if (!enemyManager || !energySystem || !explosionManager) { return; }

	const float dt = Application::Instance().GetDeltaTime();
	std::vector<std::shared_ptr<Enemy>>& enemies = enemyManager->WorkEnemies();

	for (const std::shared_ptr<Enemy>& enemy : enemies)
	{
		if (!enemy || enemy->IsExpired()) { continue; }

		Enemy* enemyKey = enemy.get();
		if (!IsEnemyOnRay(enemy->GetPos2D(), enemy->GetRadius()))
		{
			m_damageTimers.erase(enemyKey);
			continue;
		}

		auto timerItr = m_damageTimers.find(enemyKey);
		if (timerItr == m_damageTimers.end())
		{
			Application::Instance().AddResultDamage(m_damagePower);
			SpawnHitEffect(enemy->GetPos2D());
			enemy->Damage(m_damagePower);

			if (enemy->IsExpired())
			{
				explosionManager->Spawn(enemy->GetPos2D());
				Application::Instance().AddResultKill();
				enemyManager->NotifyEnemyDefeated(*enemy);
				energySystem->AddEnergy(static_cast<float>(enemy->GetEnergyReward()));
				if (enemy->CanTriggerFullScreenBurst())
				{
					TriggerStage3CrystalBurst(*enemy, *enemyManager, *energySystem, *explosionManager);
				}
				m_damageTimers.erase(enemyKey);
				continue;
			}

			timerItr = m_damageTimers.emplace(enemyKey, 0.0f).first;
		}

		float& timer = timerItr->second;
		timer += dt;

		while (timer >= m_damageInterval && !enemy->IsExpired())
		{
			timer -= m_damageInterval;
			Application::Instance().AddResultDamage(m_damagePower);
			SpawnHitEffect(enemy->GetPos2D());
			enemy->Damage(m_damagePower);

			if (enemy->IsExpired())
			{
				explosionManager->Spawn(enemy->GetPos2D());
				Application::Instance().AddResultKill();
				enemyManager->NotifyEnemyDefeated(*enemy);
				energySystem->AddEnergy(static_cast<float>(enemy->GetEnergyReward()));
				if (enemy->CanTriggerFullScreenBurst())
				{
					TriggerStage3CrystalBurst(*enemy, *enemyManager, *energySystem, *explosionManager);
				}
				m_damageTimers.erase(enemyKey);
				break;
			}
		}
	}
}

void HeatRay::TriggerStage3CrystalBurst(Enemy& crystalEnemy, EnemyManager& enemyManager, EnergySystem& energySystem, ExplosionManager& explosionManager)
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
			explosionManager.Spawn(enemy->GetPos2D());
			Application::Instance().AddResultKill();
			enemyManager.NotifyEnemyDefeated(*enemy);
			energySystem.AddEnergy(static_cast<float>(enemy->GetEnergyReward()));
			m_damageTimers.erase(enemy.get());
		}
	}
}

void HeatRay::SpawnHitEffect(const Math::Vector2& pos)
{
	HitEffect effect;
	effect.pos = pos;
	effect.frame = 0.0f;
	m_hitEffects.push_back(effect);
}

void HeatRay::UpdateHitEffects()
{
	const float dt = Application::Instance().GetDeltaTime();
	for (HitEffect& effect : m_hitEffects)
	{
		effect.frame += (m_hitEffectAnimFps / 60.0f) * dt;
	}

	m_hitEffects.erase(
		std::remove_if(
			m_hitEffects.begin(),
			m_hitEffects.end(),
			[this](const HitEffect& effect)
			{
				return effect.frame >= static_cast<float>(m_hitEffectFrameCount);
			}),
		m_hitEffects.end());
}

void HeatRay::DrawHitEffects()
{
	if (!m_spHitEffectTex) { return; }

	for (const HitEffect& effect : m_hitEffects)
	{
		const int frame = std::clamp(static_cast<int>(effect.frame), 0, m_hitEffectFrameCount - 1);
		const Math::Rectangle srcRect = {
			frame * m_hitEffectFrameWidth,
			m_hitEffectRowIndex * m_hitEffectFrameHeight,
			m_hitEffectFrameWidth,
			m_hitEffectFrameHeight
		};

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spHitEffectTex.get(),
			static_cast<int>(effect.pos.x),
			static_cast<int>(effect.pos.y),
			static_cast<int>(m_hitEffectDrawSize),
			static_cast<int>(m_hitEffectDrawSize),
			&srcRect);
	}
}

bool HeatRay::IsEnemyOnRay(const Math::Vector2& enemyPos, float enemyRadius) const
{
	const Math::Vector2 toEnemy = enemyPos - m_muzzlePos;
	const float forwardDistance = toEnemy.Dot(m_direction);
	if (forwardDistance < 0.0f || forwardDistance > m_currentRayLength) { return false; }

	const Math::Vector2 closestPoint = m_muzzlePos + m_direction * forwardDistance;
	const Math::Vector2 toRay = enemyPos - closestPoint;
	const float hitRadius = enemyRadius + std::max(m_rayWidth, m_particleSize) * 0.5f;

	return toRay.LengthSquared() <= hitRadius * hitRadius;
}
