// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetGameInstance.h"
#include "NrvGameInst.generated.h"

/**
 * 
 */
UCLASS()
class NIRVANA_API UNrvGameInst : public UNetGameInstance {
	GENERATED_BODY()

public:
	int32 CurPlayerNum = 0;
};
