#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include "Game/Water/Water.h"

#include "Utils/ScreenOut.h"

#include "Utils/Easing.h"

#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

#include "Utils/ConvertString.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


TestScene::TestScene():
	BaseScene(BaseScene::ID::Test),
	start_(),
	stop_(),
	state_(State::kLogin),
	time_(0.0f),
	threshold_(7.0f),
	thresholdTime_(10.0f),
	maxScore_(1000000.0f),
	middleScore_(10000.0f),
	minScore_(0.0f),
	timeOverRange_(0.05f),
	topThresholdTime_(0.5f),
	score_(0),
	rankingText_(),
	username_(),
	userPassword_(),
	token_()
{
	for (int i = 0; i < 32; i++) {
		usernameBuf_[i] = '\0';
		userPasswordBuf_[i] = '\0';
	}
}

void TestScene::Load()
{
}

void TestScene::Initialize() {
}

void TestScene::Finalize() {
}

void TestScene::Update()
{
	if (state_ == TestScene::State::kLogin or state_ == TestScene::State::kNewUser) {
		Lamb::screenout << "Please Login" << Lamb::endline;
	}
	else {
		Lamb::screenout << "Login user is `" << username_ << "`" << Lamb::endline << Lamb::endline;
	}

	switch (state_)
	{
	case TestScene::State::kLogin:
#ifdef USE_DEBUG_CODE
		if (not isLoginNow_) {
			ImGui::Begin("Login");
			ImGui::InputText("name", usernameBuf_, 32);
			ImGui::InputText("password", userPasswordBuf_, 32);
			if (ImGui::Button("login")) {
				for (int i = 0; i < 32; i++) {
					if (usernameBuf_[i] == '\0') {
						break;
					}
					username_ += usernameBuf_[i];
				}
				for (int i = 0; i < 32; i++) {
					if (userPasswordBuf_[i] == '\0') {
						break;
					}
					userPassword_ += userPasswordBuf_[i];
				}

				if (username_.empty() or userPassword_.empty()) {
					MessageBoxA(
						0,
						"Name or Password is empty. Please write your user name or password", "Error",
						0
					);
				}
				else {

					for (int i = 0; i < 32; i++) {
						usernameBuf_[i] = '\0';
						userPasswordBuf_[i] = '\0';
					}

					isLoginNow_ = true;
				}
			}
			if (ImGui::Button("new user")) {
				username_.clear();
				userPassword_.clear();

				state_ = TestScene::State::kNewUser;
			}
			ImGui::End();
		}
#endif // USE_DEBUG_CODE

		break;
	case TestScene::State::kNewUser:
#ifdef USE_DEBUG_CODE
		if (not isNewUser) {
			ImGui::Begin("New User");
			ImGui::InputText("name", usernameBuf_, 32);
			ImGui::InputText("password", userPasswordBuf_, 32);
			if (ImGui::Button("new")) {
				for (int i = 0; i < 32; i++) {
					if (usernameBuf_[i] == '\0') {
						break;
					}
					username_ += usernameBuf_[i];
				}
				for (int i = 0; i < 32; i++) {
					if (userPasswordBuf_[i] == '\0') {
						break;
					}
					userPassword_ += userPasswordBuf_[i];
				}

				if (username_.empty() or userPassword_.empty()) {
					MessageBoxA(
						0,
						"Name or Password is empty. Please write your user name or password", "Error",
						0
					);
				}
				else {

					for (int i = 0; i < 32; i++) {
						usernameBuf_[i] = '\0';
						userPasswordBuf_[i] = '\0';
					}

					isNewUser = true;
				}
			}
			ImGui::End();
		}
#endif // USE_DEBUG_CODE
		break;

	case TestScene::State::kReady:
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			start_ = std::chrono::steady_clock::now();
			state_ = State::kCount;
		}
		break;
	case TestScene::State::kCount:
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			stop_ = std::chrono::steady_clock::now();
			state_ = State::kStop;
		}
		break;
	case TestScene::State::kStop:
		if (input_->GetKey()->Pushed(DIK_RETURN)) {
			state_ = State::kCommunicating;
			time_ = 0.0f;
		}
		break;
	case TestScene::State::kRanking:
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			state_ = State::kReady;

			score_ = 0;
		}

		break;

	default:
		break;
	}

	// 通信中
	if (isLoginNow_ and state_ == TestScene::State::kLogin) {
		std::string userData = PostUserLoginAsync(username_, userPassword_).get();
		
		nlohmann::json j = nlohmann::json::parse(userData);
		std::string status = j["login_status"].get<std::string>();
		if (status == "success") {

			token_ = j["token"].get<std::string>();

			state_ = TestScene::State::kReady;

		}
		else {
			MessageBoxA(
				0,
				"Login error or No user found. try again", "Error",
				0
			);

			for (int i = 0; i < 32; i++) {
				usernameBuf_[i] = '\0';
				userPasswordBuf_[i] = '\0';
			}
			username_.clear();
			userPassword_.clear();
		}
		isLoginNow_ = false;

	}

	if (isNewUser and state_ == TestScene::State::kNewUser) {
		std::string userData = PostNewUserAsync(username_, userPassword_).get();

		nlohmann::json j = nlohmann::json::parse(userData);
		bool isSuccess = (j.find("name") != j.end());

		if (not isSuccess) {
			MessageBoxA(
				0,
				"New User Error. try again", "Error",
				0
			);

			for (int i = 0; i < 32; i++) {
				usernameBuf_[i] = '\0';
				userPasswordBuf_[i] = '\0';
			}
			username_.clear();
			userPassword_.clear();
		}

		else {
			state_ = TestScene::State::kLogin;

			isLoginNow_ = true;
		}

		isNewUser = false;
	}

	if (state_ == TestScene::State::kCommunicating) {
		std::string postRes = PostScoreAsync(score_, token_).get();
		std::string allScoresJson = GetAllScoresAsync(token_).get();

		Lamb::screenout << postRes << Lamb::endline;


		try {
			nlohmann::json j = nlohmann::json::parse(allScoresJson);

			if (j.find("error") != j.end()) {
				std::string userData = PostUserLoginAsync(username_, userPassword_).get();

				nlohmann::json j_userData = nlohmann::json::parse(userData);
				std::string status = j_userData["login_status"].get<std::string>();
				if (status == "success") {

					token_ = j_userData["token"].get<std::string>();
				}
			}
			else {

				rankingText_ = "Ranking Top 20:\n";

				for (int32_t i = 0; const auto& entry : j) {
					int32_t rankScore = entry["score"].get<int32_t>();
					std::string userName = entry["user"]["name"].get<std::string>();


					rankingText_ += std::to_string(i + 1) + ". " + userName + " : " +
						std::to_string(rankScore);

					if (score_ == rankScore and username_ == userName) {
						rankingText_ += " <- new!\n";
					}
					else {
						rankingText_ += "\n";
					}

					++i;
					if (30 <= i) {
						break;
					}
				}

				state_ = State::kRanking;
			}
		}
		catch (const std::exception& err) {
			rankingText_ = std::string("[Ranking Error] : ") + err.what();
			Lamb::screenout << rankingText_;
		}
	}


	std::chrono::steady_clock::time_point now = {};
	int64_t duration = 0;
	float sub = 0.0f;
	std::istringstream iss(rankingText_);
	std::string line;


	switch (state_)
	{
	case TestScene::State::kReady:
		Lamb::screenout << "Press SPACE to start";
		break;
	case TestScene::State::kCount:
		now = std::chrono::steady_clock::now();

		duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();

		time_ = static_cast<float>(duration) * 0.001f;

		if (time_ < threshold_) {
			Lamb::screenout << "time : " << time_ << "s";
		}
		else {
			Lamb::screenout << "time : " << "???s";
		}
		break;
	case TestScene::State::kStop:
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_ - start_).count();

		time_ = static_cast<float>(duration) * 0.001f;
		
		sub = thresholdTime_ - time_;
		
		score_ = Score(sub);

		Lamb::screenout << "result time : " << time_ << "s" << Lamb::endline
			<< "score : " << score_ << Lamb::endline
			<< "Please ENTER key to Ranking";

		break;


	case TestScene::State::kRanking:
		Lamb::screenout << "Your Socre : " << score_ << Lamb::endline << Lamb::endline;

		while (std::getline(iss, line)) {
			Lamb::screenout << line << Lamb::endline;
		}
		Lamb::screenout << "Press SPACE to play again";

		break;

	default:
		break;
	}
}

