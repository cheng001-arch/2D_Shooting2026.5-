#pragma once

class ProgressSystem
{
public:
	void Init();
	void Update();
	void DrawSprite();
	void AddEnemyProgress(int value);
	void SetProgress(int value);

	int GetProgress() const { return m_progress; }
	bool IsComplete() const { return m_progress >= m_maxProgress; }

private:
	void DrawProgressGauge();
	void DrawClearEffect();

	std::shared_ptr<KdTexture> m_spYesTex = nullptr;

	int m_progress = 0;
	int m_maxProgress = 100;

	float m_completeWaitFrame = 0.0f;
	float m_completeWaitDuration = 240.0f;
	float m_yesAnimFrame = 0.0f;
	float m_yesAnimDuration = 90.0f;
	bool m_yesAnimStarted = false;

	Math::Vector2 m_gaugePos = { 0.0f, 330.0f };
	Math::Vector2 m_gaugeSize = { 760.0f, 30.0f };
};
