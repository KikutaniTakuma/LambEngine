#pragma once
#include <string>

class PipelineObject {
public:
	PipelineObject() = default;
	PipelineObject(const PipelineObject&) = delete;
	PipelineObject(PipelineObject&&) = delete;
	~PipelineObject() = default;

	PipelineObject& operator=(const PipelineObject&) = delete;
	PipelineObject& operator=(PipelineObject&&) = delete;

private:
	void Initialize(const std::string& vsShader, const std::string& psShader);

	void Use();

protected:
	class Pipeline* pipeline_;
};