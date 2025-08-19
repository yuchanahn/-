#include "YC/NetAnim.h"

#include <filesystem>
#include <fstream>
#include <YC/Time/Time.hpp>

#include "NetGameInstance.h"
#include "Time.h"
#include "Anim/YCAnim.hpp"
#include "GameFramework/Character.h"

UNetAnim::UNetAnim() {
	PrimaryComponentTick.bCanEverTick = true;
	static std::vector<ConstructorHelpers::FObjectFinder<UAnimMontage>> M{
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_JudgeAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_ThunderAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Charge0Attack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_PowerSwingAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Charge1Attack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Charge2Attack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Charge3Attack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_ChargeAttackIdle",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_ChargeAttackRun",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_ChargeAttackStart",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Death",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Dash",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Hit",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_MoveAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_UtilSkill",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_PrimaryAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_DashAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_JumpAttack",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_JumpStart",
		L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_StartIdle",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Spawn",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Idle",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Falling",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Falling_Hit",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Landing",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Landing_Hit",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_HitUp",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_HitDown",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_HitLeft",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_FreeMove",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_Tracking",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_AttackSnap",
		L"/Game/01_Source/Dongjun/Monster/Gluttony/Animation/Montage/AM_Gluttony_AttackIceSpit",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Spawn",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Idle",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Falling",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Falling_Hit",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Landing",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Landing_Hit",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_HitUp",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_HitDown",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_HitLeft",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Tracking_HopLeft",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Tracking_HopRight",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_Tracking",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackSnowball",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackPull",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackSpin_Start",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackSpin_JumpF",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackSpin_Spin",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackSpin_JumpB",
		L"/Game/01_Source/Dongjun/Monster/Bambam/Animation/Montage/AM_Bambam_AttackSpin_End",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_FaemaChargeAttack",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_FaemaPrimaryAttack",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_FaemaUniqueAttack",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_FaemaMoveAttack",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_FaemaMagicBomb",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_Faema_Hit",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_Faema_FightHit_1",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_Faema_FightHit_2",
		L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_Faema_Death",
		
	};
	for (auto& i : M) MontageAssetMap.Add(i.Object);
}

void UNetAnim::BeginPlay() {
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());
	if (Owner) { AnimInst = Owner->GetMesh()->GetAnimInstance(); }
	for (int i = 0; i < MontageAssetMap.Num(); ++i) { MontageMap.Add(MontageAssetMap[i]->GetFName(), i + 1); }
	MontageAssetMap.Insert(nullptr, 0);
}

void UNetAnim::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner->HasAuthority()) { return; }
	//if (!Owner->IsLocallyControlled()) { return; }
	if (!bIsActive) return;

	const auto TT = GetTime();

	const auto Montage = YC::Anim::GetCurMontage(AnimInst);
	bool bIsMontagePaused = false;

	if (Montage) {
		checkf(MontageMap.Contains(Montage->GetFName()), *Montage->GetFName().ToString());
		CurrentMontage = MontageMap[Montage->GetFName()];

		// TODO: Pause 한번만 실행해야함! IsPaused같은거 만들어야 할듯.
		bIsMontagePaused = AnimInst->Montage_GetIsStopped(Montage);
		const auto SectionName = AnimInst->Montage_GetCurrentSection(Montage);
		CurrentSectionIdx = MontageAssetMap[CurrentMontage]->GetSectionIndex(SectionName);
	}
	else { CurrentMontage = 0; }

	// 몽타주는 안바뀌고 섹션만 이동 했을 경우.
	if (CurrentMontage == NET_Montage &&
		CurrentSectionIdx != NET_SectionIdx) {
		CurrentStartTime = TT;
		NET_SectionIdx = CurrentSectionIdx;
		S_JumpSectionMontage(CurrentMontage, CurrentSectionIdx, TT);
		return;
	}

	// 몽타주 재생중이었는데, 멈추었다면.
	if (bNET_IsPlaying && bIsMontagePaused) {
		bNET_IsPlaying = false;
		CurrentStartTime = TT;
		NMC_PauseMontage(CurrentMontage, CurrentStartTime);
		return;
	}

	// 몽타주 멈춰있었는데, 재생되었다면.
	if (!bNET_IsPlaying && !bIsMontagePaused) {
		bNET_IsPlaying = true;
		CurrentStartTime = TT;
		S_ResumeMontage(CurrentMontage, CurrentStartTime);
		return;
	}

	// Montage가 바뀐 것을 감지하고 바뀌었다면, 패킷을 보냅니다!
	if (CurrentMontage != NET_Montage) {
		bNET_IsPlaying = true;
		NET_Montage = CurrentMontage;
		if (CurrentMontage != 0) {
			CurrentStartTime = TT + YC::Anim::ToMs(YC::Anim::GetMontageCurTime(AnimInst, Montage));
		}
		else { CurrentStartTime = TT; }
		S_PlayMontage(CurrentMontage, CurrentStartTime);
	}
}

void UNetAnim::InitializeComponent() {
	Super::InitializeComponent();

	SetIsReplicated(true);
}

FAnimMontageInstance* UNetAnim::GetMontageInst(UAnimMontage* Mtg) const {
	for(const auto i : AnimInst->MontageInstances) {
		if(i->Montage == Mtg) return i;
	}
	return nullptr;
}

