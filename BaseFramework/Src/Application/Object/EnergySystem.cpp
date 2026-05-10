#include "EnergySystem.h"
#include "Application/main.h"

#include <algorithm>

void EnergySystem::Init()
{
	m_energy = 0.0f;
}

void EnergySystem::Update()
{
	AddEnergy((m_recoveryPerSecond / 60.0f) * Application::Instance().GetDeltaTime());
}

void EnergySystem::AddEnergy(float value)
{
	m_energy = std::clamp(m_energy + value, 0.0f, m_maxEnergy);
}

bool EnergySystem::TryUseEnergy(float value)
{
	if (m_energy < value) { return false; }

	m_energy = std::clamp(m_energy - value, 0.0f, m_maxEnergy);
	return true;
}
