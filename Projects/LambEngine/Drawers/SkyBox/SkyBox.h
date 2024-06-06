#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"

class SkyBox {
public:
	SkyBox();
	SkyBox(const SkyBox&) = delete;
	SkyBox(SkyBox&&) = delete;
	~SkyBox() = default;

public:
	SkyBox& operator=(const SkyBox&) = delete;
	SkyBox& operator=(SkyBox&&) = delete;

private:
	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Lamb::LambPtr<ID3D12Resource> vertexResource;
	uint32_t vertexNumber;

	D3D12_INDEX_BUFFER_VIEW indexView;
	Lamb::LambPtr<ID3D12Resource> indexResource;

	static constexpr uint32_t kIndexNumber = 36u;
};