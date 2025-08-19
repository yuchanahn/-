// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NrvController.generated.h"

/**
 * 
 */
UCLASS()
class NIRVANA_API ANrvController : public APlayerController {
	GENERATED_BODY()
	//TODO:
	
	enum MouseState {
		MS_Common,
		MS_Pressed,
		MS_Cursor
	};
	
public:
	MouseState GetMouseState() const;
};
