#include "../Lamb.hlsli"

#define PI 3.14159265358f
#define kMaxRipplePoints 96

struct AtmosphericParams {
	float32_t3 cameraPosition;
	float32_t pad;
	float32_t3 lightDirection;
	float32_t rayleighScattering;
	float32_t mieScattering;
	float32_t mieG;
};

struct WaveData{
    float32_t waveStrength;
    float32_t ripples;
    float32_t waveSpeed;
    float32_t lengthAttenuation;
    float32_t timeAttenuation;

    float32_t3 ripplesPoint[kMaxRipplePoints];
    float32_t time[kMaxRipplePoints];
};

struct WaterData {
    float32_t2 randomVec;
    float32_t density;
    uint32_t edgeDivision;
    uint32_t insideDivision;
    float32_t3 cameraPosition;
    float32_t distanceThreshold;
    float32_t3 cameraDirection;
    float32_t4x4 viewportMatrix;
    WaveData waveData;
    AtmosphericParams atmosphericParams;
    float32_t time;
};

StructuredBuffer<WaterData> kWaterData : register(t2);