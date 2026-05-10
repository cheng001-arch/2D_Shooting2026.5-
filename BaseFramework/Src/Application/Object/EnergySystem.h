#pragma once

class EnergySystem
{
public:
	void Init();
	void Reset();
	void Update();

	void AddEnergy(float value);
	bool TryUseEnergy(float value);

	float GetEnergy() const { return m_energy; }
	float GetMaxEnergy() const { return m_maxEnergy; }

private:
	float m_energy = 0.0f;
	float m_maxEnergy = 100.0f;
	float m_recoveryPerSecond = 1.0f;
};
