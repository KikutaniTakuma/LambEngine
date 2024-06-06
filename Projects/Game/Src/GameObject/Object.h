#pragma once
#include "Camera/Camera.h"
#include "Math/MathCommon.h"
#include "Transform/Transform.h"

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <type_traits>
#include <string>

class Object;

class IComp {
public:
	IComp(Object* const object) :
		object_(*object),
		deltatime_(0.0_f32)
	{}
	virtual ~IComp() = default;
	IComp() = delete;
	IComp(const IComp&) = delete;
	IComp(IComp&&) = delete;

	IComp& operator=(const IComp&) = delete;
	IComp& operator=(IComp&&) = delete;

public:
	virtual void Init() {}
	virtual void Finalize() {}

	void SetDeltaTime(float32_t deltatime) {
		deltatime_ = deltatime;
	}

	virtual void Update0() {}
	virtual void Update1() {}
	virtual void Update2() {}
	virtual void Update3() {}
	virtual void Update4() {}

	virtual void Draw() {}

protected:
	Object& object_;
	float32_t deltatime_;
};

template<class T>
concept IsBaseIComp = std::is_base_of_v<IComp, T>;

class Object {
public:
	Object() = default;
	virtual ~Object() = default;

public:
	virtual void Init();

	virtual void Update();

	virtual void Draw() const;

	void SetTag(const std::string& tag) {
		tags_.insert(tag);
	}

	bool HasTag(const std::string& tag) const {
		return tags_.contains(tag);
	}

	void EraseTag(const std::string& tag) {
		if (this->HasTag(tag)) {
			tags_.erase(tag);
		}
	}

	template<IsBaseIComp Comp>
	Comp* const AddComp() {
		auto&& key = std::string(typeid(Comp).name());
		auto isExist = components_.find(key);

		if (isExist == components_.end()) {
			components_[key] = std::make_unique<Comp>(this);
			components_[key]->Init();
		}

		return static_cast<Comp*>(components_[key].get());
	}

	template<IsBaseIComp Comp>
	Comp* const GetComp() const {
		auto&& key = std::string(typeid(Comp).name());
		auto isExist = components_.find(key);

		if (isExist == components_.end()) {
			throw Lamb::Error::Code<Object>("This comp is not add", ErrorPlace);
		}
		else {
			return static_cast<Comp*>(isExist->second.get());
		}
	}

	void SetCamera(Camera* const camera) {
		camera_ = camera;
	}

	const Camera* const GetCamera() const {
		return camera_;
	}

protected:
	std::unordered_map<std::string, std::unique_ptr<IComp>> components_;
	std::unordered_set<std::string> tags_;

	class Camera* camera_;
};