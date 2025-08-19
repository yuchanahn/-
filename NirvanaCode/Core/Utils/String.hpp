#pragma once
#include <YC/Curry/Curry.hpp>

static FString ToFStr (const std::string& Str) {
	return FString(Str.c_str());
}

static std::string ToStr (const FString& Str) {
	return std::string(TCHAR_TO_UTF8(*Str));
}

static FString Append_FStr (const FString& Str1, const FString& Str2) {
	return Str1 + Str2;
}

static FString AppendBack_FStr (const FString& Str1, const FString& Str2) {
	return Str2 + Str1;
}

static auto AppendFront_FStr (const FString& Str1) -> decltype(Curry(Append_FStr)(Str1)){
	return Curry(Append_FStr)(Str1);
}

