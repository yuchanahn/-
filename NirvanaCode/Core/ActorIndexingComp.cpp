#include "ActorIndexingComp.h"

#include "Net/UnrealNetwork.h"
#include "Server/Server.hpp"


UActorIndexingComp::UActorIndexingComp() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorIndexingComp::OnRep_Variable() {
}

void UActorIndexingComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UActorIndexingComp, OwnID);
}


void UActorIndexingComp::BeginPlay() {
	Super::BeginPlay();
	SetIsReplicated(true);
	YC::Server::OnNewEttSpawned(GetOwner());
}

void UActorIndexingComp::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	YC::Server::OnEttDead(GetOwner());
}

void UActorIndexingComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

