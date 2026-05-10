#pragma once

#include "Application/Scene/PauseScene.h"
#include "Application/Scene/ResultScene.h"
#include "Application/Scene/StageSelectScene.h"
#include "Application/Scene/TitleScene.h"

class SceneManager
{
public:
	enum class SceneType
	{
		Title,
		StageSelect,
		Game,
		Pause,
		Result
	};

	void Init();
	void Update();
	void DrawSprite();
	void ChangeScene(SceneType nextScene);
	void ResetResultStats();
	void AddResultKill(int value = 1);
	void AddResultMiss(int value = 1);
	void AddResultDamage(float value);
	void AddResultTime(float seconds);
	void StartResultScene();

	SceneType GetCurrentScene() const { return m_currentScene; }
	bool IsTitleScene() const { return m_currentScene == SceneType::Title; }
	bool IsGameScene() const { return m_currentScene == SceneType::Game; }
	bool IsPauseScene() const { return m_currentScene == SceneType::Pause; }
	bool IsResultScene() const { return m_currentScene == SceneType::Result; }
	bool IsStageSelectScene() const { return m_currentScene == SceneType::StageSelect; }

private:
	SceneType m_currentScene = SceneType::Title;
	TitleScene m_titleScene;
	StageSelectScene m_stageSelectScene;
	PauseScene m_pauseScene;
	ResultScene m_resultScene;
	bool m_prevGameEscape = false;
};
