#include "TitleScene.h"
#include "Application/main.h"
#include "Application/Scene/SceneManager.h"

void TitleScene::Init(SceneManager* sceneManager)
{
	m_sceneManager = sceneManager;

	m_spBackgroundTex = std::make_shared<KdTexture>();
	m_spBackgroundTex->Load("Asset/Textures/yuzhoubeijing.png");

	m_spTitleTex = std::make_shared<KdTexture>();
	m_spTitleTex->Load("Asset/Textures/title.png");
	m_titleSize = FitTextureSize(m_spTitleTex, 560.0f);
	m_titlePos = { 0.0f, 170.0f };

	m_startButton.tex = std::make_shared<KdTexture>();
	m_startButton.tex->Load("Asset/Textures/start.png");
	m_startButton.pos = { 0.0f, -70.0f };
	FitButtonSize(m_startButton, 300.0f);

	m_exitButton.tex = std::make_shared<KdTexture>();
	m_exitButton.tex->Load("Asset/Textures/exit.png");
	m_exitButton.pos = { 0.0f, -250.0f };
	FitButtonSize(m_exitButton, 300.0f);
}

void TitleScene::Update()
{
	const Math::Vector2 mousePos = GetMouseSpritePos();
	const bool leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	const bool leftTrigger = leftButton && !m_prevLeftButton;

	UpdateButton(m_startButton, mousePos);
	UpdateButton(m_exitButton, mousePos);

	if (leftTrigger && m_startButton.hover && m_sceneManager)
	{
		m_sceneManager->ChangeScene(SceneManager::SceneType::StageSelect);
	}
	else if (leftTrigger && m_exitButton.hover)
	{
		Application::Instance().End();
	}

	m_prevLeftButton = leftButton;
}

void TitleScene::DrawSprite()
{
	if (m_spBackgroundTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spBackgroundTex.get(),
			0,
			0,
			1280,
			720);
	}

	DrawTitle();
	DrawButton(m_startButton);
	DrawButton(m_exitButton);
}

Math::Vector2 TitleScene::GetMouseSpritePos() const
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

bool TitleScene::IsMouseOverButton(const Button& button, const Math::Vector2& mousePos) const
{
	const float margin = 26.0f;
	const float halfW = button.size.x * button.scale * 0.5f + margin;
	const float halfH = button.size.y * button.scale * 0.5f + margin;

	return
		mousePos.x >= button.pos.x - halfW &&
		mousePos.x <= button.pos.x + halfW &&
		mousePos.y >= button.pos.y - halfH &&
		mousePos.y <= button.pos.y + halfH;
}

void TitleScene::UpdateButton(Button& button, const Math::Vector2& mousePos)
{
	button.hover = IsMouseOverButton(button, mousePos);
	const float targetScale = button.hover ? 1.08f : 1.0f;
	button.scale += (targetScale - button.scale) * 0.25f;
}

void TitleScene::DrawButton(const Button& button)
{
	if (!button.tex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		button.tex.get(),
		static_cast<int>(button.pos.x),
		static_cast<int>(button.pos.y),
		static_cast<int>(button.size.x * button.scale),
		static_cast<int>(button.size.y * button.scale));
}

void TitleScene::FitButtonSize(Button& button, float targetWidth)
{
	button.size = FitTextureSize(button.tex, targetWidth);
}

Math::Vector2 TitleScene::FitTextureSize(const std::shared_ptr<KdTexture>& tex, float targetWidth) const
{
	if (!tex) { return { targetWidth, targetWidth }; }

	const UINT texW = tex->GetWidth();
	const UINT texH = tex->GetHeight();

	if (texW == 0 || texH == 0) { return { targetWidth, targetWidth }; }

	const float aspect = static_cast<float>(texH) / static_cast<float>(texW);
	return { targetWidth, targetWidth * aspect };
}

void TitleScene::DrawTitle()
{
	if (!m_spTitleTex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTitleTex.get(),
		static_cast<int>(m_titlePos.x),
		static_cast<int>(m_titlePos.y),
		static_cast<int>(m_titleSize.x),
		static_cast<int>(m_titleSize.y));
}
