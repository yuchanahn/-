#include "FModPlayer.hpp"

#include "..\Static\ResourceMap\ResSound.h"

#include "YC/Core/Client/Client.hpp"
#include "YC/Core/Server/Server.hpp"


ANetPC* YC::FMod::CheckServer::PC = nullptr;

void YC::FMod::SendSFX(const FString& Key, UFMODEvent* Event, const FVector& InPos) {
	if(CheckServer::PC == nullptr) return;//checkf(0, L"CheckServer::PC is nullptr");
	Send(FPac_FModPlay {
		.EventKey = Key,
		.EventID = Res::FMod::GetIdxOf(Event),
		.EttIdx = -1,
		.bAttach = false,
		.Pos = InPos,
		.bStop =  false,
		.bRelese = false,
	}, CheckServer::PC);
}

void YC::FMod::SendSFX_Stop(const FString& Key, bool bRelease) {
	Send(FPac_FModPlay {
		.EventKey = Key,
		.EventID = -1,
		.EttIdx = -1,
		.bAttach = false,
		.Pos = FVector::ZeroVector,
		.bStop =  true,
		.bRelese = bRelease,
	}, CheckServer::PC);
}


void YC::FMod::SendSFXTo(UFMODEvent* Event, const AActor* InTarget) {
	
}