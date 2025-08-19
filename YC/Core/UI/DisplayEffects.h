// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <YC/Coroutine/Coro.hpp>

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DisplayEffects.generated.h"

/**
 * 
 */
UCLASS()
class NIRVANA_API UDisplayEffects : public UUserWidget {
	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidget))
	class UImage* FadeImage = nullptr;
	
	float TargetOpacity = 0.f;

	static coroutine CoFade(UImage* Img, float TargetOpacity, UUserWidget* Widget);
	
	UFUNCTION(BlueprintCallable, Category = "YC|UI")
	void FadeIn();

	UFUNCTION(BlueprintCallable, Category = "YC|UI")
	void FadeOut();
};
