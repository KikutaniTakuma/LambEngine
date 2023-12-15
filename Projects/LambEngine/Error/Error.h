#pragma once
#include <string>
#include <typeinfo>

class Error {
public:
	class Code {
		friend Error;
	public:
		const std::string& what() const {
			return errorCode_;
		}

		const std::string& function() const {
			return functionName_;
		}

		const std::string& className() const {
			return className_;
		}

	private:
		std::string className_;
		std::string errorCode_;
		std::string functionName_;
	};

public:
	Error() = default;
	Error(const Error&) = default;
	Error(Error&&) = default;
	~Error() = default;

	Error& operator=(const Error&) = default;
	Error& operator=(Error&&) = default;

public:
	const Error::Code& code() const {
		return code_;
	}

	template<class T>
	const Error& set(const std::string& errorCode, const std::string& functionName) {
		code_.errorCode_ = errorCode;
		code_.className_ = typeid(T).name();
		code_.functionName_ = functionName;

		return *this;
	}

private:
	Code code_;
};