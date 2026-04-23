#include "GameScene.h"

void GameScene::PreUpate()
{	
	KdDebugGUI::Instance().AddLog("PreUpdate()\n");
}

void GameScene::Upate()
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
