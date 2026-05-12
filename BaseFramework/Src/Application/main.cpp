#include "main.h"

#include "Application/Object/BlackHole.h"
#include "Application/Object/CollisionSystem.h"
#include "Application/Object/EnergySystem.h"
#include "Application/Object/EnemyManager.h"
#include "Application/Object/ExplosionManager.h"
#include "Application/Object/HeatRay.h"
#include "Application/Object/PlayerPlanet.h"
#include "Application/Object/ProgressSystem.h"
#include "Application/Object/ProjectileManager.h"
#include "Application/Object/Turret.h"
#include "Application/Object/UIManager.h"
#include "Application/Object/WeaponSystem.h"
#include "Application/Scene/SceneManager.h"

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// エントリーポイント
// アプリケーションはこの関数から進行する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR , _In_ int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		CoUninitialize();

		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行]
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}
// アプリケーション更新開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginUpdate()
{
	// 入力状況の更新
	KdInputManager::Instance().Update();

	// 空間環境の更新
	KdShaderManager::Instance().WorkAmbientController().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostUpdate()
{
	// 3DSoundListnerの行列を更新
	KdAudioManager::Instance().SetListnerMatrix(KdShaderManager::Instance().GetCameraCB().mView.Invert());
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreUpdate()
{
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Update()
{
	if (m_sceneManager)
	{
		const bool shouldShowCursor = !m_sceneManager->IsGameScene();
		SyncCursorVisibility(shouldShowCursor);
		m_sceneManager->Update();
		if (m_sceneManager->IsGameScene() && !m_isStageRunning)
		{
			ResetStageState();
			m_sceneManager->ResetResultStats();
			m_isStageRunning = true;
			m_isResultWaiting = false;
			m_resultWaitFrame = 0.0f;
		}
		else if (!m_sceneManager->IsGameScene() && !m_sceneManager->IsPauseScene())
		{
			m_isStageRunning = false;
		}
		if (!m_sceneManager->IsGameScene())
		{
			return;
		}
	}

	if (m_sceneManager && !m_isResultWaiting)
	{
		m_sceneManager->AddResultTime(Application::Instance().GetDeltaTime() / 60.0f);
	}

	if (m_playerPlanet && (GetAsyncKeyState('O') & 0x8000))
	{
		m_playerPlanet->ResetHp();
		if (m_turret)
		{
			m_turret->ResetDestruction();
		}
	}

	if (m_turret)
	{
		m_turret->Update();
	}

	if (m_weaponSystem)
	{
		m_weaponSystem->Update();
	}

	if (m_blackHole)
	{
		m_blackHole->Update();
	}

	if (m_progressSystem)
	{
		m_progressSystem->Update();
	}

	if (m_enemyManager && m_playerPlanet)
	{
		m_enemyManager->Update(*m_playerPlanet);
	}

	if (m_energySystem)
	{
		m_energySystem->Update();
	}

	if (m_collisionSystem && m_projectileManager && m_enemyManager && m_playerPlanet && m_turret && m_energySystem && m_explosionManager)
	{
		m_collisionSystem->Update(*m_projectileManager, *m_enemyManager, *m_playerPlanet, *m_turret, *m_energySystem, *m_explosionManager);
	}

	if (m_explosionManager)
	{
		m_explosionManager->Update();
	}

	if (m_uiManager)
	{
		m_uiManager->Update();
	}

	const bool isStageClear = m_progressSystem && m_progressSystem->IsComplete();
	const bool isPlayerDead = m_playerPlanet && m_playerPlanet->GetHp() <= 0;
	if (isPlayerDead && m_turret)
	{
		m_turret->BeginDeathExplosion();
	}
	if (m_sceneManager && (isStageClear || isPlayerDead))
	{
		m_isResultWaiting = true;
		m_resultWaitFrame += Application::Instance().GetDeltaTime();
		if (m_resultWaitFrame >= m_resultWaitDuration)
		{
			m_sceneManager->StartResultScene();
		}
	}
}

void Application::AddResultKill(int value)
{
	if (m_sceneManager && m_isStageRunning && !m_isResultWaiting)
	{
		m_sceneManager->AddResultKill(value);
	}
}

void Application::AddResultMiss(int value)
{
	if (m_sceneManager && m_isStageRunning && !m_isResultWaiting)
	{
		m_sceneManager->AddResultMiss(value);
	}
}

void Application::AddResultDamage(float value)
{
	if (m_sceneManager && m_isStageRunning && !m_isResultWaiting)
	{
		m_sceneManager->AddResultDamage(value);
	}
}

void Application::ResetStageState()
{
	if (m_playerPlanet)
	{
		m_playerPlanet->ResetHp();
	}

	if (m_progressSystem)
	{
		m_progressSystem->Reset();
	}

	if (m_enemyManager)
	{
		if (m_sceneManager)
		{
			m_enemyManager->SetStageNo(m_sceneManager->GetSelectedStageNo());
		}
		m_enemyManager->Reset();
	}

	if (m_projectileManager)
	{
		m_projectileManager->WorkProjectiles().clear();
	}

	if (m_energySystem)
	{
		m_energySystem->Reset();
	}

	if (m_explosionManager)
	{
		m_explosionManager->Reset();
	}

	if (m_heatRay)
	{
		m_heatRay->Reset();
	}

	if (m_weaponSystem)
	{
		m_weaponSystem->Reset();
		if (m_sceneManager)
		{
			m_weaponSystem->SetStageNo(m_sceneManager->GetSelectedStageNo());
		}
	}

	if (m_blackHole)
	{
		m_blackHole->Reset();
		if (m_sceneManager)
		{
			m_blackHole->SetStageNo(m_sceneManager->GetSelectedStageNo());
		}
	}

	if (m_uiManager && m_sceneManager)
	{
		m_uiManager->SetStageNo(m_sceneManager->GetSelectedStageNo());
	}

	if (m_turret)
	{
		m_turret->ResetDestruction();
	}
}

void Application::SyncCursorVisibility(bool visible)
{
	if (m_isCursorVisible == visible)
	{
		return;
	}

	if (visible)
	{
		while (ShowCursor(TRUE) < 0) {}
	}
	else
	{
		while (ShowCursor(FALSE) >= 0) {}
	}

	m_isCursorVisible = visible;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostUpdate()
{
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginDraw(bool usePostProcess)
{
	KdDirect3D::Instance().ClearBackBuffer();

	KdShaderManager::Instance().WorkAmbientController().Draw();

	if (!usePostProcess) return;
	KdShaderManager::Instance().m_postProcessShader.Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostDraw()
{
	// Imguiのレンダリング
	KdDebugGUI::Instance().GuiProcess();

	// BackBuffer -> 画面表示
	KdDirect3D::Instance().WorkSwapChain()->Present(0, 0);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreDraw()
{
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Draw()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(透明な部分を含む物体やエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostDraw()
{
	// 画面のぼかしや被写界深度処理の実施
	KdShaderManager::Instance().m_postProcessShader.PostEffectProcess();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// デバッグ情報の描画はこの間で行う
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 2Dスプライトの描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::DrawSprite()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 2Dの描画はこの間で行う
	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		if (m_sceneManager && !m_sceneManager->IsGameScene())
		{
			m_sceneManager->DrawSprite();
			KdShaderManager::Instance().m_spriteShader.End();
			return;
		}

		if (m_backgroundTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_backgroundTex.get(),
				0,
				0,
				1280,
				720);
		}

		if (m_playerPlanet)
		{
			m_playerPlanet->DrawSprite();
		}

		if (m_turret)
		{
			m_turret->DrawSprite();
		}

		if (m_enemyManager)
		{
			m_enemyManager->DrawSprite();
		}

		if (m_projectileManager)
		{
			m_projectileManager->DrawSprite();
		}

		if (m_explosionManager)
		{
			m_explosionManager->DrawSprite();
		}

		if (m_heatRay)
		{
			m_heatRay->DrawSprite();
		}

		if (m_blackHole)
		{
			m_blackHole->DrawSprite();
		}

		if (m_uiManager)
		{
			m_uiManager->DrawSprite();
		}

		if (m_progressSystem)
		{
			m_progressSystem->DrawSprite();
		}
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション初期設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "Galactic Crisis", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
//	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
//		bFullScreen = true;
//	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif

	// Direct3D初期化
	std::string errorMsg;
	if (KdDirect3D::Instance().Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		HRESULT hr;

		hr = KdDirect3D::Instance().SetFullscreenState(TRUE, 0);
		if (FAILED(hr))
		{
			MessageBoxA(m_window.GetWndHandle(), "フルスクリーン設定失敗", "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	//===================================================================
	// imgui初期化
	//===================================================================
	KdDebugGUI::Instance().GuiInit(w, h);

	//===================================================================
	// シェーダー初期化
	//===================================================================
	KdShaderManager::Instance().Init();

	//===================================================================
	// オーディオ初期化
	//===================================================================
	KdAudioManager::Instance().Init();

	//===================================================================
	// フォント初期化
	//===================================================================
	KdFontManager::Instance().Init(GetWindowHandle());
	KdFontManager::Instance().AddFont(0, "Arial", 32);
	
	//===================================================================
	// ゲーム固有の初期化
	//===================================================================
	// 例えばカーソルを消したい場合
	SyncCursorVisibility(true);

	m_backgroundTex = std::make_shared<KdTexture>();
	m_backgroundTex->Load("Asset/Textures/yuzhoubeijing.png");

	m_playerPlanet = std::make_shared<PlayerPlanet>();
	m_playerPlanet->Init();
	m_playerPlanet->SetPos({ 0.0f, -810.0f });

	m_turret = std::make_shared<Turret>();
	m_turret->Init();
	m_turret->SetPos({ 0.0f, -232.0f });

	m_projectileManager = std::make_shared<ProjectileManager>();
	m_projectileManager->Init();

	m_progressSystem = std::make_shared<ProgressSystem>();
	m_progressSystem->Init();

	m_enemyManager = std::make_shared<EnemyManager>();
	m_enemyManager->Init();
	m_enemyManager->SetProgressSystem(m_progressSystem);

	m_collisionSystem = std::make_shared<CollisionSystem>();

	m_energySystem = std::make_shared<EnergySystem>();
	m_energySystem->Init();

	m_explosionManager = std::make_shared<ExplosionManager>();
	m_explosionManager->Init();

	m_heatRay = std::make_shared<HeatRay>();
	m_heatRay->Init(m_turret, m_enemyManager, m_energySystem, m_explosionManager);

	m_weaponSystem = std::make_shared<WeaponSystem>();
	m_weaponSystem->Init(m_turret, m_projectileManager, m_heatRay);

	m_uiManager = std::make_shared<UIManager>();
	m_uiManager->Init(m_playerPlanet, m_energySystem, m_weaponSystem, m_heatRay);

	m_blackHole = std::make_shared<BlackHole>();
	m_blackHole->Init(m_energySystem, m_uiManager, m_enemyManager, m_projectileManager);

	m_sceneManager = std::make_shared<SceneManager>();
	m_sceneManager->Init();

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション実行
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Execute()
{
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);

	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (Application::Instance().Init(atoi(sizeData[0].c_str()), atoi(sizeData[1].c_str())) == false) {
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	m_fpsController.Init();

	// ループ
	while (1)
	{
		// 処理開始時間Get
		m_fpsController.UpdateStartTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE) && (!m_sceneManager || m_sceneManager->IsTitleScene()))
		{
//			if (MessageBoxA(m_window.GetWndHandle(), "本当にゲームを終了しますか？",
//				"終了確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
			{
				End();
			}
		}

		//=========================================
		//
		// アプリケーション更新処理
		//
		//=========================================

		KdBeginUpdate();
		{
			PreUpdate();

			Update();

			PostUpdate();
		}
		KdPostUpdate();

		//=========================================
		//
		// アプリケーション描画処理
		//
		//=========================================

		KdBeginDraw();
		{
			PreDraw();

			Draw();

			PostDraw();

			DrawSprite();
		}
		KdPostDraw();

		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		m_fpsController.Update();
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}

// アプリケーション終了
void Application::Release()
{
	m_sceneManager = nullptr;
	m_uiManager = nullptr;
	m_blackHole = nullptr;
	m_progressSystem = nullptr;
	m_explosionManager = nullptr;
	m_energySystem = nullptr;
	m_backgroundTex = nullptr;
	m_collisionSystem = nullptr;
	m_enemyManager = nullptr;
	m_weaponSystem = nullptr;
	m_heatRay = nullptr;
	m_projectileManager = nullptr;
	m_turret = nullptr;
	m_playerPlanet = nullptr;

	KdInputManager::Instance().Release();

	KdShaderManager::Instance().Release();

	KdAudioManager::Instance().Release();

	KdDirect3D::Instance().Release();

	// ウィンドウ削除
	m_window.Release();
}
