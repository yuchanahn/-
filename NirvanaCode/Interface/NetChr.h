// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetChr.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNetChr : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class NIRVANA_API INetChr {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void NetBegin() = 0;
	virtual void RPC_To(FString InKey) = 0;
};
