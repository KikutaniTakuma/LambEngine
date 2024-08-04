#pragma once
#include "Math/MathCommon.h"
#include "Utils/SafePtr.h"
#include "Utils/Flg.h"
#include "Math/Mat4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <type_traits>
#include <string>
#include "json.hpp"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


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


/// 最小構成のコンポーネント
/// #include "../Object.h"
/// class DerivedComp : public IComp {
/// public:
/// 	using IComp::IComp;
/// 
/// 	~DerivedComp() = default;
///		
/// public:
///		void Save(nlohmann::json& json) override;
///		void Load(nlohmann::json& json) override;
/// };

// 前方宣言
class Object;
class CameraComp; 
class IComp;

template<class T>
concept IsBaseIComp = std::is_base_of_v<IComp, T>;

class IComp : public GameFlow {
public:
	IComp(Object* const object) :
		object_(*object)
	{}
	virtual ~IComp() = default;
	IComp() = delete;
	IComp(const IComp&) = delete;
	IComp(IComp&&) = delete;

	IComp& operator=(const IComp&) = delete;
	IComp& operator=(IComp&&) = delete;

public:
	virtual void Load() {}

	virtual void Init() {}
	virtual void Finalize() {}


	virtual void FirstUpdate() override {}
	virtual void Move() override {}
	virtual void Event() override {}
	virtual void Update() override {}
	virtual void LastUpdate() override {}

	virtual void Draw([[maybe_unused]] CameraComp*) {}

	virtual void Debug([[maybe_unused]]const std::string& guiName) {};

	virtual void Save(nlohmann::json& json) = 0;
	virtual void Load(nlohmann::json& json) = 0;

	template<IsBaseIComp Comp>
	void SetCompName(nlohmann::json& json) {
		json["CompName"] = typeid(Comp).name();
	}


public:
	const Object& getObject() const {
		return object_;
	}

protected:
	Object& object_;
};

#ifndef SaveCompName
#define SaveCompName(json) IComp::SetCompName<std::remove_reference_t<decltype(*this)>>(json)
#endif // !SaveCompName


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

	virtual void Draw([[maybe_unused]] CameraComp* cameraComp) const;

	virtual void Debug(const std::string& guiName);

	bool DebugAddComp();

	void Save(nlohmann::json& json);
	void Load(nlohmann::json& compData);

private:
	template<IsBaseIComp Comp>
	void DebugAdd() {
#ifdef _DEBUG
		if (ImGui::Button((std::string("Add ") + typeid(Comp).name()).c_str())) {
			AddComp<Comp>();
		}
#endif // _DEBUG
	}

public:
	void SetDeltaTime(float32_t deltatime) {
		deltatime_ = deltatime;
	}

	float32_t GetDeltaTime() const {
		return deltatime_;
	}

public:
	void SetTag(const std::string& tag) {
		tags_.insert(tag);
	}

	[[nodiscard]] bool HasTag(const std::string& tag) const {
		return tags_.contains(tag);
	}
	template<IsBaseIComp Name>
	[[nodiscard]] bool HasTag(const std::string& tag) const {
		return tags_.contains(typeid(Name).name());
	}

	void EraseTag(const std::string& tag) {
		if (this->HasTag(tag)) {
			tags_.erase(tag);
		}
	}

	const std::unordered_set<std::string>& GetTags() const {
		return tags_;
	}

	template<IsBaseIComp CompType>
	CompType* const AddComp() {
		auto&& key = std::string(typeid(CompType).name());
		tags_.insert(key);
		bool isExist = components_.contains(key);

		if (not isExist) {
			components_[key] = std::make_unique<CompType>(this);
			components_[key]->Init();
		}

		return static_cast<CompType*>(components_.at(key).get());
	}

	void AddComps(nlohmann::json& compData);

	template<IsBaseIComp CompType>
	void AddAndLoadComp(const std::string& className, nlohmann::json& compData) {
		if (className == typeid(CompType).name()) {
			auto comp = AddComp<CompType>();
			comp->Load(compData);
		}
	}


	template<IsBaseIComp CompType>
	[[nodiscard]]CompType* const GetComp() const {
		auto&& key = std::string(typeid(CompType).name());
		bool isExist = components_.contains(key);

		if (not isExist) {
			throw Lamb::Error::Code<Object>("This comp is not add", ErrorPlace);
		}
		else {
			return static_cast<CompType*>(components_.at(key).get());
		}
	}

	template<IsBaseIComp CompType>
	[[nodiscard]] bool HasComp() const {
		auto&& key = std::string(typeid(CompType).name());
		return components_.contains(key);
	}

	const std::string& GetObjectName() const {
		return objectName_;
	}

	void SetObjectName(const std::string& objectName) {
		objectName_ = objectName;
	}

protected:
	std::unordered_map<std::string, std::unique_ptr<IComp>> components_;
	std::unordered_set<std::string> tags_;
	std::string objectName_;
	float32_t deltatime_ = 0.0_f32;
};