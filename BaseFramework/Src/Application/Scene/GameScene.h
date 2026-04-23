#pragma once

class GameScene
{
	//public: 哪里都能akuess
public:
	GameScene(){}//生成时调用
	~GameScene() {}//销毁时调用

	void PreUpate();//更新前处理
	void Upate();//更新
	void Draw();//绘制
	void Init();//初始化
	void Release();//释放 解放处理
	//private: 只有类内能访问

private:
	int test = 1;
};
