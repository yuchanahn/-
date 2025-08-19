// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ServerSetting.generated.h"


UENUM()
enum class ESpawnCharacterType {
	Padma,
	Faema
};

UCLASS()
class NIRVANA_API UServerSetting : public UDataAsset {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "YC|ServerSetting")
	ESpawnCharacterType ServerCharacter;
	
	UPROPERTY(EditAnywhere, Category = "YC|ServerSetting")
	ESpawnCharacterType ClientCharacter;
};
