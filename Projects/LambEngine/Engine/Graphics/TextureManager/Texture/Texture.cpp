/// ===========================
/// ==  Textureクラスの定義  ==
/// ===========================


#include "Texture.h"
#include "../TextureManager.h"
#include "../externals/DirectXTex/d3dx12.h"
#include "Utils/ConvertString.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include <cassert>
#include <filesystem>
#include "dxgiformat.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Error/Error.h"

Texture::Texture():
	textureResouce_(nullptr),
	intermediateResource_(nullptr),
	srvDesc_(),
	isLoad_(false),
	threadLoadFlg_(false),
	isCubeMap_(false),
	size_(),
	fileName_()
{}

Texture::~Texture() {
	Unload();
}


Texture::Texture(Texture&& tex) noexcept {
	*this = std::move(tex);
}
Texture& Texture::operator=(Texture&& tex) noexcept {
	this->textureResouce_ = std::move(tex.textureResouce_);
	this->intermediateResource_ = std::move(tex.intermediateResource_);
	this->srvDesc_ = tex.srvDesc_;
	isLoad_ = tex.isLoad_;
	fileName_ = tex.fileName_;

	return *this;
}

void Texture::Load(const std::string& filePath, ID3D12GraphicsCommandList* commandList) {
	if (!isLoad_ && !threadLoadFlg_) {
		this->fileName_ = filePath;
		std::string extension = std::filesystem::path(fileName_).extension().string();

		DirectX::ScratchImage images = LoadTexture(filePath);
		const DirectX::TexMetadata& metadata = images.GetMetadata();
		size_ = { static_cast<float>(metadata.width),static_cast<float>(metadata.height) };
		textureResouce_ = CreateTextureResource(metadata);

		if (textureResouce_) {
			intermediateResource_
				= UploadTextureData(
				textureResouce_.Get(), 
				images, 
				commandList
			);
		}
		else {
			return;
		}

		srvDesc_.Format = metadata.format;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) {
			srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc_.TextureCube.MostDetailedMip = 0;
			srvDesc_.TextureCube.MipLevels = UINT_MAX;
			srvDesc_.TextureCube.ResourceMinLODClamp = 0.0f;

			isCubeMap_ = true;
		}
		else {
			srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc_.Texture2D.MipLevels = UINT(metadata.mipLevels);
		}

		// load済み
		isLoad_ = true;
	}
}

void Texture::Unload() {
	if (isLoad_) {
		srvDesc_ = {};

		if (intermediateResource_) {
			intermediateResource_.Reset();
		}
		if (textureResouce_) {
			textureResouce_.Reset();
		}

		CbvSrvUavHeap::GetInstance()->ReleaseView(*this);

		// Unload済み
		isLoad_ = false;
	}
}


DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath) {
	if (!std::filesystem::exists(filePath)) {
		throw Lamb::Error::Code<Texture>("This file is not exist -> " + filePath, ErrorPlace);
	}

	// テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	bool isddsfile = filePathW.ends_with(L".dds");
	HRESULT hr;
	if (isddsfile) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Texture>("DirectX::Load failed", ErrorPlace);
	}

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};

	// 圧縮されていたら解凍する
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		DirectX::ScratchImage tmpImage{};
		hr = DirectX::Decompress(
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			DXGI_FORMAT_UNKNOWN,
			tmpImage
		);

		if (not SUCCEEDED(hr)) {
			throw Lamb::Error::Code<Texture>("DirectX::Decompress failed", ErrorPlace);
		}

		image = std::move(tmpImage);
	}

	if (not isddsfile) {
		hr = DirectX::GenerateMipMaps(
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			DirectX::TEX_FILTER_SRGB, 0, mipImages
		);

		if (not SUCCEEDED(hr)) {
			throw Lamb::Error::Code<Texture>("DirectX::GenerateMipMaps failed", ErrorPlace);
		}

		image = std::move(mipImages);
	}

	// ミップマップ付きのデータを返す
	return image;
}

ID3D12Resource* Texture::CreateTextureResource(
	const DirectX::TexMetadata& metaData
) {
	Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();

	if (metaData.width == 0 || metaData.height == 0) {
		return nullptr;
	}

	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metaData.width);
	resourceDesc.Height = UINT(metaData.height);
	resourceDesc.MipLevels = UINT16(metaData.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metaData.arraySize);
	resourceDesc.Format = metaData.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);

	// 利用するHeapの設定。非常に特殊な運用
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resouceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	if (FAILED(hr)) {
		throw Lamb::Error::Code<Texture>("somehitng error", ErrorPlace);
	}

	return resource;
}

ID3D12Resource* Texture::UploadTextureData(
	ID3D12Resource* texture, 
	const DirectX::ScratchImage& images, 
	ID3D12GraphicsCommandList* commandList
) {
	Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
	
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device.get(), images.GetImages(), images.GetImageCount(), images.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* resource = DirectXDevice::GetInstance()->CreateBufferResuorce(intermediateSize);
	UpdateSubresources(commandList, texture, resource, 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResouceStateを変更する
	
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = texture;
	// subResourceの設定
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// 遷移前(現在)のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	// 遷移後のResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	return resource;
}



void Texture::CreateView(
	D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
	D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
	UINT heapHandle
) {
	Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
	device->CreateShaderResourceView(textureResouce_.Get(), &srvDesc_, heapHandleCPU);

	heapHandleCPU_ = heapHandleCPU;
	heapHandleGPU_ = heapHandleGPU;
	heapHandle_ = heapHandle;
}


void Texture::ReleaseIntermediateResource() {
	if (intermediateResource_) {
		intermediateResource_.Reset();
	}
}

void Texture::Use(UINT rootParamator) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	textureManager->Use(heapHandle_, rootParamator);
}

void Texture::Set(
	const Lamb::LambPtr<ID3D12Resource>& resource,
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc,
	D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
	UINT heapHandle
) {
	if (CanUse()) {
		CbvSrvUavHeap* srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(*this);
		textureResouce_.Reset();
	}

	textureResouce_ = resource;
	srvDesc_ = viewDesc;

	heapHandleGPU_ = heapHandleGPU;
	heapHandle_ = heapHandle;

	// load済み
	isLoad_ = true;
}