#include "JsonComp.h"
#include "Utils/FileUtils.h"

void JsonCmop::Load(const std::string& fileName)
{
	jsonData_ = Lamb::LoadJson(fileName);
}

nlohmann::json& JsonCmop::GetJsonData() {
	return jsonData_;
}

const nlohmann::json& JsonCmop::GetJsonData() const {
	return jsonData_;
}
