#include "LevelLoader.h"
#include "GameObject/Comp/ModelRenderComp.h"
#include "GameObject/Comp/TransformComp.h"
#include "GameObject/Comp/ObbComp.h"
#include "GameObject/Comp/Camera2DComp.h"
#include "GameObject/Comp/Camera3DComp.h"

LevelData* LevelLoader::Load(const std::string& fileName)
{
    // jsonをロード
    auto data = Lamb::LoadJson(fileName);

    // シーンの名前を取得
    std::string name = data["name"].get<std::string>();

    // これがsceneのデータか
    if (name.compare("scene") != 0) {
        throw Lamb::Error::Code<Lamb::Error::Function>("this file is invalid" + fileName, ErrorPlace);
    }

    Lamb::SafePtr levelData = Lamb::MakeSafePtr<LevelData>();

    AddObjects(data, levelData);

    return levelData.get();
}

void LevelLoader::AddObjects(nlohmann::json& data, Lamb::SafePtr<LevelData> levelData)
{
    // 全オブジェクトを走査
    for (nlohmann::json& objectData : data["objects"]) {
        // オブジェクトを追加
        levelData->objects.emplace_back(std::make_unique<Object>());
        Object& object = *levelData->objects.back();

        // オブジェクトの名前を設定
        if (objectData.contains("name")) {
            object.SetObjectName(objectData["name"].get<std::string>());
        }

        // タイプがあるかどうか確認
        if (objectData.contains("type")) {
            // タイプの文字列取得
            std::string type = objectData["type"].get<std::string>();

            // タイプがメッシュだったら
            if (type.compare("MESH") == 0) {
                if (objectData.contains("file_name")) {
                    Lamb::SafePtr model = object.AddComp<ModelRenderComp>();
                    model->SetFileNmae(objectData["file_name"]);
                    object.SetTag("Model");
                }
            }
            // カメラだったら
            if (type.compare("CAMERA") == 0) {
                AddCamera(objectData, object);
            }
        }
        // コライダーがあるなら
        if (objectData.contains("collider")) {
            AddObb(objectData, object);
        }
        // Transformがあるなら
        if (objectData.contains("transform")) {
            AddTransform(objectData, object);
        }
        // childrenがあるなら
        if (objectData.contains("transform")) {
            AddChildren(objectData, levelData, object);
        }
    }
}

void LevelLoader::AddTransform(nlohmann::json& data, Object& object)
{
    Lamb::SafePtr transformComp = object.AddComp<TransformComp>();
    Transform transform{};

    nlohmann::json& transformData = data["transform"];

    for (size_t i = 0; i < transform.translate.size(); i++) {
        transform.translate[i] = static_cast<float>(transformData["translation"][i]);
    }
    for (size_t i = 0; i < transform.rotate.size(); i++) {
        transform.rotate[i] = static_cast<float>(transformData["rotation"][i]);
    }
    for (size_t i = 0; i < transform.scale.size(); i++) {
        transform.scale[i] = static_cast<float>(transformData["scaling"][i]);
    }

    transformComp->translate = transform.translate;
    transformComp->rotate = Quaternion::EulerToQuaternion(transform.rotate);
    transformComp->scale = transform.scale;

    object.SetTag("transform");
}

void LevelLoader::AddCamera(nlohmann::json& data, Object& object)
{

    std::string cameratype = data["camera_type"].get<std::string>();

    if (cameratype.compare("Perspective") == 0) {
        Lamb::SafePtr cameraComp = object.AddComp<Camera3DComp>();
        cameraComp->SetFov(static_cast<float32_t>(data["fov"]));
        cameraComp->SetAspectRatio(static_cast<float32_t>(data["aspect_ratio"]));
        cameraComp->SetFarClip(static_cast<float32_t>(data["far_clip"]));
        cameraComp->SetNearClip(static_cast<float32_t>(data["near_clip"]));
    }
    else if (cameratype.compare("Othographic") == 0) {
        Lamb::SafePtr cameraComp = object.AddComp<Camera2DComp>();

        cameraComp->SetWidth(static_cast<float32_t>(data["width"]));
        cameraComp->SetHeight(static_cast<float32_t>(data["height"]));
        cameraComp->SetFarClip(static_cast<float32_t>(data["far_clip"]));
        cameraComp->SetNearClip(static_cast<float32_t>(data["near_clip"]));
    }
}

void LevelLoader::AddObb(nlohmann::json& data, Object& object)
{
    Lamb::SafePtr obbComp = object.AddComp<ObbComp>();

    nlohmann::json& colliderData = data["collider"];

    for (size_t i = 0; i < obbComp->center.size(); i++) {
        obbComp->center[i] = static_cast<float>(colliderData["center"][i]);
    }
    for (size_t i = 0; i < obbComp->scale.size(); i++) {
        obbComp->scale[i] = static_cast<float>(colliderData["size"][i]);
    }

    object.SetTag("collider");
}

void LevelLoader::AddChildren(nlohmann::json& data, Lamb::SafePtr<LevelData> levelData, Object& parent)
{
    // 全childlenを走査
    for (nlohmann::json& objectData : data["children"]) {
        // オブジェクトを追加
        levelData->objects.emplace_back(std::make_unique<Object>());
        Object& object = *levelData->objects.back();

        // オブジェクトの名前を設定
        if (objectData.contains("name")) {
            object.SetObjectName(objectData["name"].get<std::string>());
        }

        // タイプがあるかどうか確認
        if (objectData.contains("type")) {
            // タイプの文字列取得
            std::string type = objectData["type"].get<std::string>();

            // タイプがメッシュだったら
            if (type.compare("MESH") == 0) {
                if (objectData.contains("file_name")) {
                    Lamb::SafePtr model = object.AddComp<ModelRenderComp>();
                    model->SetFileNmae(objectData["file_name"]);
                    object.SetTag("Model");
                }
            }
            // カメラだったら
            if (type.compare("CAMERA") == 0) {
                AddCamera(objectData, object);
            }
        }
        // コライダーがあるなら
        if (objectData.contains("collider")) {
            AddObb(objectData, object);
        }
        // Transformがあるなら
        if (objectData.contains("transform")) {
            AddTransform(objectData, object);

            // 親をセット
            Lamb::SafePtr transform = object.GetComp<TransformComp>();
            Lamb::SafePtr parentTransform = parent.GetComp<TransformComp>();
            transform->SetParent(parentTransform);
        }
        // childrenがあるなら
        if (objectData.contains("transform")) {
            AddChildren(objectData, levelData, object);
        }
    }
}
