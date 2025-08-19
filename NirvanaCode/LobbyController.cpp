#include "LobbyController.h"

#include "NetGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Core/Static/ResourceMap/ResWidget.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/GM_GameWaitingRoom.h"


ALobbyController::ALobbyController() {
	PrimaryActorTick.bCanEverTick = true;
}

void ALobbyController::BeginPlay() {
	Super::BeginPlay();
}

void ALobbyController::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void ALobbyController::OpenLoadingScreen_Implementation() {
	YC_LOG_DP("Show LoadingScreen!!");
	const auto Widget = ::CreateWidget(this, Res::WBP_Loading);
	if(Widget != nullptr) {
		Widget->AddToViewport();
	} else {
		YC_LOG_DP("Widget is nullptr!!");
	}
}

void ALobbyController::NMC_SendChat_Implementation(const FString& Msg) {
	OnChatReceived.Broadcast(Msg);
}
void ALobbyController::S_SendChat_Implementation(const FString& Msg) {
	const auto Gm = Cast<AGM_GameWaitingRoom>(GetWorld()->GetAuthGameMode());
	auto Sender = Cast<UNetGameInstance>(GetGameInstance())->GetUserNickname(this);
	for(const auto Pc : Gm->PlayerControllerList) {
		Cast<ALobbyController>(Pc)->NMC_SendChat(Sender.Append(" : ") + Msg);
	}
}

void ALobbyController::UpdateUserList_Implementation(const TArray<FString>& InUserList) {
	UserList = InUserList;
}

