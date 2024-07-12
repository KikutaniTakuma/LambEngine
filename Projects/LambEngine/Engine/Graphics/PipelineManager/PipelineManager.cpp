#include "PipelineManager.h"
#include <cassert>
#include <algorithm>
#include "Engine/Graphics/RootSignature/RootSignature.h"
#include "Utils/SafeDelete.h"
#include "Utils/ExecutionLog.h"
#include "Error/Error.h"

Lamb::SafePtr<PipelineManager> PipelineManager::instance_ = nullptr;

void PipelineManager::Initialize() {
	if (instance_) {
		return;
	}
	instance_.reset(new PipelineManager());
	if(instance_){
		Lamb::AddLog("Initialize PipelineManager succeeded");
	}
	else {
		throw Lamb::Error::Code<PipelineManager>("instance is nullptr", ErrorPlace);
	}
}
void PipelineManager::Finalize() {
	instance_.reset();

	Lamb::AddLog("Finalize PipelineManager succeeded");
}

PipelineManager* const PipelineManager::GetInstance()
{
	return instance_.get();
}

RootSignature* PipelineManager::CreateRootSgnature(const RootSignature::Desc& desc, bool isTexture)
{
	auto IsSame = [&desc, &isTexture](const std::unique_ptr<RootSignature>& rootSignature_) {
		return rootSignature_->IsSame(desc, isTexture);
		};

	auto rootSignatureItr = std::find_if(instance_->rootSignatures_.begin(), instance_->rootSignatures_.end(), IsSame);

	if (rootSignatureItr == instance_->rootSignatures_.end()) {
		auto rootSignature = std::make_unique<RootSignature>();

		rootSignature->Create(desc, isTexture);

		instance_->rootSignatures_.push_back(std::move(rootSignature));

		return instance_->rootSignatures_.back().get();
	}
	else {
		return rootSignatureItr->get();
	}
}

void PipelineManager::SetDesc(const Pipeline::Desc& desc) {
	pipelineDesc_ = desc;
}

Pipeline* const PipelineManager::Create() {
	auto IsSmae = [this](const std::unique_ptr<Pipeline>& pipeline) {
			return pipeline->IsSame(pipelineDesc_);
		};

	auto pipelineItr = std::find_if(instance_->pipelines_.begin(), instance_->pipelines_.end(), IsSmae);

	if (pipelineItr == instance_->pipelines_.end()) {
		auto pipeline = std::make_unique<Pipeline>();
		pipeline->Create(pipelineDesc_);

		if (not pipeline->graphicsPipelineState_) {
			return nullptr;
		}

		instance_->pipelines_.push_back(std::move(pipeline));

		return instance_->pipelines_.back().get();
	}
	else {
		return pipelineItr->get();
	}
}

Pipeline* const PipelineManager::CreateCubeMap()
{
	auto IsSmae = [this](const std::unique_ptr<Pipeline>& pipeline) {
		return pipeline->IsSame(pipelineDesc_);
		};

	auto pipelineItr = std::find_if(instance_->pipelines_.begin(), instance_->pipelines_.end(), IsSmae);

	if (pipelineItr == instance_->pipelines_.end()) {
		auto pipeline = std::make_unique<Pipeline>();
		pipeline->CreateCubeMap(pipelineDesc_);

		if (not pipeline->graphicsPipelineState_) {
			return nullptr;
		}

		instance_->pipelines_.push_back(std::move(pipeline));

		return instance_->pipelines_.back().get();
	}
	else {
		return pipelineItr->get();
	}
}

void PipelineManager::StateReset() {
	pipelineDesc_ = {};
}

PipelineManager::PipelineManager() :
	pipelines_(),
	rootSignatures_(),
	pipelineDesc_()
{}