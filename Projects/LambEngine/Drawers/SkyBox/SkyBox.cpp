#include "SkyBox.h"
#include <array>
#include "Math/Vector4.h"
#include "Math/MathCommon.h"

SkyBox::SkyBox() {
    std::array vertexData = {
        Vector4(  1.0f,  1.0f, -1.0f, 1.0f ),
        Vector4(  1.0f, -1.0f, -1.0f, 1.0f ),
        Vector4(  1.0f,  1.0f,  1.0f, 1.0f ),
        Vector4(  1.0f, -1.0f,  1.0f, 1.0f ),
        Vector4( -1.0f,  1.0f, -1.0f, 1.0f ),
        Vector4( -1.0f, -1.0f, -1.0f, 1.0f ),
        Vector4( -1.0f,  1.0f,  1.0f, 1.0f ),
        Vector4( -1.0f, -1.0f,  1.0f, 1.0f ),
    };

    std::array indexData = {
        0_u16, 2_u16, 4_u16,
        3_u16, 7_u16, 2_u16,
        7_u16, 5_u16, 6_u16,
        5_u16, 7_u16, 1_u16,
        1_u16, 3_u16, 0_u16,
        5_u16, 1_u16, 4_u16,
        2_u16, 6_u16, 4_u16,
        7_u16, 6_u16, 2_u16,
        5_u16, 4_u16, 6_u16,
        7_u16, 3_u16, 1_u16,
        3_u16, 2_u16, 0_u16,
        1_u16, 0_u16, 4_u16,
    };
}
