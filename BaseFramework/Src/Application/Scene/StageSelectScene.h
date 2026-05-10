#pragma once

#include <array>

class SceneManager;

class StageSelectScene
{
public:
	void Init(SceneManager* sceneManager);
	void Update();
	void DrawSprite();

private:
	struct StageIcon
	{
		std::shared_ptr<KdTexture> tex = nullptr;
		Math::Vector2 basePos = Math::Vector2::Zero;
		Math::Vector2 drawPos = Math::Vector2::Zero;
		Math::Vector2 size = { 96.0f, 96.0f };
		float scale = 1.0f;
		float floatTimer = 0.0f;
		bool hover = false;
		bool active = false;
	};

	Math::Vector2 GetMouseSpritePos() const;
	Math::Vector2 GetStageGridPos(int index) const;
	void SetupStageIcon(int index, const char* texturePath);
	bool IsMouseOverIcon(const StageIcon& icon, const Math::Vector2& mousePos) const;
	void UpdateIcon(StageIcon& icon, const Math::Vector2& mousePos);
	void DrawIcon(const StageIcon& icon);

	SceneManager* m_sceneManager = nullptr;
	std::shared_ptr<KdTexture> m_spBackgroundTex = nullptr;
	std::array<StageIcon, 25> m_stageIcons;
	StageIcon m_backIcon;
	bool m_prevEscape = false;
	bool m_prevLeftButton = false;
};
