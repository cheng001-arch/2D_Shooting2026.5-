#pragma once

class SceneManager;

class ResultScene
{
public:
	void Init(SceneManager* sceneManager);
	void ResetStats();
	void AddKill(int value = 1);
	void AddMiss(int value = 1);
	void AddDamage(float value);
	void AddTime(float seconds);
	void Start();
	void Update();
	void DrawSprite();

private:
	void DrawResultText();

	SceneManager* m_sceneManager = nullptr;
	std::shared_ptr<KdTexture> m_spBackgroundTex = nullptr;

	int m_kills = 0;
	int m_miss = 0;
	float m_damage = 0.0f;
	float m_timeSeconds = 0.0f;
	float m_resultFrame = 0.0f;
	float m_resultDuration = 300.0f;
};
