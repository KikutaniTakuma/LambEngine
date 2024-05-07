#pragma once
#include "Drawers/PeraRender/PeraRender.h"
#include "Utils/SafePtr/SafePtr.h"
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
		if (not gray_) {
			throw Lamb::Error::Code<PostEffectManager>("peraCamera is nullptr", __func__, __FILE__, __LINE__);
		}
		return *camera_;
	}

private:
	std::unique_ptr<Camera> camera_;

public:
	PeraRender& GetGrayPera() {
		if (not gray_) {
			throw Lamb::Error::Code<PostEffectManager>("Gray pera is nullptr", __func__, __FILE__, __LINE__);
		}
		return *gray_;
	}

private:
	std::unique_ptr<PeraRender> gray_;

};