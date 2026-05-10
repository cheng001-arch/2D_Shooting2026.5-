#include "PauseScene.h"
#include "Application/main.h"
#include "Application/Scene/SceneManager.h"

void PauseScene::Init(SceneManager* sceneManager)
{
	m_sceneManager = sceneManager;

	m_spBackgroundTex = std::make_shared<KdTexture>();
	m_spBackgroundTex->Load("Asset/Textures/zanting.png");

	m_backButton.tex = std::make_shared<KdTexture>();
	m_backButton.tex->Load("Asset/Textures/back.png");
	m_backButton.pos = { -150.0f, -120.0f };
	FitButtonSize(m_backButton, 170.0f);

	m_menuButton.tex = std::make_shared<KdTexture>();
	m_menuButton.tex->Load("Asset/Textures/menu.png");
	m_menuButton.pos = { 150.0f, -120.0f };
	FitButtonSize(m_menuButton, 170.0f);
}

void PauseScene::Update()
{
	const Math::Vector2 mousePos = GetMouseSpritePos();
	const bool leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	const bool leftTrigger = leftButton && !m_prevLeftButton;

	UpdateButton(m_backButton, mousePos);
	UpdateButton(m_menuButton, mousePos);

	if (leftTrigger && m_backButton.hover && m_sceneManager)
	{
		m_sceneManager->ChangeScene(SceneManager::SceneType::Game);
	}
	else if (leftTrigger && m_menuButton.hover && m_sceneManager)
	{
		m_sceneManager->ChangeScene(SceneManager::SceneType::Title);
	}

	m_prevLeftButton = leftButton;
}

void PauseScene::DrawSprite()
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

	DrawButton(m_backButton);
	DrawButton(m_menuButton);
}

Math::Vector2 PauseScene::GetMouseSpritePos() const
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

bool PauseScene::IsMouseOverButton(const Button& button, const Math::Vector2& mousePos) const
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

void PauseScene::UpdateButton(Button& button, const Math::Vector2& mousePos)
{
	button.hover = IsMouseOverButton(button, mousePos);
	const float targetScale = button.hover ? 1.12f : 1.0f;
	button.scale += (targetScale - button.scale) * 0.25f;
}

void PauseScene::DrawButton(const Button& button)
{
	if (!button.tex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		button.tex.get(),
		static_cast<int>(button.pos.x),
		static_cast<int>(button.pos.y),
		static_cast<int>(button.size.x * button.scale),
		static_cast<int>(button.size.y * button.scale));
}

void PauseScene::FitButtonSize(Button& button, float targetWidth)
{
	if (!button.tex) { return; }

	const UINT texW = button.tex->GetWidth();
	const UINT texH = button.tex->GetHeight();

	if (texW == 0 || texH == 0) { return; }

	const float aspect = static_cast<float>(texH) / static_cast<float>(texW);
	button.size = { targetWidth, targetWidth * aspect };
}
