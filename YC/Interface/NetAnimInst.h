// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "YC/NetAnim.h"
#include "NetAnimInst.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNetAnimInst : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class NIRVANA_API INetAnimInst {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Setup(UNetAnim* NetAnim) = 0;
};
