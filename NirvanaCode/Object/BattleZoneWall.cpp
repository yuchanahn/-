// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleZoneWall.h"

#include "Components/BoxComponent.h"
#include "YC/Core/Log/Log.hpp"


// Sets default values
ABattleZoneWall::ABattleZoneWall() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Wall = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall"));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABattleZoneWall::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABattleZoneWall::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void ABattleZoneWall::NMC_UnBlockAll_Implementation() {
	if(!HasAuthority()) YC::Log::Push("Client: UnBlockAll");
	Wall->SetCollisionProfileName(TEXT("NoCollision"));
}

void ABattleZoneWall::NMC_BlockAll_Implementation() {
	if(!HasAuthority()) YC::Log::Push("Client: BlockAll");
	Wall->SetCollisionProfileName(TEXT("BlockAll"));
}

