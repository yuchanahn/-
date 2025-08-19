#pragma once
#include "FMODEvent.h"

#include "ResSound.generated.h"

#define RES_WRAPPER(STRUCT, VAR, VAL) \
	STRUCT.VAR = VAL; \
	VAR = VAL;

USTRUCT()
struct FResSoundStruct
{
	GENERATED_BODY()

	UPROPERTY()
	UFMODEvent* SFX_Padma_Hit;
	UPROPERTY()
	UFMODEvent* SFX_Padma_ChargingLoop;
	UPROPERTY()
	UFMODEvent* SFX_Padma_ChargingPoint;
	UPROPERTY()
	UFMODEvent* SFX_Faema_ChargingLoop;
	UPROPERTY()
	UFMODEvent* SFX_Faema_ChargingPoint;
	UPROPERTY()
	UFMODEvent* SFX_Faema_ChargingShot;
	UPROPERTY()
	UFMODEvent* SFX_Faema_Move;
	UPROPERTY()
	UFMODEvent* SFX_PL_Exp;

	UPROPERTY()
	UFMODEvent* SFX_UI_InvenSelect;
};

namespace Res {

inline FResSoundStruct ResSound;
	
inline UFMODEvent* SFX_Padma_Hit;
inline UFMODEvent* SFX_Padma_ChargingLoop;
inline UFMODEvent* SFX_Padma_ChargingPoint;
inline UFMODEvent* SFX_Faema_ChargingLoop;
inline UFMODEvent* SFX_Faema_ChargingPoint;
inline UFMODEvent* SFX_Faema_ChargingShot;
inline UFMODEvent* SFX_Faema_Move;
inline UFMODEvent* SFX_PL_Exp;

inline UFMODEvent* SFX_UI_InvenSelect;

inline void LoadSound() {
	using Find_SFX = ConstructorHelpers::FObjectFinder<UFMODEvent>;
	RES_WRAPPER(ResSound, SFX_Padma_Hit, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Common/SFX_PL_Damage").Object);
	RES_WRAPPER(ResSound, SFX_Padma_ChargingLoop,  Find_SFX(L"/Game/FMOD/Events/SFX/Player/Padma/SFX_PA_ChargingLoop").Object);
	RES_WRAPPER(ResSound, SFX_Padma_ChargingPoint, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Padma/SFX_PA_ChargingPoint").Object);
	RES_WRAPPER(ResSound, SFX_PL_Exp, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Common/SFX_PL_Exp").Object);
	RES_WRAPPER(ResSound, SFX_Faema_ChargingLoop, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Faema/SFX_FA_ChargingLoop").Object);
	RES_WRAPPER(ResSound, SFX_Faema_ChargingPoint, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Faema/SFX_FA_ChargingPoint").Object);
	RES_WRAPPER(ResSound, SFX_Faema_ChargingShot, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Faema/SFX_FA_ChargingShot").Object);
	RES_WRAPPER(ResSound, SFX_Faema_Move, Find_SFX(L"/Game/FMOD/Events/SFX/Player/Faema/SFX_FA_Move").Object);
	
	RES_WRAPPER(ResSound, SFX_UI_InvenSelect, Find_SFX(L"/Game/FMOD/Events/UI/SFX_UI_InvenSelect").Object);
}

#undef RES_WRAPPER
	
inline std::pair<std::vector<UFMODEvent*>, std::unordered_map<UFMODEvent*, int>> Load_SFX() {
	static std::vector SFXList = {
		SFX_Padma_Hit,
		SFX_Padma_ChargingLoop,
		SFX_Padma_ChargingPoint,
		SFX_Faema_ChargingLoop,
		SFX_Faema_ChargingPoint,
		SFX_Faema_ChargingShot,
		SFX_Faema_Move,
		SFX_PL_Exp,
	};
	static std::unordered_map<UFMODEvent*, int> SFXMap;
	if(SFXMap.empty()) {
		for (int i = 0; i < SFXList.size(); ++i) {
			SFXMap[SFXList[i]] = i;
		}
	}
	return { SFXList, SFXMap };
}

namespace FMod
{
inline int GetIdxOf(UFMODEvent* SFX) {
	return Load_SFX().second[SFX];
}
inline UFMODEvent* Load(int SFX) {
	return Load_SFX().first[SFX];
}
}
	
}
