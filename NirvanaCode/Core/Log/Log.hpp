#pragma once
#include <memory>
#include <string>
#include <format>
#include <stdexcept>

namespace YC_Impl {
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
	int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
	if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
	auto size = static_cast<size_t>( size_s );
	std::unique_ptr<char[]> buf( new char[ size ] );
	std::snprintf( buf.get(), size, format.c_str(), args ... );
	return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
}

namespace YC {
struct Log {
	static void Push(const FString& Msg) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
	}

	template<typename... Ts>
	static void PushFmt(const FString& Msg, Ts ...Args) {
		auto FmtStr = YC_Impl::string_format(TCHAR_TO_UTF8(*Msg), Args...);
		UE_LOG(LogTemp, Warning, TEXT("%hs"), FmtStr.c_str());
	}
	
	static void Push_Dp(const FString& Msg) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Msg);
	}

	static void Push_S(const std::string& Msg) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(UTF8_TO_TCHAR(Msg.c_str())));
	}

	template<typename... Ts>
	static void PushFmt_S(const std::string& Msg, Ts ...Args) {
		auto Target = YC_Impl::string_format(Msg, Args...);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(UTF8_TO_TCHAR(Target.c_str())));
	}
	
	static void Push_Err_S(const std::string& Msg) {
		UE_LOG(LogTemp, Error, TEXT("%s"), *FString(UTF8_TO_TCHAR(Msg.c_str())));
	}

	static void Push_Err(const FString& Msg) {
		UE_LOG(LogTemp, Error, TEXT("%s"), *Msg);
	}

	static void Push_Dp_S(const std::string& Msg) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *FString(UTF8_TO_TCHAR(Msg.c_str())));
	}
};
}
