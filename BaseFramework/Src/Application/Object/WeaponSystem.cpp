#include "WeaponSystem.h"
#include "Application/main.h"
#include "Application/Object/HeatRay.h"
#include "Application/Object/ProjectileManager.h"
#include "Application/Object/Turret.h"

void WeaponSystem::Init(
	const std::shared_ptr<Turret>& turret,
	const std::shared_ptr<ProjectileManager>& projectileManager,
	const std::shared_ptr<HeatRay>& heatRay)
{
	m_wpTurret = turret;
	m_wpProjectileManager = projectileManager;
	m_wpHeatRay = heatRay;
}

void WeaponSystem::Reset()
{
	m_currentWeapon = WeaponType::Meteor;
	m_prevLeftClick = false;
	m_prevEKey = false;
	m_waitLeftRelease = true;
	m_meteorShootCoolFrame = 0.0f;
	m_mimicPlanetAmmo = m_mimicPlanetMaxAmmo;
	m_mimicPlanetRecoveryFrame = 0.0f;
}

void WeaponSystem::SetStageNo(int stageNo)
{
	m_isWeaponSwitchLocked = stageNo == 1;
	m_isMimicPlanetOnlySwitchStage = stageNo == 2;
	if (m_isWeaponSwitchLocked)
	{
		m_currentWeapon = WeaponType::Meteor;
	}
}

void WeaponSystem::Update()
{
	const float dt = Application::Instance().GetDeltaTime();

	if (m_meteorShootCoolFrame > 0.0f)
	{
		m_meteorShootCoolFrame -= dt;
	}

	UpdateMimicPlanetAmmo();
	ChangeWeapon();

	const std::shared_ptr<ProjectileManager> projectileManager = m_wpProjectileManager.lock();
	if (projectileManager)
	{
		projectileManager->Update();
	}

	const bool leftClick = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	const bool trigger = leftClick && !m_prevLeftClick;
	m_prevLeftClick = leftClick;

	const std::shared_ptr<HeatRay> heatRay = m_wpHeatRay.lock();
	if (m_waitLeftRelease)
	{
		if (heatRay)
		{
			heatRay->Update(false);
		}

		if (!leftClick)
		{
			m_waitLeftRelease = false;
		}
		return;
	}

	if (heatRay)
	{
		heatRay->Update(m_currentWeapon == WeaponType::HeatRay && leftClick);
	}

	if (!leftClick) { return; }

	const std::shared_ptr<Turret> turret = m_wpTurret.lock();
	if (!turret || !projectileManager) { return; }

	const Math::Vector2 muzzlePos = turret->GetMuzzlePos();
	const Math::Vector2 direction = GetMouseSpritePos() - muzzlePos;

	if (m_currentWeapon == WeaponType::MimicPlanet)
	{
		if (!trigger) { return; }
		if (m_mimicPlanetAmmo <= 0) { return; }

		--m_mimicPlanetAmmo;
		projectileManager->Spawn(muzzlePos, direction, ProjectileManager::AmmoType::MimicPlanet);
		return;
	}

	if (m_currentWeapon == WeaponType::HeatRay) { return; }

	if (m_meteorShootCoolFrame > 0.0f) { return; }

	projectileManager->Spawn(muzzlePos, direction, ProjectileManager::AmmoType::Meteor);
	m_meteorShootCoolFrame = m_meteorShootInterval;
}

Math::Vector2 WeaponSystem::GetMouseSpritePos() const
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

void WeaponSystem::UpdateMimicPlanetAmmo()
{
	if (m_mimicPlanetAmmo >= m_mimicPlanetMaxAmmo)
	{
		m_mimicPlanetRecoveryFrame = 0.0f;
		return;
	}

	m_mimicPlanetRecoveryFrame += Application::Instance().GetDeltaTime();
	while (m_mimicPlanetRecoveryFrame >= m_mimicPlanetRecoveryInterval && m_mimicPlanetAmmo < m_mimicPlanetMaxAmmo)
	{
		++m_mimicPlanetAmmo;
		m_mimicPlanetRecoveryFrame -= m_mimicPlanetRecoveryInterval;
	}
}

void WeaponSystem::ChangeWeapon()
{
	const bool eKey = (GetAsyncKeyState('E') & 0x8000) != 0;
	const bool trigger = eKey && !m_prevEKey;
	m_prevEKey = eKey;

	if (m_isWeaponSwitchLocked)
	{
		m_currentWeapon = WeaponType::Meteor;
		return;
	}

	if (!trigger) { return; }

	if (m_isMimicPlanetOnlySwitchStage)
	{
		m_currentWeapon = m_currentWeapon == WeaponType::MimicPlanet ? WeaponType::Meteor : WeaponType::MimicPlanet;
		return;
	}

	if (m_currentWeapon == WeaponType::Meteor)
	{
		m_currentWeapon = WeaponType::MimicPlanet;
	}
	else if (m_currentWeapon == WeaponType::MimicPlanet)
	{
		m_currentWeapon = WeaponType::HeatRay;
	}
	else
	{
		m_currentWeapon = WeaponType::Meteor;
	}
}
