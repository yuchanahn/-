// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Updrafts.generated.h"

UCLASS()
class NIRVANA_API AUpdrafts : public AActor {
	GENERATED_BODY()
	
public:
	AUpdrafts();

	UPROPERTY(EditAnywhere, Category = "YC|Gimmick|Updrafts|Component")
	class UArrowComponent* Arrow;
	UPROPERTY(EditAnywhere, Category = "YC|Gimmick|Updrafts|Component")
	class UBoxComponent* Box;
	UPROPERTY(EditAnywhere, Category = "YC|Gimmick|Updrafts|Component")
	class UNiagaraComponent* Niagara;
protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;

	
};
