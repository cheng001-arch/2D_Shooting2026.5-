#pragma once

class SceneManager;

class TitleScene
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
		Math::Vector2 size = { 320.0f, 112.0f };
		float scale = 1.0f;
		bool hover = false;
	};

	Math::Vector2 GetMouseSpritePos() const;
	bool IsMouseOverButton(const Button& button, const Math::Vector2& mousePos) const;
	void UpdateButton(Button& button, const Math::Vector2& mousePos);
	void DrawButton(const Button& button);
	void FitButtonSize(Button& button, float targetWidth);
	Math::Vector2 FitTextureSize(const std::shared_ptr<KdTexture>& tex, float targetWidth) const;
	void DrawTitle();

	SceneManager* m_sceneManager = nullptr;
	std::shared_ptr<KdTexture> m_spBackgroundTex = nullptr;
	std::shared_ptr<KdTexture> m_spTitleTex = nullptr;
	Math::Vector2 m_titlePos = { 0.0f, 190.0f };
	Math::Vector2 m_titleSize = { 600.0f, 400.0f };
	Button m_startButton;
	Button m_exitButton;
	bool m_prevLeftButton = false;
};
