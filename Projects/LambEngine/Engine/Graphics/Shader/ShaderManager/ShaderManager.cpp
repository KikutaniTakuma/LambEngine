#include "ShaderManager.h"
#include "../ShaderFactory/ShaderFactory.h"
#include "Engine/Engine.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>
#include <format>
#include "Utils/ExecutionLog/ExecutionLog.h"
#pragma warning(disable: 6387)

ShaderManager* ShaderManager::instance_ = nullptr;

ShaderManager::ShaderManager() {
	vertexShader_.clear();
	hullShader_.clear();
	domainShader_.clear();
	geometoryShader_.clear();
	pixelShader_.clear();

	ShaderFactory::Initialize();

	shaderFactory_ = ShaderFactory::GetInstance();

	Lamb::AddLog("ShaderManager Create Succeeded");
}

ShaderManager::~ShaderManager() {
	ShaderFactory::Finalize();
}

void ShaderManager::Initialize() {
	instance_ = new ShaderManager();
	assert(instance_);
}

void ShaderManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

IDxcBlob* ShaderManager::LoadVertexShader(const std::string& fileName) {
	auto itr = instance_->vertexShader_.find(fileName);
	if (itr == instance_->vertexShader_.end()) {
		IDxcBlob* shader = instance_->shaderFactory_->CompilerShader(ConvertString(fileName), L"vs_6_0");
		assert(shader);
		instance_->vertexShader_.insert(std::make_pair(fileName, shader));
	}
	return instance_->vertexShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadHullShader(const std::string& fileName) {
	auto itr = instance_->hullShader_.find(fileName);
	if (itr == instance_->hullShader_.end()) {
		IDxcBlob* shader = instance_->shaderFactory_->CompilerShader(ConvertString(fileName), L"hs_6_0");
		assert(shader);
		instance_->hullShader_.insert(std::make_pair(fileName, shader));
	}

	return instance_->hullShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadDomainShader(const std::string& fileName) {
	auto itr = instance_->domainShader_.find(fileName);
	if (itr == instance_->domainShader_.end()) {
		IDxcBlob* shader = instance_->shaderFactory_->CompilerShader(ConvertString(fileName), L"ds_6_0");
		assert(shader);
		instance_->domainShader_.insert(std::make_pair(fileName, shader));
	}
	return instance_->domainShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadGeometoryShader(const std::string& fileName) {
	auto itr = instance_->geometoryShader_.find(fileName);
	if (itr == instance_->geometoryShader_.end()) {
		IDxcBlob* shader = instance_->shaderFactory_->CompilerShader(ConvertString(fileName), L"gs_6_0");
		assert(shader);
		instance_->geometoryShader_.insert(std::make_pair(fileName, shader));
	}
	return instance_->geometoryShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadPixelShader(const std::string& fileName) {
	auto itr = instance_->pixelShader_.find(fileName);
	if (itr == instance_->pixelShader_.end()) {
		IDxcBlob* shader = instance_->shaderFactory_->CompilerShader(ConvertString(fileName), L"ps_6_0");
		assert(shader);
		instance_->pixelShader_.insert(std::make_pair(fileName, shader));
	}
	return instance_->pixelShader_[fileName].Get();
}