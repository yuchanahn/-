#pragma once

#include "GameCharacter/CharacterSystem.hpp"

#include "../Client/Client.hpp"
#include "../Server/Server.hpp"

constexpr double DSec = 0.01;

struct FGameStartData {
	std::chrono::time_point<std::chrono::steady_clock> GameLoopStartTime;
	YC::Client::FWorld World;
};

inline FGameStartData GameStart(
	const UObject* InGameMaster,
	const std::vector<std::function<void()>>& OnGameStart_EventList,
	YC::Client::FPacBuf* Pacbuf
	) {
	checkf(0, L"GameStart 함수는 더이상 사용하지 않습니다.");
	
	Panic = [](const std::string& Msg) {
		UE_LOG(LogTemp, Error, TEXT("%s"), *FString(Msg.c_str()));
		checkf(0, L"스텍트레이스 잘 읽어라...{ Err : %s }", *FString(Msg.c_str()));
	};
	const auto NetPC = YC::GetWorld(InGameMaster) | YC::GetNetPC;
	if(NetPC.IsErr()) {
		NetPC | WhenErr | YC::Log::Push_S;
		return {
			.GameLoopStartTime = std::chrono::high_resolution_clock::now(),
			.World = {}
		};
	}

	const YC::Client::FWorld World;// = ClientLoad(InGameMaster, Pacbuf, NetPC.Unwrap(), TODO);
	
	if((YC::GetWorld(InGameMaster) | YC::GetNetPC | YC::HasAuthority).Or(false)) {
		//YC::Server::ServerLoad(const_cast<UObject*>(InGameMaster));
	}

	for(auto& Event : OnGameStart_EventList) { Event(); }

	return {
		.GameLoopStartTime = std::chrono::high_resolution_clock::now(),
		.World = World
	};
}

struct FGameLoopEndData {
	int TickCount;
	YC::Client::FWorld World;
	bool bIsGameEnd;
};

inline FGameLoopEndData GameLoop(
	const std::chrono::time_point<std::chrono::steady_clock> GameLoopStartTime,
	const int TickCount,
	YC::Client::FPacBuf* PacBuf,
	const YC::Client::FWorld& World,
	const UObject* G
	) {
	const auto T = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now() - GameLoopStartTime).count() / 1000.0;
	int NewTickCount = TickCount;
	
	double Sec = T - TickCount * DSec;
	
	YC::Client::FWorld NewWorld = World;
	
	while (Sec >= DSec) {
		auto NetPc = YC::GetWorld(G) | YC::GetNetPC;

		if((NetPc | YC::HasAuthority).Or(false)) YC::Server::ServerTick(G);
		
		if(NetPc.IsOk()) {
			const auto Pac_Clone = *PacBuf;
			PacBuf->Clear();
			NewWorld = ClientTick(G, Pac_Clone, NewWorld);
		}
		Sec -= DSec;
		NewTickCount++;
	}
	if((YC::GetWorld(G) | YC::GetNetPC | YC::HasAuthority).Or(false)) {
		YC::Server::ServerRenderTick(G);
	}
	NewWorld = ClientRenderTick(G, NewWorld);
	
	return FGameLoopEndData {
		.TickCount = NewTickCount,
		.World = NewWorld,
		.bIsGameEnd = false
	};
}

inline void GameEnded() {
	YC::Server::FServer::OnGameEnded();
}