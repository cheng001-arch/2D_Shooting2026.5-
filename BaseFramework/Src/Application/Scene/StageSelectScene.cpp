#include "StageSelectScene.h"
#include "Application/main.h"
#include "Application/Scene/SceneManager.h"

void StageSelectScene::Init(SceneManager* sceneManager)
{
	m_sceneManager = sceneManager;

	m_spBackgroundTex = std::make_shared<KdTexture>();
	m_spBackgroundTex->Load("Asset/Textures/yuzhoubeijing2.png");

	SetupStageIcon(0, "Asset/Textures/yunshi2.png");
	SetupStageIcon(1, "Asset/Textures/yunshi1.png");
	SetupStageIcon(2, "Asset/Textures/zishuijingxingqiu2.png");
	SetupStageIcon(3, "Asset/Textures/zishuijingxingqiu.png");
	SetupStageIcon(4, "Asset/Textures/huixingwei.png");
	SetupStageIcon(5, "Asset/Textures/zisexingqiu.png");
	SetupStageIcon(6, "Asset/Textures/lvsexingqiu.png");
	SetupStageIcon(7, "Asset/Textures/lansexingqiu.png");
	SetupStageIcon(8, "Asset/Textures/nitaixingqiu.png");

	m_backIcon.tex = std::make_shared<KdTexture>();
	m_backIcon.tex->Load("Asset/Textures/back.png");
	m_backIcon.basePos = { -540.0f, -280.0f };
	m_backIcon.drawPos = m_backIcon.basePos;
	m_backIcon.size = { 128.0f, 128.0f };
	m_backIcon.active = true;
}

void StageSelectScene::Update()
{
	const Math::Vector2 mousePos = GetMouseSpritePos();
	const bool leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	const bool leftTrigger = leftButton && !m_prevLeftButton;

	const bool escape = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
	if (escape && !m_prevEscape && m_sceneManager)
	{
		m_sceneManager->ChangeScene(SceneManager::SceneType::Title);
	}

	for (StageIcon& icon : m_stageIcons)
	{
		UpdateIcon(icon, mousePos);
	}
	UpdateIcon(m_backIcon, mousePos);

	if (leftTrigger && m_backIcon.hover && m_sceneManager)
	{
		m_sceneManager->ChangeScene(SceneManager::SceneType::Title);
	}
	else if (leftTrigger && m_sceneManager)
	{
		for (const StageIcon& icon : m_stageIcons)
		{
			if (icon.hover)
			{
				m_sceneManager->ChangeScene(SceneManager::SceneType::Game);
				break;
			}
		}
	}

	m_prevEscape = escape;
	m_prevLeftButton = leftButton;
}

void StageSelectScene::DrawSprite()
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

	for (const StageIcon& icon : m_stageIcons)
	{
		DrawIcon(icon);
	}
	DrawIcon(m_backIcon);
}

Math::Vector2 StageSelectScene::GetMouseSpritePos() const
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

Math::Vector2 StageSelectScene::GetStageGridPos(int index) const
{
	constexpr int columns = 5;
	const float startX = -460.0f;
	const float startY = 220.0f;
	const float gapX = 230.0f;
	const float gapY = 120.0f;

	const int col = index % columns;
	const int row = index / columns;

	return {
		startX + gapX * static_cast<float>(col),
		startY - gapY * static_cast<float>(row)
	};
}

void StageSelectScene::SetupStageIcon(int index, const char* texturePath)
{
	if (index < 0 || index >= static_cast<int>(m_stageIcons.size())) { return; }

	StageIcon& icon = m_stageIcons[index];
	icon.tex = std::make_shared<KdTexture>();
	icon.tex->Load(texturePath);
	icon.basePos = GetStageGridPos(index);
	icon.drawPos = icon.basePos;
	icon.size = { 108.0f, 108.0f };
	icon.floatTimer = static_cast<float>(index) * 0.35f;
	icon.scale = 1.0f;
	icon.hover = false;
	icon.active = true;
}

bool StageSelectScene::IsMouseOverIcon(const StageIcon& icon, const Math::Vector2& mousePos) const
{
	if (!icon.active) { return false; }

	const float margin = 42.0f;
	const float halfW = icon.size.x * 0.5f + margin;
	const float halfH = icon.size.y * 0.5f + margin;

	return
		mousePos.x >= icon.basePos.x - halfW &&
		mousePos.x <= icon.basePos.x + halfW &&
		mousePos.y >= icon.basePos.y - halfH &&
		mousePos.y <= icon.basePos.y + halfH;
}

void StageSelectScene::UpdateIcon(StageIcon& icon, const Math::Vector2& mousePos)
{
	if (!icon.active) { return; }

	icon.floatTimer += 0.05f;
	const float floatOffset = std::sin(icon.floatTimer) * 6.0f;
	icon.drawPos = { icon.basePos.x, icon.basePos.y + floatOffset };

	icon.hover = IsMouseOverIcon(icon, mousePos);
	const float targetScale = icon.hover ? 1.18f : 1.0f;
	icon.scale += (targetScale - icon.scale) * 0.22f;
}

void StageSelectScene::DrawIcon(const StageIcon& icon)
{
	if (!icon.active || !icon.tex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		icon.tex.get(),
		static_cast<int>(icon.drawPos.x),
		static_cast<int>(icon.drawPos.y),
		static_cast<int>(icon.size.x * icon.scale),
		static_cast<int>(icon.size.y * icon.scale));
}
