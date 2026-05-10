#pragma once

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End()							{ m_endFlag = true; }

	HWND GetWindowHandle()		const	{ return m_window.GetWndHandle(); }
	int GetMouseWheelValue()	const	{ return m_window.GetMouseWheelVal(); }

	int		GetNowFPS()			const	{ return m_fpsController.m_nowfps; }
	int		GetMaxFPS()			const	{ return m_fpsController.m_maxFps; }
	float	GetDeltaTime()		const	{ return m_fpsController.GetDeltaTime(); }
private:

	void KdBeginUpdate();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void KdPostUpdate();

	void KdBeginDraw(bool usePostProcess = true);
	void PreDraw();
	void Draw();
	void PostDraw();
	void DrawSprite();
	void KdPostDraw();

	// アプリケーション初期化
	bool Init(int w, int h);

	// アプリケーション解放
	void Release();

	// ゲームウィンドウクラス
	KdWindow		m_window;

	// FPSコントローラー
	KdFPSController	m_fpsController;

	std::shared_ptr<class PlayerPlanet> m_playerPlanet = nullptr;
	std::shared_ptr<class Turret> m_turret = nullptr;
	std::shared_ptr<class ProjectileManager> m_projectileManager = nullptr;
	std::shared_ptr<class HeatRay> m_heatRay = nullptr;
	std::shared_ptr<class WeaponSystem> m_weaponSystem = nullptr;
	std::shared_ptr<class BlackHole> m_blackHole = nullptr;
	std::shared_ptr<class EnemyManager> m_enemyManager = nullptr;
	std::shared_ptr<class CollisionSystem> m_collisionSystem = nullptr;
	std::shared_ptr<class EnergySystem> m_energySystem = nullptr;
	std::shared_ptr<class ExplosionManager> m_explosionManager = nullptr;
	std::shared_ptr<class UIManager> m_uiManager = nullptr;
	std::shared_ptr<KdTexture> m_backgroundTex = nullptr;

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;

//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Application() {}

public:
	static Application &Instance(){
		static Application Instance;
		return Instance;
	}
};
