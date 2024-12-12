#pragma once
#include "Utils/Flg.h"
#include "./Camera/Camera.h"
#include <memory>

/// <summary>
/// そのシーン中にさらに別のシーンを処理するためのクラス(ポーズ画面など)
/// </summary>
class SubMenu {
public:
	SubMenu();
	~SubMenu() = default;

	SubMenu(const SubMenu&) = delete;
	SubMenu(SubMenu&&) = delete;
	SubMenu& operator=(const SubMenu&) = delete;
	SubMenu& operator=(SubMenu&&) = delete;

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	void ActiveUpdate();
	virtual void Draw() = 0;

protected:
	virtual void Update() = 0;

protected:
	Lamb::Flg isActive_;

protected:
	std::unique_ptr<Camera> camera_;
};