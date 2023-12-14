#pragma once
#include <string>
#include "../Engine/Graphics/Shader/Shader.h"

class PipelineObject {
public:
	PipelineObject() = default;
	PipelineObject(const PipelineObject&) = delete;
	PipelineObject(PipelineObject&&) = delete;
	~PipelineObject() = default;

	PipelineObject& operator=(const PipelineObject&) = delete;
	PipelineObject& operator=(PipelineObject&&) = delete;

protected:
	void LoadShader(const std::string& vsShader, const std::string& psShader);

public:
	virtual void Use() = 0;

	virtual void Init(const std::string& vsShader, const std::string& psShader) = 0;

protected:
	Shader shader_;

	class Pipeline* pipeline_;
};