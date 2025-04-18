/// =========================
/// ==  Errorクラスの定義  ==
/// =========================


#pragma once
#include <string>
#include <filesystem>
#include <typeinfo>
#include "Utils/ExecutionLog.h"

namespace Lamb {
	class Error {
	public:
		class Function{};
		class Value{};

	public:
		Error() = default;
		Error(const Error&) = default;
		Error(Error&&) = default;
		~Error() = default;

		Error& operator=(const Error&) = default;
		Error& operator=(Error&&) = default;

	public:
		/// <summary>
		/// エラー内容
		/// </summary>
		const std::string& What() const {
			return errorCode_;
		}

		/// <summary>
		/// エラーが起きた関数
		/// </summary>
		const std::string& FunctionName() const {
			return functionName_;
		}

		/// <summary>
		/// エラーが起きたクラス
		/// </summary>
		const std::string& ClassName() const {
			return className_;
		}

		/// <summary>
		/// エラーが起きたファイル名
		/// </summary>
		const std::string& SourceFileName() const {
			return sourceFileName_;
		}
		/// <summary>
		/// エラーが起きた行数
		/// </summary>
		const std::string& CodeLineNumber() const {
			return codeLineNumber_;
		}

	private:
		std::string className_;
		std::string errorCode_;
		std::string functionName_;
		std::string sourceFileName_;
		std::string codeLineNumber_;

	public:
		/// <summary>
		/// 例外をthrowするための関数
		/// </summary>
		/// <typeparam name="T">className</typeparam>
		/// <param name="errorCode">どんな内容のエラーか</param>
		/// <param name="functionName">関数名</param>
		/// <param name="sourceFileName">ソースコードのファイル名</param>
		/// <param name="codeLineNumber">ラインの場所</param>
		/// <returns></returns>
		template<class T>
		static const Error& Code(
			const std::string& errorCode, 
			const std::string& functionName,
			const std::string& sourceFileName,
			uint32_t codeLineNumber
		) {
			static Error err;
			err = Error{};

			err.errorCode_ = errorCode;
			err.className_ = typeid(T).name();
			err.functionName_ = functionName + "()";
			err.sourceFileName_ = sourceFileName;
			err.codeLineNumber_ = std::to_string(codeLineNumber);
			Lamb::DebugLog(std::string("Error ") + err.SourceFileName() + ":" + err.CodeLineNumber() + " : " + err.What());

			return err;
		}
	};
}

#ifndef ErrorPlace
#define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define ErrorPlace __func__, std::filesystem::path(__FILE__).filename().string(), __LINE__
#endif // !ErrorPlace

#ifndef FilePlace
#define FilePlace ErrorPlace
#endif // !FilePlace
