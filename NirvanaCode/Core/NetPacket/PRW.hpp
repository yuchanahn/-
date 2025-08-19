#pragma once

template <typename T>
concept is_u_struct = std::is_same_v<decltype(T::StaticStruct()), UScriptStruct*>;

namespace YC {namespace PRW {



template <typename T>
T Deserialize(const TArray<uint8>& SerializedData) {
	T Value;
	FMemoryReader Reader(SerializedData);
	if constexpr (is_u_struct<T>) { Value.StaticStruct()->SerializeItem(Reader, &Value, nullptr); }
	else { Reader << Value; }
	return Value;
}

template <typename T>
TArray<uint8> Serialize(T PacketData) {
	TArray<uint8> SerializedData;
	FMemoryWriter Writer(SerializedData);
	if constexpr (is_u_struct<T>) { PacketData.StaticStruct()->SerializeItem(Writer, &PacketData, nullptr); }
	else { Writer << PacketData; }
	return SerializedData;
}



}}
