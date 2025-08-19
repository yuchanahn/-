#include "NetPC.h"

#include "GameLoop.h"
#include "GameWorld/GLoop.hpp"
#include "NetPacket/Packets.hpp"
#include "Server/Server.hpp"

void ANetPC::BeginPlay() {
	Super::BeginPlay();
	if(!HasAuthority()) return; 
	if(YC::Server::FServer::IsServerInit) {
		YC::Server::OnClientConnect(this);
		bInit = true;
	} else {
		StartOnTick = true;
	}
}

void ANetPC::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	
	if(HasAuthority() && YC::Server::FServer::IsServerInit && StartOnTick && !bInit) {
		YC::Server::OnClientConnect(this);
		bInit = true;
	}
	
	if(!bGameLoopCheck) {
		for(const auto Actor : YC::GetAllActors(this)) {
			if(const auto GL = Cast<AGameLoop>(Actor)) {
				GL->SetNetPc(this);
				GameLoop = GL;
				bGameLoopCheck = true;
				break;
			}
		}
	}
}

void ANetPC::Destroyed() {
	Super::Destroyed();
	if(HasAuthority()) {
		YC::Server::OnClientDisconnect(this);
	}
}

ANetPC::ANetPC() {
	PrimaryActorTick.bCanEverTick = true;
}

void ANetPC::SendPacketToServer(const uint8 ID, const TArray<uint8>& Packet) const {
	S_ServerRecvPacket(ID, Packet);
}

void ANetPC::SendPacketToClient(const uint8 ID, const TArray<uint8>& Packet) const {
	C_ClientRecvPacket(ID, Packet);
}

void ANetPC::DestroySession() const {
	auto GInst = Cast<UNetGameInstance>(GetGameInstance());
	GInst->DestroySession(GInst->CurrentSessionName);
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++
		 Iterator) {
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController) {
			PlayerController->ClientReturnToMainMenuWithTextReason(
				FText::FromString(TEXT("Server closed the connection.")));
		}
	}
}

void ANetPC::GiveMeItem(TArray<int> ItemIds)
{
	YC::Server::Test_AddItem(ItemIds, this);
}

void ANetPC::YC_SendFadeOut() {
	YC::Server::Move2NextLv(this);
}

void ANetPC::SetIgnoreMoveInput(bool bNewMoveInput) {
	Super::SetIgnoreMoveInput(bNewMoveInput);
	C_SetIgnoreMoveInput(bNewMoveInput);
}

void ANetPC::C_SetIgnoreMoveInput_Implementation(bool bNewMoveInput) {
	YC::Log::Push_Err(L"C_SetIgnoreMoveInput_ImplementationC_SetIgnoreMoveInput_ImplementationC_SetIgnoreMoveInput_Implementation");
	Super::SetIgnoreMoveInput(bNewMoveInput);
}

void ANetPC::C_ClientRecvPacket_Implementation(const uint8 ID, const TArray<uint8>& Packet) const{
	if(!HasAuthority()) 
		YC::Log::Push_Dp(FString("C_ClientRecvPacket_Implementation : ") + FString::FromInt(ID));
	
	__call_packet_event(Packet, ID, GameLoop->ClientPacketQueue) | WhenErr | YC::Log::Push_Err_S;
}

void ANetPC::S_ServerRecvPacket_Implementation(const uint8 ID, const TArray<uint8>& Packet) const{
	auto ClntID = YC::Server::GetClientID(this);
	ClntID | WhenErr | YC::Log::Push_Err_S;
	if(ClntID.IsErr()) return;
	__server__call_packet_event(Packet, ID, ClntID.Unwrap(), GameLoop->ServerPacketQueue) | WhenErr | YC::Log::Push_Err_S;
}