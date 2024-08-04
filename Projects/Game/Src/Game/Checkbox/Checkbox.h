#pragma once

#include "Math/Vector2.h"

class Checkbox {
public:
    Checkbox(const Vector2& pos, const Vector2& scale) : position_(pos), scale_(scale) {}

    bool IsCollision(const Vector2& mousePos);
private:
    Vector2 position_;
    Vector2 scale_;
};
