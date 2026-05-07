#include "GameScene.h"

void GameScene::PreUpdate()
{	
	KdDebugGUI::Instance().AddLog("PreUpdate()\n");
}

void GameScene::Update()
{
	KdDebugGUI::Instance().AddLog("Upate()\n");
}

void GameScene::Draw()
{
	KdDebugGUI::Instance().AddLog("Draw()\n");
}

void GameScene::Init()
{
	KdDebugGUI::Instance().AddLog("Init()\n");
}

void GameScene::Release()
{
	KdDebugGUI::Instance().AddLog("Release()\n");
}