void TestScene::Draw()
{
	
}

int TestScene::Score(float sub)
{
	float score = 0.0f;

	sub = thresholdTime_ - time_;
	// 10秒超過かつ誤差0.05秒以上
	if (std::signbit(sub) and (timeOverRange_ < std::abs(sub))) {
		score = 0.0f;
	}
	// 10秒により近い
	else if (std::abs(sub) < topThresholdTime_) {
		score = std::lerp(maxScore_, middleScore_, Easing::OutCirc(std::abs(sub) / topThresholdTime_));
	}
	// それ以外
	else {
		score = std::lerp(middleScore_, minScore_, Easing::InOutExpo(sub / (thresholdTime_ - topThresholdTime_)));
	}

	return static_cast<int>(score);
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);

	return size * nmemb;
}

std::future<std::string> TestScene::PostScoreAsync(int score, const std::string& token)
{
	return std::async(std::launch::async, [score, &token]()->std::string {
		CURL* curl  = curl_easy_init();
		if (!curl) {
			return "CURL初期化エラー";
		}

#pragma warning(push)
#pragma warning(disable : 26495)
		nlohmann::json body = nlohmann::json::object();
		body["score"] = score;
#pragma warning(pop)
		std::string bodyStr = body.dump();

		std::string authHeader = "Authorization: Bearer " + token;

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, authHeader.c_str());

		std::string response;

		curl_easy_setopt(curl, CURLOPT_URL,
			"https://swgame-nine-umber.vercel.app/scores");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);

		long httpCode = 0l;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("送信エラー: ") + curl_easy_strerror(res);
		}

		return response;
		}
	);
}

