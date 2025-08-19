#pragma once
#include "InputMappingContext.h"
#include "NiagaraSystem.h"
#include "Dongjun/Monster/Monster.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "YC/Core/Ett/Ett_Type.hpp"

#include "ResCharacter.generated.h"

#define RES_WRAPPER(STRUCT, VAR, VAL) \
	STRUCT.VAR = VAL; \
	VAR = VAL;
	
USTRUCT()
struct FResCharacterStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<ACharacter> BP_Padma;
	UPROPERTY()
	TSubclassOf<ACharacter> BP_Feama;

	UPROPERTY()
	TSoftObjectPtr<UDataTable> DB_PlayerSpawnPos;
	
	UPROPERTY()
	TSoftObjectPtr<UDataAsset> DA_ServerSetting;

	UPROPERTY()
	TObjectPtr<UTexture2D> T_Portrait_Padma;
	UPROPERTY()
	TObjectPtr<UTexture2D> T_Portrait_Faema;

	UPROPERTY()
	TObjectPtr<UAnimMontage> AM_Padma_Death;
	UPROPERTY()
	TObjectPtr<UAnimMontage> AM_Faema_Death;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> NS_Burn;

	UPROPERTY()
	TSubclassOf<AMonster> BP_Monster_Gluttony_Melee;
	UPROPERTY()
	TSubclassOf<AMonster> BP_Monster_Gluttony_Ranged;
	UPROPERTY()
	TSubclassOf<AMonster> BP_Monster_Bambam;
};

namespace Res {

inline FResCharacterStruct ResCharacter;
	
inline TSubclassOf<ACharacter> BP_Padma;
inline TSubclassOf<ACharacter> BP_Feama;

inline TSoftObjectPtr<UDataTable> DB_PlayerSpawnPos;
inline TSoftObjectPtr<UDataAsset> DA_ServerSetting;

inline TObjectPtr<UTexture2D> T_Portrait_Padma;
inline TObjectPtr<UTexture2D> T_Portrait_Faema;

inline TObjectPtr<UAnimMontage> AM_Padma_Death;
inline TObjectPtr<UAnimMontage> AM_Faema_Death;

inline TObjectPtr<UNiagaraSystem> NS_Burn;

inline TSubclassOf<AMonster> BP_Monster_Gluttony_Melee;
inline TSubclassOf<AMonster> BP_Monster_Gluttony_Ranged;
inline TSubclassOf<AMonster> BP_Monster_Bambam;

enum EPlayer {
	Padma,
	Feama,
	Num,
};

inline void LoadCharacter() {
	using Find_Imc = ConstructorHelpers::FObjectFinder<UInputMappingContext>;
	using Find_IA =  ConstructorHelpers::FObjectFinder<UInputAction>;
	using Find_Character = ConstructorHelpers::FClassFinder<ACharacter>;
	using Find_Monster = ConstructorHelpers::FClassFinder<AMonster>;
	using Find_DB = ConstructorHelpers::FObjectFinder<UDataTable>;
	using Find_DA = ConstructorHelpers::FObjectFinder<UDataAsset>;
	using Find_T = ConstructorHelpers::FObjectFinder<UTexture2D>;
	using Find_AM = ConstructorHelpers::FObjectFinder<UAnimMontage>;
	using Find_NS = ConstructorHelpers::FObjectFinder<UNiagaraSystem>;
	
	RES_WRAPPER(ResCharacter, DB_PlayerSpawnPos, Find_DB(L"/Game/01_Source/Yuchan/DB/DT_SpawnPlayer").Object);
	RES_WRAPPER(ResCharacter, DA_ServerSetting, Find_DA(L"/Game/01_Source/Yuchan/DB/DA_ServerSetting").Object);
	
	RES_WRAPPER(ResCharacter, BP_Padma, Find_Character(L"/Game/01_Source/Taehun/Character/BP_Padma").Class);
	RES_WRAPPER(ResCharacter, BP_Feama, Find_Character(L"/Game/01_Source/Dongyun/Character/BP_Faema").Class);
	
	RES_WRAPPER(ResCharacter, T_Portrait_Padma, Find_T(L"/Game/01_Source/Dongyun/UI/FixTexture/T_Portrait_Padma").Object);
	RES_WRAPPER(ResCharacter, T_Portrait_Faema, Find_T(L"/Game/01_Source/Dongyun/UI/FixTexture/T_Portrait_Faema").Object);

	RES_WRAPPER(ResCharacter, AM_Padma_Death, Find_AM(L"/Game/01_Source/Taehun/Character/Animation/Montage/AM_Padma_Death").Object);
	RES_WRAPPER(ResCharacter, AM_Faema_Death, Find_AM(L"/Game/01_Source/Dongyun/Character/Animations/Montage/AM_Faema_Death").Object);

	RES_WRAPPER(ResCharacter, NS_Burn, Find_NS(L"/Game/02_Graphics/Effect/J/Niagara/All_FireBuff/NS_All_FireBuff_01").Object);

	RES_WRAPPER(ResCharacter, BP_Monster_Gluttony_Melee, Find_Monster(TEXT("/Game/01_Source/Dongjun/Monster/Gluttony/Prefab/BP_Monster_Gluttony_Melee")).Class);
	RES_WRAPPER(ResCharacter, BP_Monster_Gluttony_Ranged, Find_Monster(TEXT("/Game/01_Source/Dongjun/Monster/Gluttony/Prefab/BP_Monster_Gluttony_Ranged")).Class);
	RES_WRAPPER(ResCharacter, BP_Monster_Bambam, Find_Monster(TEXT("/Game/01_Source/Dongjun/Monster/Bambam/Prefab/BP_Monster_Bambam_Base")).Class);
}

#undef RES_WRAPPER
	
inline UTexture2D* LoadPortrait(EttType::EType Type) {
	switch (Type) {
	case EttType::Padma: return T_Portrait_Padma;
	case EttType::Faema: return T_Portrait_Faema;
	default: return nullptr;
	}
}
	
}