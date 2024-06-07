#pragma once
#include "Camera/Camera.h"
#include "Math/MathCommon.h"
#include "Utils/SafePtr.h"

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <type_traits>
#include <string>

/// 1シーンの流れ
/// ロード
/// 初期化
/// 更新
/// 状態に応じてシーン変更
/// 描画
/// シーンの変更が行われたら終了処理

/// <summary>
/// 1fの流れ
/// <para>1 : 最初に行う更新処理</para>
/// <para>2 : 移動</para>
/// <para>(3 : 当たり判定)</para>
/// <para>4 : 移動や当たり判定によるイベント</para>
/// <para>5 : 更新処理</para>
/// <para>6 : 最後に行う更新処理</para>
/// </summary>
class GameFlow {
public:
	virtual ~GameFlow() = default;

	virtual void FirstUpdate() {}
	virtual void Move() {}

	virtual void Event() {}
	virtual void Update() {}
	virtual void LastUpdate() {}
};

// 前方宣言
class Object;

class IComp : public GameFlow {
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

	virtual void FirstUpdate() override {}
	virtual void Move() override {}
	virtual void Event() override {}
	virtual void Update() override {}
	virtual void LastUpdate() override {}

	virtual void Draw() {}

protected:
	Object& object_;
	float32_t deltatime_;
};

template<class T>
concept IsBaseIComp = std::is_base_of_v<IComp, T>;

class Object : public GameFlow {
public:
	Object() = default;
	virtual ~Object() = default;

public:
	virtual void Init();

	virtual void FirstUpdate() override;
	virtual void Move() override;
	virtual void Event() override;
	virtual void Update() override;
	virtual void LastUpdate() override;

	virtual void Draw() const;


public:
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

	template<IsBaseIComp CompType>
	CompType* const AddComp() {
		auto&& key = std::string(typeid(CompType).name());
		bool isExist = components_.contains(key);

		if (not isExist) {
			components_[key] = std::make_unique<CompType>(this);
			components_[key]->Init();
		}

		return static_cast<CompType*>(components_.at(key).get());
	}

	template<IsBaseIComp CompType>
	CompType* const GetComp() const {
		auto&& key = std::string(typeid(CompType).name());
		bool isExist = components_.contains(key);

		if (not isExist) {
			throw Lamb::Error::Code<Object>("This comp is not add", ErrorPlace);
		}
		else {
			return static_cast<CompType*>(components_.at(key).get());
		}
	}

	void SetCamera(Camera* const camera) {
		camera_ = camera;
	}

	const Camera* const GetCamera() const {
		return camera_.get();
	}

protected:
	std::unordered_map<std::string, std::unique_ptr<IComp>> components_;
	std::unordered_set<std::string> tags_;

	Lamb::SafePtr<Camera> camera_;
};