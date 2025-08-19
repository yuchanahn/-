#pragma once

#include "CoreMinimal.h"
#include "Controller/NrvController.h"
#include "Dongyun/PlayerBase.h"
#include "GameFramework/PlayerController.h"
#include "NetPC.generated.h"

UCLASS()
class NIRVANA_API ANetPC : public ANrvController {
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;
public:
	ANetPC();
	void SendPacketToServer(uint8 ID, const TArray<uint8>& Packet) const;
	void SendPacketToClient(const uint8 ID, const TArray<uint8>& Packet) const;

	void DestroySession() const;

	bool bInit = false;
private:
	bool StartOnTick = false;
	bool bGameLoopCheck = false;
	
	UPROPERTY()
	class AGameLoop* GameLoop = nullptr;

	UFUNCTION(Server, Reliable)
	void S_ServerRecvPacket(uint8 ID, const TArray<uint8>& Packet) const;
	
	UFUNCTION(Client, Reliable)
	void C_ClientRecvPacket(uint8 ID, const TArray<uint8>& Packet) const;

	UFUNCTION(Exec, Category="Commands")
	void GiveMeItem(TArray<int> ItemIds);

	UFUNCTION(Exec, Category="Commands")
	void YC_SendFadeOut();

	virtual void SetIgnoreMoveInput(bool bNewMoveInput) override;
	
	UFUNCTION(Client, Reliable)
	void C_SetIgnoreMoveInput(bool bNewMoveInput);

	
};