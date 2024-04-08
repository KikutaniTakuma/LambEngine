#pragma once
#include "../GraphicsStructs.h"

#include <unordered_map>
#include <memory>

class AnimationManager {
private:
	AnimationManager() = default;
	
	AnimationManager(const AnimationManager&);
	AnimationManager(AnimationManager&&);
	
	AnimationManager& operator=(const AnimationManager&) = delete;
	AnimationManager& operator=(AnimationManager&&) = delete;

public:
	~AnimationManager() = default;

public:
	static void Initialize();

	static void Finalize();

 	static [[nodiscard]] AnimationManager* const GetInstance();

private:
	static AnimationManager* instance_;

public:
	[[noreturn]] void LoadAniamtions(const std::string& fileName);
	
	[[nodiscard]] Animations* const GetAnimations(const std::string& fileName);

private:
	std::unordered_map<std::string, std::unique_ptr<Animations>> animationData_;
};