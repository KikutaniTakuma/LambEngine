#include "SkyBox.hlsli"
#define PI 3.14159265358f

struct AtmosphericParams {
	float32_t3 cameraPosition;   // カメラの位置
	float32_t atmosphereHeight;  // 大気の高さ
	float32_t3 lightDirection;   // 太陽光の方向（正規化ベクトル）
	float32_t humidity;          // 大気の湿度
	float32_t3 rayleighScattering; // Rayleigh散乱係数
	float32_t mieScattering;     // Mie散乱係数
	float32_t mieG;              // Mie散乱位相関数のg値（0から1の範囲）
};

ConstantBuffer<AtmosphericParams> gAtmosphericParams : register(b1);

static const float32_t InnerRadius = 10000.0f;
static const float32_t OuterRadius = 10250.0f;
 
static const float32_t Kr = 0.0025f;
static const float32_t Km = 0.001f;

static const float32_t fSamples = 2.0f;
 
static const float32_t3 three_primary_colors = float32_t3(0.68f, 0.55f, 0.44f);
static const float32_t3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);
static const float32_t fOuterRadius = OuterRadius;
static const float32_t fInnerRadius = InnerRadius;
static const float32_t fESun = 20.0f;
static const float32_t fKrESun = Kr * fESun;
static const float32_t fKmESun = Km * fESun;
static const float32_t fKr4PI = Kr * 4.0f * PI;
static const float32_t fKm4PI = Km * 4.0f * PI;
static const float32_t fScale = 1.0f / (OuterRadius - InnerRadius);
static const float32_t fScaleDepth = 0.25f;
static const float32_t fScaleOverScaleDepth = fScale / fScaleDepth;
static const float32_t g = -0.999f;
static const float32_t g2 = g * g;

float32_t Scale(float32_t fcos){
	float32_t x = 1.0 - fcos;
	return fScaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.8f + x * 5.25f))));
}

float32_t3 IntersectionPos(float32_t3 dir, float32_t3 a, float32_t radius) {
	float32_t b = dot(a, dir);
	float32_t c = dot(a, a) - radius * radius;
	float32_t d = max(b * b - c, 0.0f);
 
	return a + dir * (-b + sqrt(d));
}

PixelOutPut main(VertexOutput input)
{
	float32_t3 worldPos = input.worldPosition.xyz;
	worldPos =IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);
	float32_t3 cameraPos = gAtmosphericParams.cameraPosition;
	cameraPos.y += fInnerRadius;
    float32_t3 viewDirection = normalize(worldPos - cameraPos);
    float32_t3 lightDirection = normalize(gAtmosphericParams.lightDirection + viewDirection);

	float32_t3 v3Ray = worldPos - cameraPos;
	float32_t fFar = length(v3Ray);
	v3Ray *= rcp(fFar);

	float32_t3 v3Start = cameraPos;
	float32_t fCameraHeight = length(cameraPos);
	float32_t fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
	float32_t fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	float32_t fStartOffset = fStartDepth * Scale(fStartAngle);
 
	float32_t fSampleLength = fFar / fSamples;
	float32_t fScaledLength = fSampleLength * fScale;
	float32_t3 v3SampleRay = v3Ray * fSampleLength;
	float32_t3 v3SamplePoint = v3Start + v3SampleRay * 0.5;
 
	float32_t3 v3FrontColor = 0.0;
	for(int32_t n = 0; n < int32_t(fSamples); n++){
		float32_t fHeight = length(v3SamplePoint);
		float32_t fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float32_t fLightAngle = dot(lightDirection, v3SamplePoint) / fHeight;
		float32_t fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float32_t fScatter = (fStartOffset + fDepth * (Scale(fLightAngle) - Scale(fCameraAngle)));
		float32_t3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}
 
	float32_t3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
	float32_t3 c1 = v3FrontColor * fKmESun;
	float32_t3 v3Direction = cameraPos - worldPos;
 
	float32_t fcos = dot(lightDirection, v3Direction) / length(v3Direction);
	float32_t fcos2 = fcos * fcos;
 
	float32_t rayleighPhase = 0.75f * (1.0f + fcos2);
	float32_t miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fcos2) / pow(1.0f + g2 - 2.0f * g * fcos, 1.5f);

	PixelOutPut output;
	output.color.rgb = rayleighPhase * c0 + miePhase * c1;
	output.color.rgb = min(output.color.rgb, float32_t3(1.0f, 1.0f, 1.0f));

	output.color.a = 1.0f;

	return output;
}