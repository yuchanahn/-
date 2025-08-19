// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Object.h"
#include "DBType.generated.h"

USTRUCT(BlueprintType)
struct FSpawnPlayerPos_DB : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int8 PlayerType;

	UPROPERTY(EditAnywhere)
	FVector SpawnPos;
};