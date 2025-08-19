#pragma once
#include "EOpt.h"

namespace YC::Anim {
struct FPlayAnimData {
	UAnimMontage* Montage;
	float CurAnimTime;
};

inline float GetPlayTime(const UAnimMontage* MT) { return MT->GetPlayLength() * MT->RateScale; };
inline int64 ToMs(const float F) { return F * 1000; };

inline void PlayAnim(UAnimInstance* InAnimInst, const FPlayAnimData& PlayAnimData) {
	check(InAnimInst != nullptr);

	InAnimInst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	InAnimInst->Montage_Play(PlayAnimData.Montage, 1, EMontagePlayReturnType::MontageLength, PlayAnimData.CurAnimTime, false);
}

inline void StopAnim(UAnimInstance* InAnimInst, const UAnimMontage* AnimMtg) {
	check(InAnimInst != nullptr);

	InAnimInst->Montage_Stop(0.25f, AnimMtg);
}

inline UAnimMontage* GetCurMontage(const UAnimInstance* InAnimInst) {
	check(InAnimInst != nullptr);
	return InAnimInst->GetCurrentActiveMontage();
}

inline float GetMontageCurTime(const UAnimInstance* InAnimInst, const UAnimMontage* InMontage) {
	check(InAnimInst != nullptr);
	check(InMontage != nullptr);
	
	return InAnimInst->Montage_GetPosition(InMontage);
}

inline void SetMontageCurTime(UAnimInstance* InAnimInst, const UAnimMontage* InMontage, const float InTime) {
	check(InAnimInst != nullptr);
	check(InMontage != nullptr);
	
	InAnimInst->Montage_SetPosition(InMontage, InTime);
};

}
