#include "YC/NetFlag.h"

#include "Net/UnrealNetwork.h"

UNetFlag::UNetFlag()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UNetFlag::BeginPlay()
{
	Super::BeginPlay();
}


void UNetFlag::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNetFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNetFlag, OwnID);
}

