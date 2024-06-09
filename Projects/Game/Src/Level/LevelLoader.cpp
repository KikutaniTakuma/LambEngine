#include "LevelLoader.h"
#include "Utils/FileUtils.h"
#include "GameObject/Comp/ModelRenderComp.h"
#include "GameObject/Comp/TransformComp.h"
#include "GameObject/Comp/ObbComp.h"

LevelData* LoadLevel(const std::string& fileName)
{
    auto data = Lamb::LoadJson(fileName);

    std::string name = data["name"].get<std::string>();

    if (name.compare("scene") != 0) {
        throw Lamb::Error::Code<Lamb::Error::Function>("this file is invalid" + fileName, ErrorPlace);
    }

    LevelData* levelData = new LevelData();

    for (nlohmann::json& object : data["objects"]) {
        if (not object.contains("type")) {
            throw Lamb::Error::Code<Lamb::Error::Function>("this file is invalid" + fileName, ErrorPlace);
        }

        std::string type = object["type"] = object["type"].get<std::string>();

        if (type.compare("MESH") == 0) {
            levelData->objects.emplace_back(std::make_unique<Object>());

            Object& element = *levelData->objects.back();

            if (object.contains("file_name")) {
                Lamb::SafePtr model = element.AddComp<ModelRenderComp>();
                model->SetFileNmae(object["file_name"]);
                element.SetTag("Model");
            }

            if (type.compare("transform") == 0) {
                Lamb::SafePtr transformComp = element.AddComp<TransformComp>();
                Transform transform{};

                nlohmann::json& transformData = object["transform"];

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

                element.SetTag("transform");
            }
            if (type.compare("collider") == 0) {
                Lamb::SafePtr obbComp = element.AddComp<ObbComp>();
                Transform transform{};

                nlohmann::json& transformData = object["transform"];

                for (size_t i = 0; i < transform.translate.size(); i++) {
                    transform.translate[i] = static_cast<float>(transformData["translation"][i]);
                }
                for (size_t i = 0; i < transform.rotate.size(); i++) {
                    transform.rotate[i] = static_cast<float>(transformData["rotation"][i]);
                }
                for (size_t i = 0; i < transform.scale.size(); i++) {
                    transform.scale[i] = static_cast<float>(transformData["scaling"][i]);
                }


                obbComp->transform.translate = transform.translate;
                obbComp->transform.rotate = Quaternion::EulerToQuaternion(transform.rotate);
                obbComp->transform.scale = transform.scale;

                element.SetTag("obb");
            }
        }
    }

    return levelData;
}
