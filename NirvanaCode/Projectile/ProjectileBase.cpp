// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBase::AProjectileBase() {
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void AProjectileBase::BeginPlay() {
	Super::BeginPlay();

	ProjectileMovementComp->bSimulationEnabled = true;
	ProjectileMovementComp->InitialSpeed = Speed;
	ProjectileMovementComp->MaxSpeed = Speed;
}

void AProjectileBase::SetDir(const FVector& InDir) const {
	ProjectileMovementComp->Velocity = InDir * ProjectileMovementComp->InitialSpeed;
}

void AProjectileBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

