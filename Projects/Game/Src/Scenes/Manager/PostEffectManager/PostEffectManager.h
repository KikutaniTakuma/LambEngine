#pragma once
#include "Drawers/PeraRender/PeraRender.h"
#include "Utils/SafePtr.h"
#include <memory>
#include "Camera/Camera.h"

class PostEffectManager {
private:
	PostEffectManager();
	PostEffectManager(const PostEffectManager&) = delete;
	PostEffectManager(PostEffectManager&&) = delete;

public:
	~PostEffectManager() = default;

private:
	PostEffectManager& operator=(const PostEffectManager&) = delete;
	PostEffectManager& operator=(PostEffectManager&&) = delete;

public:
	static PostEffectManager* const GetInstance() {
		return instance_.get();
	}

	static void Initialize();

	static void Finalize();

private:
	static Lamb::SafePtr<PostEffectManager> instance_;

	
public:
	const Camera& GetPeraCamera() const {
		if (not pera_) {
			throw Lamb::Error::Code<PostEffectManager>("peraCamera is nullptr", ErrorPlace);
		}
		return *camera_;
	}

private:
	std::unique_ptr<Camera> camera_;

public:
	PeraRender& GetPera() {
		if (not pera_) {
			throw Lamb::Error::Code<PostEffectManager>("Gray pera is nullptr", ErrorPlace);
		}
		return *pera_;
	}

private:
	std::unique_ptr<PeraRender> pera_;

};