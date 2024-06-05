#include "SkyBox.h"
#include "Utils/Array.h"
#include "Math/Vector4.h"
#include "Math/MathCommon.h"

SkyBox::SkyBox() {
    Lamb::Array vertexData = {
        Vector4(  1.0f,  1.0f, -1.0f, 1.0f ),
        Vector4(  1.0f, -1.0f, -1.0f, 1.0f ),
        Vector4(  1.0f,  1.0f,  1.0f, 1.0f ),
        Vector4(  1.0f, -1.0f,  1.0f, 1.0f ),
        Vector4( -1.0f,  1.0f, -1.0f, 1.0f ),
        Vector4( -1.0f, -1.0f, -1.0f, 1.0f ),
        Vector4( -1.0f,  1.0f,  1.0f, 1.0f ),
        Vector4( -1.0f, -1.0f,  1.0f, 1.0f ),
    };

    Lamb::Array indexData = {
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
