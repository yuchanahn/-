// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleZoneWall.h"
#include "ItemObject.h"
#include "MobSpawner.h"
#include "GameFramework/Actor.h"
#include "MobSpawnTrigger.generated.h"

UCLASS()
class NIRVANA_API AMobSpawnTrigger : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMobSpawnTrigger();

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	TArray<ABattleZoneWall*> Walls;

	UPROPERTY(EditAnywhere)
	AMobSpawner* Spawner;

	UPROPERTY(EditAnywhere)
	AItemObject* ItemObject;

	TArray<AMonster*> SpawnedMonsters;
	
protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
