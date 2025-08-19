#pragma once

#include <chrono>

#include "CoreMinimal.h"
#include "Client/Client.hpp"
#include "GameFramework/Actor.h"
#include "GameCharacter/CharacterSystem.hpp"

#include "GameLoop.generated.h"

namespace YC::Client {
struct FPacBuf;
}

UCLASS()
class NIRVANA_API AGameLoop : public AActor {
	GENERATED_BODY()
	
	std::once_flag StartFlag;
	std::chrono::time_point<std::chrono::steady_clock> StartTime;

	YC::Client::FPacBuf PacBuf;
	YC::Client::FWorld World;

	int TickCount = 0;

	UPROPERTY()
	ANetPC* NetPC = nullptr;

	bool bIsServer = false;
	bool bIsLoadComplete = false;


	
public:
	UPROPERTY()
	AActor* Target = nullptr;
	
	void SetNetPc(ANetPC* InNetPc);
	clnt_packet_queue ClientPacketQueue;
	srv_packet_queue ServerPacketQueue;
	AGameLoop();
	
	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	
	void Tick2(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};