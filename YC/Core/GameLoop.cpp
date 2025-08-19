#include "GameLoop.h"

#include "GameWorld/GLoop.hpp"
#include "YC/Core/NetPacket/Packets.hpp"
#include "YC/Coroutine/Coro.hpp"

#include <vector>

#include "Static\ResourceMap\ResSound.h"
#include "Static\ResourceMap\ResWidget.h"

void AGameLoop::SetNetPc(ANetPC* InNetPc) {
	NetPC = InNetPc;
	bIsServer = InNetPc->HasAuthority();
}

AGameLoop::AGameLoop() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	
	Res::LoadCharacter();
	Res::LoadWidget();
	Res::LoadSound();
}

void AGameLoop::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	Panic = [](const std::string& Msg) {
		UE_LOG(LogTemp, Error, TEXT("%s"), *FString(Msg.c_str()));
		checkf(0, L"스텍트레이스 잘 읽어라...{ Err : %s }", *FString(Msg.c_str()));
	};
	
	if(NetPC == nullptr) return;

	if(IsValid(Target))	SetActorLocation(Target->GetActorLocation());
	
	if(!bIsLoadComplete) {
		ClientPacketQueue = clnt_packet_queue(1000);
		const auto NewWorld = ClientLoad(this, &PacBuf, NetPC, ClientPacketQueue);

		if(bIsServer) {
			ServerPacketQueue = srv_packet_queue(1000);
			YC::Server::ServerLoad(this, ServerPacketQueue);
		}
		
		World = NewWorld;
		Coro::CoInit();
		bIsLoadComplete = true;
	}
	
	if(bIsLoadComplete) {
		if(bIsServer) {
			YC::Server::ServerTick(this);
			YC::Server::ServerRenderTick(this);
		}
		
		World = ClientTick(this, PacBuf, World);
		World = ClientRenderTick(this, World);
		PacBuf.Clear();
	}

	Coro::UpdateTime(GetWorld()->GetTimeSeconds());
	Coro::CoLoop();
}

void AGameLoop::Tick2(const float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if(GetWorld()->GetFirstPlayerController() == nullptr) return;
	
	std::call_once(StartFlag, [self = this] {
		YC::Log::Push("__packet_events resized!");
		
		auto [StartTime, World] = GameStart(self, {}, &self->PacBuf);

		YC::Log::Push("__packet_bind end!");
		self->StartTime = StartTime;
		self->World = World;
	});
	
	auto [Tick, NewWorld, GameEnded] = GameLoop(StartTime, TickCount, &PacBuf, World, this);
	
	TickCount = Tick;
	World = NewWorld;

	Coro::UpdateTime(GetWorld()->GetTimeSeconds());
	Coro::CoLoop();
}

void AGameLoop::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if(!HasAuthority()) return;
	GameEnded();
	if(NetPC != nullptr) NetPC->DestroySession();
}