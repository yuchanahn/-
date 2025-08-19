#include "GM_GameWaitingRoom.h"

#include "NetGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "YC/LobbyController.h"
#include "YC/Core/Log/Log.hpp"

AGM_GameWaitingRoom::AGM_GameWaitingRoom() {
	PrimaryActorTick.bCanEverTick = true;
}

void AGM_GameWaitingRoom::BeginPlay() {
	Super::BeginPlay();
	GInst = Cast<UNetGameInstance>(GetGameInstance());
	IsServer = HasAuthority();
}

void AGM_GameWaitingRoom::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	PlayerControllerList.Add(NewPlayer);
	
	YC::Log::Push_Dp_S(std::format("NewPlayer : {}", GInst->GetUserNickname(NewPlayer)));
}

void AGM_GameWaitingRoom::Logout(AController* Exiting) {
	PlayerControllerList.Remove(Cast<APlayerController>(Exiting));
	Super::Logout(Exiting);
	YC::Log::Push_Dp_S(std::format("LogOutPlayer : {}", GInst->GetUserNickname(Cast<APlayerController>(Exiting))));
}

void AGM_GameWaitingRoom::OnGameStartFn_Implementation() { }
void AGM_GameWaitingRoom::OnGameEndFn_Implementation() { }

void AGM_GameWaitingRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

APlayerController* AGM_GameWaitingRoom::ProcessClientTravel(FString& URL, bool bSeamless, bool bAbsolute) {
	for(const auto Player : PlayerControllerList) {
		if(Cast<ALobbyController>(Player) == nullptr) {
			YC_LOG_DP("Cast<ALobbyController>(Player) == nullptr");
			continue;
		}
		Cast<ALobbyController>(Player)->OpenLoadingScreen();
	}
	return Super::ProcessClientTravel(URL, bSeamless, bAbsolute);
}

void AGM_GameWaitingRoom::Server_StartGame() {
	OnGameStartFn();
	//GetWorld()->LoadStreamLevel(InGameLevel);
	//UGameplayStatics::LoadStreamLevel(GetWorld(), FName(InGameLevel), true, true, FLatentActionInfo());
	GetWorld()->ServerTravel(InGameLevel);
}

void AGM_GameWaitingRoom::Server_LeaveRoom() {
	OnGameEndFn();
	GInst->DestroySession(GInst->CurrentSessionName);
}

void AGM_GameWaitingRoom::Client_LeaveRoom() {
	OnGameEndFn();
	GInst->DestroySession(GInst->CurrentSessionName);
	//GInst->LeaveSession();
}