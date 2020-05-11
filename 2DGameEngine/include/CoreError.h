#pragma once
#include <string>
#include <exception>
#include <window.h>

namespace CoreError
{
	using std::string;
	using std::exception;
	/*
	Error codes
		@Negative numbers are fatal errors that may require the game engine to be shutdown.
		@Positive numbers are warnings that do not require the game engine to be shutdown.
	 */
	const INT32 FATAL_ERROR = -1;
	const INT32 WARNING = 1;

	// Engine Error class. Thrown when an error is detected by the game engine.
	// Inherits from std::exception
	class GameEngineError : exception
	{
	private:
		INT32 errorCode;
		string message;
	public:
		GameEngineError() throw() : errorCode(FATAL_ERROR), message("Undefined error in engine core.") {}
		GameEngineError(const INT32 _code, const string& _msg) throw() : errorCode(_code), message(_msg) {}

		GameEngineError(const GameEngineError& _gee) throw() : exception(_gee), errorCode(_gee.errorCode), message(_gee.message) {}
		GameEngineError& operator=(const GameEngineError& rhs) throw() {
			exception::operator=(rhs);
			this->errorCode = rhs.errorCode;
			this->message = rhs.message;
		}

		virtual ~GameEngineError() noexcept {};

		virtual const char* what() const throw() {
			return this->getMessage();
		}
		const char* getMessage() const throw() {
			return message.c_str();
		}
		INT32 getErrorCode() const throw() {
			return errorCode;
		}
	};
};
