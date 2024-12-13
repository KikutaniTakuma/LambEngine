#include "Corals.h"
#include <string>

#include "Utils/Random.h"


void Corals::Init(float32_t coralHeight, ModelVertexLevel modelLevel) {
    std::string coralModelFilePath;

    // 頂点を変更
    switch (modelLevel)
    {
    case Corals::ModelVertexLevel::LEVEL0_3584:
        coralModelFilePath = "./Resources/Coral/Coral_3584.obj";
        break;
    case Corals::ModelVertexLevel::LEVEL1_9866:
        coralModelFilePath = "./Resources/Coral/Coral_9866.obj";
        break;
    case Corals::ModelVertexLevel::LEVEL2_48522:
        coralModelFilePath = "./Resources/Coral/Coral_48522.obj";
        break;
    }

    models_ = std::make_unique<std::array<ModelInstance, kLowCoralObjectNum_* kLowCoralObjectNum_>>();

    float32_t minScale = 8.0f;
    float32_t maxScale = 13.0f;
    float32_t maxScaleY = 10.0f;

    float32_t offset = 13.0f;
    size_t countX = 0;
    size_t countY = 0;

    float32_t posRandomMin = -4.0f;
    float32_t posRandomMax = 4.0f;
    std::array<uint32_t, 4> colorRandom = {
        0x2DFF19E0,
        0xDDFF19E0,
        0xFF1919E0,
        0xC919FFE0,
    };

    Vector2 startPos = Vector2(-143.0f, -143.0f);


    for (auto& i : *models_) {
        i.Load(coralModelFilePath);
        i.blend = BlendType::kNone;
        i.color = colorRandom[Lamb::Random(0llu, colorRandom.size() - 1)];
        i.scale.y = Lamb::Random(minScale, maxScaleY);
        i.scale.x = Lamb::Random(minScale, maxScale);
        i.scale.z = Lamb::Random(minScale, maxScale);
        i.pos.y = coralHeight;

        i.pos.x = startPos.x + offset * static_cast<float32_t>(countX) + Lamb::Random(posRandomMin, posRandomMax);
        i.pos.z = 100.0f + startPos.y + offset * static_cast<float32_t>(countY) + Lamb::Random(posRandomMin, posRandomMax);
        countX++;
        if (kLowCoralObjectNum_ <= countX) {
            countX = 0;
            countY++;
        }
    }

    for (auto& i : *models_) {
        i.Update();
    }
}

void Corals::Draw(const float32_t4x4& cameraMat) {
    for (auto& i : *models_) {
        i.Draw(cameraMat);
    }
}
