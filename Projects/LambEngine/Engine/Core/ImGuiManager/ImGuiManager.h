/// ================================
/// ==  ImGuiManagerクラスの宣言  ==
/// ================================


#pragma once
#include "Utils/SafePtr.h"

class ImGuiManager {
private:
	ImGuiManager();
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager(ImGuiManager&&) = delete;

	ImGuiManager& operator=(const ImGuiManager&) = delete;
	ImGuiManager& operator=(ImGuiManager&&) = delete;
public:
	~ImGuiManager();

public:
	static ImGuiManager* const GetInstance();
	static void Initialize();
	static void Finalize();

private:
	static Lamb::SafePtr<ImGuiManager> pInstance_;

public:
	void Start();

	void End();
};