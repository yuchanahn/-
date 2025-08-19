// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Move2NextLv.generated.h"

UCLASS()
class NIRVANA_API AMove2NextLv : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMove2NextLv();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
