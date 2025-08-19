#pragma once
#include "Packets.hpp"
#include "PRW.hpp"
#include "../NetPC.h"

// Server 가 호출 합니다.
template<is_packet T>
ErrorOr<void> Server_Send(T Packet, const ANetPC* TargetClient) {
	if(TargetClient == nullptr) return Err { std::string("Target is not valid") };
 	auto Data = YC::PRW::Serialize(Packet);
	if(Data.Num() == 0) return Err { std::string("Serialize failed!") };
	TargetClient->SendPacketToClient(Packet.__packet_id, Data);
	return {};
}

// Client 가 호출 합니다.
template<is_packet T>
ErrorOr<void> Send(T Packet, const ANetPC* Client) {
	if(Client == nullptr) return Err { std::string("Client is not valid") };
	auto Data = YC::PRW::Serialize(Packet);
	if(Data.Num() == 0) return Err { std::string("Serialize failed!") };
	Client->SendPacketToServer(Packet.__packet_id, Data);
	return {};
}

template<is_packet T> static ErrorOr<T> Unpack(T Pac) { return Pac; }
template<typename T> static ErrorOr<T> To(T Pac) { return Pac; }
