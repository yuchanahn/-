#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_GameWaitingRoom.generated.h"

class UNetGameInstance;

UENUM()
enum EGameWaitingRoomState {
	Waiting,
	Ready,
	Start,
};

UCLASS()
class NIRVANA_API AGM_GameWaitingRoom : public AGameModeBase {
	GENERATED_BODY()


	bool bIsGameStart = false;

	UPROPERTY()
	UNetGameInstance* GInst;
public:
	AGM_GameWaitingRoom();
	
	UPROPERTY(BlueprintReadWrite, Category = "YC|World")
	FString InGameLevel;

	UPROPERTY(BlueprintReadWrite, Category = "YC|World")
	bool IsServer = false;

	UPROPERTY(BlueprintReadWrite, Category = "YC|World")
	TArray<APlayerController*> PlayerControllerList;
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	
	UFUNCTION(BlueprintNativeEvent)
	void OnGameStartFn();

	UFUNCTION(BlueprintNativeEvent)
	void OnGameEndFn();
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual APlayerController* ProcessClientTravel(FString& URL, bool bSeamless, bool bAbsolute) override;
	
	UFUNCTION(BlueprintCallable)
	void Server_StartGame();
	
	UFUNCTION(BlueprintCallable)
	void Server_LeaveRoom();
	
	UFUNCTION(BlueprintCallable)
	void Client_LeaveRoom();
};
