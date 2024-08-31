#include "BaseScene.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


std::array<std::string, static_cast<uint32_t>(BaseScene::ID::kNum)> BaseScene::kSceneStrings = {
	"Title",
	"Game",
	"StageSelect",
	"Result"
};

BaseScene::BaseScene(BaseScene::ID sceneID) :
	sceneManager_(nullptr),
	drawerManager_(nullptr),
	audioManager_(nullptr),
	frameInfo_(nullptr),
	input_(nullptr),
	stringOutPutManager_(nullptr),
	sceneID_(sceneID),
	camera_(std::make_unique<Camera>()),
	currentCamera_(camera_.get())
{}

void BaseScene::SceneInitialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	drawerManager_ = DrawerManager::GetInstance();

	audioManager_ = AudioManager::GetInstance();

	frameInfo_ = FrameInfo::GetInstance();

	stringOutPutManager_ = StringOutPutManager::GetInstance();

	input_ = Input::GetInstance();

	animationManager_ = AnimationManager::GetInstance();

	renderingManager_ = RenderingManager::GetInstance();
}

void BaseScene::ChangeCamera()
{

}

const Camera& BaseScene::GetCamera() const
{
	return *camera_;
}
