#include "BlackHole.h"
#include "Application/main.h"
#include "Application/Object/EnergySystem.h"
#include "Application/Object/Enemy.h"
#include "Application/Object/EnemyManager.h"
#include "Application/Object/UIManager.h"

#include <algorithm>
#include <cmath>

void BlackHole::Init(
	const std::shared_ptr<EnergySystem>& energySystem,
	const std::shared_ptr<UIManager>& uiManager,
	const std::shared_ptr<EnemyManager>& enemyManager,
	const std::shared_ptr<ProjectileManager>& projectileManager)
{
	m_wpEnergySystem = energySystem;
	m_wpUIManager = uiManager;
	m_wpEnemyManager = enemyManager;
	m_wpProjectileManager = projectileManager;

	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load("Asset/Textures/heidong4.png");
}

void BlackHole::Reset()
{
	ReleaseCapturedEnemies();
	m_enemyCenterTimers.clear();
	m_projectileCenterTimers.clear();
	m_lifeFrame = 0.0f;
	m_anime = 0.0f;
	m_isActive = false;
	m_prevRKey = false;
}

void BlackHole::Update()
{
	const bool rKey = (GetAsyncKeyState('R') & 0x8000) != 0;
	const bool trigger = rKey && !m_prevRKey;
	m_prevRKey = rKey;

	if (trigger)
	{
		const std::shared_ptr<EnergySystem> energySystem = m_wpEnergySystem.lock();
		if (energySystem && energySystem->TryUseEnergy(m_needEnergy))
		{
			CreateBlackHole({ 0.0f, 180.0f });
		}
		else
		{
			const std::shared_ptr<UIManager> uiManager = m_wpUIManager.lock();
			if (uiManager)
			{
				uiManager->ShakeEnergyGauge();
			}
		}
	}

	if (!m_isActive) { return; }

	const float dt = Application::Instance().GetDeltaTime();
	m_lifeFrame += dt;
	m_anime += (m_animFps / 60.0f) * dt;
	if (m_anime >= static_cast<float>(m_frameCount))
	{
		m_anime = 0.0f;
	}

	if (m_lifeFrame >= m_lifeLimitFrame)
	{
		ReleaseCapturedEnemies();
		m_isActive = false;
		m_lifeFrame = 0.0f;
		m_enemyCenterTimers.clear();
		m_projectileCenterTimers.clear();
		return;
	}

	PullEnemies();
	PullProjectiles();
}

void BlackHole::DrawSprite()
{
	if (!m_isActive || !m_spTex) { return; }

	const int currentFrame = static_cast<int>(m_anime) % m_frameCount;
	const int col = currentFrame % m_frameColumns;
	const int row = currentFrame / m_frameColumns;
	const Math::Rectangle srcRect = {
		col * m_frameWidth,
		row * m_frameHeight,
		m_frameWidth,
		m_frameHeight
	};

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTex.get(),
		static_cast<int>(m_pos.x),
		static_cast<int>(m_pos.y),
		static_cast<int>(m_drawSize.x),
		static_cast<int>(m_drawSize.y),
		&srcRect);
}

Math::Vector2 BlackHole::GetMouseSpritePos() const
{
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	RECT clientRect{};
	GetClientRect(Application::Instance().GetWindowHandle(), &clientRect);

	const float clientW = static_cast<float>(clientRect.right - clientRect.left);
	const float clientH = static_cast<float>(clientRect.bottom - clientRect.top);

	return {
		static_cast<float>(mousePos.x) - clientW * 0.5f,
		clientH * 0.5f - static_cast<float>(mousePos.y)
	};
}

void BlackHole::CreateBlackHole(const Math::Vector2& pos)
{
	ReleaseCapturedEnemies();
	m_pos = pos;
	m_lifeFrame = 0.0f;
	m_anime = 0.0f;
	m_enemyCenterTimers.clear();
	m_projectileCenterTimers.clear();
	m_isActive = true;
}

void BlackHole::PullEnemies()
{
	const std::shared_ptr<EnemyManager> enemyManager = m_wpEnemyManager.lock();
	if (!enemyManager) { return; }

	const float dt = Application::Instance().GetDeltaTime();
	std::vector<std::shared_ptr<Enemy>>& enemies = enemyManager->WorkEnemies();

	for (const std::shared_ptr<Enemy>& enemy : enemies)
	{
		if (!enemy || enemy->IsExpired()) { continue; }

		enemy->SetBlackHoleCaptured(true);
		const Math::Vector2 toCenter = m_pos - enemy->GetPos2D();
		const float distance = toCenter.Length();

		if (distance > 0.001f)
		{
			Math::Vector2 pullDir = toCenter;
			pullDir.Normalize();
			const float moveDistance = std::min(distance, m_pullSpeed * dt);
			enemy->SetPos(enemy->GetPos2D() + pullDir * moveDistance);
		}

		if (distance <= m_centerRadius)
		{
			float& timer = m_enemyCenterTimers[enemy.get()];
			timer += dt;
			if (timer >= m_absorbDelayFrame)
			{
				Application::Instance().AddResultKill();
				enemy->Damage(9999.0f);
			}
		}
		else
		{
			m_enemyCenterTimers.erase(enemy.get());
		}
	}
}

void BlackHole::PullProjectiles()
{
	const std::shared_ptr<ProjectileManager> projectileManager = m_wpProjectileManager.lock();
	if (!projectileManager) { return; }

	const float dt = Application::Instance().GetDeltaTime();
	std::vector<ProjectileManager::Projectile>& projectiles = projectileManager->WorkProjectiles();

	for (ProjectileManager::Projectile& projectile : projectiles)
	{
		if (projectile.attackPower <= 0) { continue; }

		const Math::Vector2 toCenter = m_pos - projectile.pos;
		const float distance = toCenter.Length();

		if (distance > 0.001f)
		{
			Math::Vector2 pullDir = toCenter;
			pullDir.Normalize();
			const float moveDistance = std::min(distance, m_pullSpeed * dt);
			projectile.pos += pullDir * moveDistance;
			projectile.velocity = Math::Vector2::Zero;
			projectile.angle = static_cast<float>(std::atan2(pullDir.y, pullDir.x)) + DirectX::XMConvertToRadians(-90.0f);
		}

		if (distance <= m_centerRadius)
		{
			float& timer = m_projectileCenterTimers[&projectile];
			timer += dt;
			if (timer >= m_absorbDelayFrame)
			{
				projectile.attackPower = 0;
			}
		}
		else
		{
			m_projectileCenterTimers.erase(&projectile);
		}
	}

	projectileManager->CleanupExpiredProjectiles();
}

void BlackHole::ReleaseCapturedEnemies()
{
	const std::shared_ptr<EnemyManager> enemyManager = m_wpEnemyManager.lock();
	if (!enemyManager) { return; }

	for (const std::shared_ptr<Enemy>& enemy : enemyManager->WorkEnemies())
	{
		if (enemy)
		{
			enemy->SetBlackHoleCaptured(false);
		}
	}
}
