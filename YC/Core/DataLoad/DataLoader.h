// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataLoader.generated.h"

UCLASS()
class NIRVANA_API ADataLoader : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADataLoader();

 	inline static FString MobSpawnData;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
