#pragma once

#include "CoreMinimal.h"
#include "BattleZoneWall.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ItemObject.generated.h"

UCLASS()
class NIRVANA_API AItemObject : public AActor {
	GENERATED_BODY()

public:
	AItemObject();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YC|Mesh")
	TSubclassOf<AActor> SpawnTargetMesh = nullptr;

	TArray<ABattleZoneWall*> Walls;
	
	UPROPERTY()
	AActor* MyMesh = nullptr;
	virtual void Tick(float DeltaTime) override;
	bool IsStart = false;
	UFUNCTION(BlueprintCallable)
	void RegisterObject();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStart();
	
	UFUNCTION(BlueprintCallable)
	virtual void Start();

	UFUNCTION(NetMulticast, Reliable)
	void NMC_Start();
	
	UFUNCTION(BlueprintCallable)
	void OpenObject();
};