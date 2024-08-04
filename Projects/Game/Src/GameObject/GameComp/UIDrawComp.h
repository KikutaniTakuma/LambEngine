#include "../Object.h"
class UIDrawComp : public IComp {
public:
	using IComp::IComp;

	~UIDrawComp() = default;

public:
	void Init() override;

	void Event() override;

	void Draw(CameraComp* cameraComp) override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;


private:
	Lamb::SafePtr<class Texture2D> tex2D_;
	Lamb::SafePtr<class SpriteRenderDataComp> renderDataComp_;
	Lamb::SafePtr<class TransformComp> transformComp_;
};
