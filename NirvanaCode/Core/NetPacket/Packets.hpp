// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once
#include <YC/ErrorOr/ErrorOr.hpp>

template <typename Type>
concept is_packet = requires {
	Type::__packet_id;
};

using clnt_packet_queue = std::vector<std::function<void(const TArray<uint8>&)>>;
using srv_packet_queue = std::vector<std::function<void(const TArray<uint8>&, int)>>;

inline ErrorOr<void> __call_packet_event(
	const TArray<uint8>& data,
	const uint8_t packet_id,
	const clnt_packet_queue& pac_queue)
{
	if(pac_queue.size() <= packet_id) return Err { std::string("Packet ID is out of range.") };
	if(!pac_queue[packet_id]) return Err { std::format("Packet ID({}) not bind.", packet_id) };
	
	pac_queue[packet_id](data);
	return {};
};

inline ErrorOr<void> __server__call_packet_event(
	const TArray<uint8>& data,
	const uint8_t packet_id,
	const int client_id,
	const srv_packet_queue& pac_queue){
	if(pac_queue.size() <= packet_id) return Err { std::string("Packet ID is out of range.") }; 
	if(!pac_queue[packet_id]) return Err { std::string("Packet ID not bind.") }; 
	
	pac_queue[packet_id](data, client_id);
	return {};
};