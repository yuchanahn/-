#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetAnim.generated.h"

USTRUCT()
struct FPauseMontage {
	GENERATED_BODY()

	UPROPERTY()
	UAnimMontage* Montage = nullptr;
	UPROPERTY()
	float PauseTime = 0.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NIRVANA_API UNetAnim : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNetAnim();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	
	// Server RPC
	std::function<void()> OnAttackHitCheck;
	std::function<void()> OnMotionWarpingUpdate;
	std::function<void()> OnJumpCheck;
	// Multycast RPC
private:
	UPROPERTY()
	UAnimInstance* AnimInst = nullptr;
	
	UPROPERTY()
	ACharacter* Owner = nullptr;

	UPROPERTY()
	TMap<FName, uint8> MontageMap;
	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<UAnimMontage*> MontageAssetMap;
	
	uint8 CurrentMontage = 0;
	uint8 NET_Montage = 0;
	int64 CurrentStartTime = 0;
	int32 CurrentSectionIdx = 0;
	int32 NET_SectionIdx = -1;

	bool bNET_IsPlaying = true;

	FAnimMontageInstance* GetMontageInst(UAnimMontage* Mtg) const;
	
	UFUNCTION(Server, Reliable)
	void S_PlayMontage(uint8 InMontage, int64 InStartTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void NMC_PlayMontage(uint8 InMontage, int64 InStartTime);

	UFUNCTION(Server, Reliable)
	void S_PauseMontage(uint8 InMontage, int64 InPauseStartTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void NMC_PauseMontage(uint8 InMontage, int64 InPauseStartTime);

	UFUNCTION(Server, Reliable)
	void S_ResumeMontage(uint8 InMontage, int64 InResumeStartTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void NMC_ResumeMontage(uint8 InMontage, int64 InResumeStartTime);

	UFUNCTION(Server, Reliable)
	void S_JumpSectionMontage(uint8 InMontage, int32 InIndex, int64 InStartTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void NMC_JumpSectionMontage(uint8 InMontage, int32 InIndex, int64 InStartTime);

	int64 GetTime() const;
public:
	UFUNCTION(Server, Reliable)
	void S_StartMontage();
	
	bool bIsActive = true;

public:	
	UFUNCTION(Server, Reliable)
	void S_Notify_AttackHitCheck();
	
	UFUNCTION(Server, Reliable)
	void S_Notify_MotionWarpingUpdate();
	
	UFUNCTION(Server, Reliable)
	void S_Notify_JumpCheck();
};

