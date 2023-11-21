#include "Framework.h"
#include "Engine/Engine.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"
#include "Utils/Math/Vector2.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Input/Input.h"

void Framework::Initialize() {
	// ライブラリ初期化
	if (!Engine::Initialize("DirectXGame", Vector2{ 1280.0f, 720.0f })) {
		ErrorCheck::GetInstance()->ErrorTextBox(
			"Engine::Initialize() Failed",
			"WinMain"
		);
		return;
	}
}

void Framework::Finalize() {
	// ライブラリ終了
	Engine::Finalize();
}

void Framework::Execution() {
	Initialize();

	FrameInfo* const frameInfo = FrameInfo::GetInstance();
	Input* const input = Input::GetInstance();

	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		frameInfo->Debug();

		// 入力処理
		input->InputStart();

#ifdef _DEBUG		
		if (frameInfo->GetIsDebugStop() && frameInfo->GetIsOneFrameActive()) {
			this->Update();
			frameInfo->SetIsOneFrameActive(false);
		}
		else if (!frameInfo->GetIsDebugStop()) {
			// 更新処理
			this->Update();
		}
#else
		// 更新処理
		this->Update();
#endif
		// 終了リクエストが来たら終わり
		if (isEnd_) {
			break;
		}

		// 描画処理
		this->Draw();

		// フレーム終了処理
		Engine::FrameEnd();
	}

	Finalize();
}