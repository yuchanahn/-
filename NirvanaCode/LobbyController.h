// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Controller/NrvController.h"
#include "GameFramework/PlayerController.h"
#include "LobbyController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatReceived, FString, Msg);

UCLASS()
class NIRVANA_API ALobbyController : public ANrvController {
	GENERATED_BODY()

public:
	ALobbyController();

	UPROPERTY(BlueprintReadWrite, Category = "YC|World")
	TArray<FString> UserList;

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "YC|World")
	FOnChatReceived OnChatReceived;
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void UpdateUserList(const TArray<FString>& InUserList);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void S_SendChat(const FString& Msg);
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NMC_SendChat(const FString& Msg);

	UFUNCTION(NetMulticast, Reliable)
	void OpenLoadingScreen();
};