std::future<std::string> TestScene::GetAllScoresAsync(const std::string& token)
{
	return std::async(std::launch::async, [&token]() -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) {
			return "CURL初期化エラー";
		}

		std::string response;

		std::string authHeader = "Authorization: Bearer " + token;

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, authHeader.c_str());

		curl_easy_setopt(curl, CURLOPT_URL, "https://swgame-nine-umber.vercel.app/scores");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("取得エラー: ") + curl_easy_strerror(res);
		}

		return response;
		}
	);
}

std::future<std::string> TestScene::PostNewUserAsync(const std::string& name, const std::string& password)
{
	return std::async(std::launch::async, [&name, &password]()->std::string {
		CURL* curl = curl_easy_init();
		if (!curl) {
			return "CURL初期化エラー";
		}

#pragma warning(push)
#pragma warning(disable : 26495)
		nlohmann::json body = nlohmann::json::object();
		body["name"] = name;
		body["password"] = password;
#pragma warning(pop)
		std::string bodyStr = body.dump();

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		std::string response;

		curl_easy_setopt(curl, CURLOPT_URL,
			"https://swgame-nine-umber.vercel.app/users/new");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);

		long httpCode = 0l;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("送信エラー: ") + curl_easy_strerror(res);
		}

		return response;
		}
	);
}

std::future<std::string> TestScene::PostUserLoginAsync(const std::string& name, const std::string& password)
{
	return std::async(std::launch::async, [&name, &password]()->std::string {
		CURL* curl = curl_easy_init();
		if (!curl) {
			return "CURL初期化エラー";
		}

#pragma warning(push)
#pragma warning(disable : 26495)
		nlohmann::json body = nlohmann::json::object();
		body["name"] = name;
		body["password"] = password;
#pragma warning(pop)
		std::string bodyStr = body.dump();

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		std::string response;

		curl_easy_setopt(curl, CURLOPT_URL,
			"https://swgame-nine-umber.vercel.app/users/login");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);

		long httpCode = 0l;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return std::string("送信エラー: ") + curl_easy_strerror(res);
		}

		return response;
		}
	);
}