int64 UNetAnim::GetTime() const {
	static UNetGameInstance* GInst = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
	return GInst->LocalData.TimeOffset + YC::Time::Epoch_Ms();
}

void UNetAnim::S_StartMontage_Implementation() {
	if (CurrentMontage == 0) return;
	NMC_PlayMontage(CurrentMontage, GetTime());
}

// ========================================================

void UNetAnim::S_Notify_JumpCheck_Implementation() { if (OnJumpCheck) OnJumpCheck(); }
void UNetAnim::S_Notify_MotionWarpingUpdate_Implementation() { if (OnMotionWarpingUpdate) OnMotionWarpingUpdate(); }
void UNetAnim::S_Notify_AttackHitCheck_Implementation() { if (OnAttackHitCheck) OnAttackHitCheck(); }

// ========================================================

void UNetAnim::S_PlayMontage_Implementation(uint8 InMontage, int64 InStartTime) { NMC_PlayMontage(InMontage, InStartTime); }
void UNetAnim::S_JumpSectionMontage_Implementation(uint8 InMontage, int32 InIndex, int64 InStartTime) { NMC_JumpSectionMontage(InMontage, InIndex, InStartTime); }
void UNetAnim::S_ResumeMontage_Implementation(uint8 InMontage, int64 InResumeStartTime) { NMC_ResumeMontage(InMontage, InResumeStartTime); }
void UNetAnim::S_PauseMontage_Implementation(uint8 InMontage, int64 InPauseStartTime) { NMC_PauseMontage(InMontage, InPauseStartTime); }

// ========================================================


// NetMulticast 함수들입니다.

void UNetAnim::NMC_PlayMontage_Implementation(uint8 InMontage, int64 InStartTime) {
	if (Owner == nullptr) return;
	//if (Owner->IsLocallyControlled()) return;
	if (Owner->HasAuthority()) return;
	if (!IsValid(AnimInst)) return;
	
	const auto Prev = CurrentMontage;

	NET_Montage = InMontage;
	CurrentMontage = InMontage;
	CurrentStartTime = InStartTime;

	if (InMontage == 0) {
		if (Prev != 0) { AnimInst->Montage_Stop(0.0f, MontageAssetMap[Prev]); }
		return;
	}

	UAnimMontage* Montage = MontageAssetMap[NET_Montage];
	const float T = FMath::Clamp((GetTime() - InStartTime) * 0.001f * Montage->RateScale, 0.f, Montage->GetPlayLength());
	//AnimInst->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, T);
	AnimInst->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0);

	//FRootMotionMovementParams* Out = nullptr;
	//GetMontageInst(Montage)->Advance(T, Out, false);
}

void UNetAnim::NMC_JumpSectionMontage_Implementation(uint8 InMontage, int32 InIndex, int64 InStartTime) {
	if (NET_SectionIdx == InIndex) { return; }
	if (!IsValid(AnimInst)) return;
	
	CurrentStartTime = InStartTime;
	NET_SectionIdx = InIndex;
	CurrentSectionIdx = InIndex;
	

	const auto Montage = YC::Anim::GetCurMontage(AnimInst);

	if (!Montage) { return; }

	// 여기서 이전 섹션 시간 구해야함.
	const float T = FMath::Clamp((GetTime() - InStartTime) * 0.001f * Montage->RateScale, 0.f,
								 Montage->GetPlayLength());
	float PrevSectionLength = 0.f;
	for (int i = 0; i < InIndex; ++i) PrevSectionLength += Montage->GetSectionLength(i);

	const auto Name = Montage->GetSectionName(CurrentSectionIdx);
	AnimInst->Montage_JumpToSection(Name, Montage);
	//AnimInst->Montage_SetPosition(Montage, T + PrevSectionLength);
	AnimInst->Montage_SetPosition(Montage, PrevSectionLength);
	//FRootMotionMovementParams* Out = nullptr;
	//GetMontageInst(Montage)->Advance(T + PrevSectionLength, Out, false);
}

void UNetAnim::NMC_ResumeMontage_Implementation(uint8 InMontage, int64 InResumeStartTime) {
	if (bNET_IsPlaying) { return; }
	if (!IsValid(AnimInst)) return;

	bNET_IsPlaying = true;
	CurrentStartTime = InResumeStartTime;

	const auto Montage = YC::Anim::GetCurMontage(AnimInst);

	if (!Montage) { return; }

	if (AnimInst->Montage_GetIsStopped(Montage)) {
		const float T = FMath::Clamp((GetTime() - InResumeStartTime) * 0.001f * Montage->RateScale, 0.f,
		                             Montage->GetPlayLength());
		AnimInst->Montage_SetPosition(Montage, T);
		AnimInst->Montage_Resume(Montage);
	}
}

void UNetAnim::NMC_PauseMontage_Implementation(uint8 InMontage, int64 InPauseStartTime) {
	if (!bNET_IsPlaying) { return; }
	if (!IsValid(AnimInst)) return;
	
	bNET_IsPlaying = false;
	CurrentStartTime = InPauseStartTime;

	const auto Montage = YC::Anim::GetCurMontage(AnimInst);

	if (!Montage) { return; }

	if (!AnimInst->Montage_GetIsStopped(Montage)) {
		const float T = FMath::Max((GetTime() - InPauseStartTime) * 0.001f * Montage->RateScale, 0);
		AnimInst->Montage_SetPosition(Montage, T);
		AnimInst->Montage_Pause(Montage);
	}
}