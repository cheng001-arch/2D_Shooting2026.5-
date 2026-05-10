#pragma once

class SceneManager;

class PauseScene
{
public:
	void Init(SceneManager* sceneManager);
	void Update();
	void DrawSprite();

private:
	struct Button
	{
		std::shared_ptr<KdTexture> tex = nullptr;
		Math::Vector2 pos = Math::Vector2::Zero;
		Math::Vector2 size = { 160.0f, 160.0f };
		float scale = 1.0f;
		bool hover = false;
	};

	Math::Vector2 GetMouseSpritePos() const;
	bool IsMouseOverButton(const Button& button, const Math::Vector2& mousePos) const;
	void UpdateButton(Button& button, const Math::Vector2& mousePos);
	void DrawButton(const Button& button);
	void FitButtonSize(Button& button, float targetWidth);

	SceneManager* m_sceneManager = nullptr;
	std::shared_ptr<KdTexture> m_spBackgroundTex = nullptr;
	Button m_backButton;
	Button m_menuButton;
	bool m_prevLeftButton = false;
};
