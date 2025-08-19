// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dongjun/Monster/Monster.h"
#include "GameFramework/Actor.h"
#include "Serialization/Csv/CsvParser.h"
#include "MobSpawner.generated.h"


UCLASS()
class NIRVANA_API AMobSpawner : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMobSpawner();

	enum EDataInfo {
		SharedOrUnique,
		MobID,
		MobCount,
	};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	
	UPROPERTY(EditAnywhere)
	int32 BattleFiledID;
	bool bStartSpawn = false;
	bool bIsSpawned = true;
	
	std::function<void(TArray<AMonster*>)> Callback;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="MobSpawner")
	AMonster* SpawnMonster(TSubclassOf<AMonster> BP_Monster) const;
	
	void BeforeSpawn(std::function<void(TArray<AMonster*>)> Callback);
	
	float SpawnTimer = 1.f;
	TArray<AMonster*> MonsterList;
	TQueue<int32> MonsterQueue;
	
	TArray<int> MobKey;
	TArray<int> MobCount_;
	
	UPROPERTY(EditAnywhere)
	TMap<int, TSubclassOf<AMonster>> BP_Monsters;
};
