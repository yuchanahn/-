#pragma once
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Engine/DataTable.h"

#include "YC/ErrorOr/ErrorOr.hpp"
#include "YC/Core/ActorIndexingComp.h"
#include "YC/Core/Log/Log.hpp"
#include "YC/Core/Ett/Ett_Type.hpp"
#include "YC/Core/Utils/Functional.hpp"
#include "YC/Core/Static/Static.hpp"
#include "YC/Type/DBType.h"
#include "YCBase/Entity.h"

#include "Taehun/NirvanaPadmaCharacter.h"
#include "../Static/ResourceMap/ResCharacter.h"

namespace YC::Sys::Chr {

static ErrorOr<FETT> SpawnPlayer(const UObject* G, const int32 EttIdx, const Res::EPlayer PlayerType) {
	const FSpawnPlayerPos_DB* SpawnData = nullptr;
	if(const auto DT = Cast<UDataTable>(Res::DB_PlayerSpawnPos.Get())) {
		SpawnData = DT->FindRow<FSpawnPlayerPos_DB>(PlayerType == Res::Padma ? "Padma"
																			 : "Feama", TEXT("Spawn"), true);
	}
	const auto Tr = FTransform {FRotator::ZeroRotator, SpawnData ? SpawnData->SpawnPos : FVector::ZeroVector};

	const auto Chr = SpawnActorDef<ACharacter>(G, (PlayerType == Res::Padma ? Res::BP_Padma : Res::BP_Feama), Tr);
	Chr | FinishSpawningActor(Tr) | WhenErr | Log::Push_Dp_S;

	auto ActorIndexing = Chr | YC_Cast<UObject>
							 | AddComp<UActorIndexingComp>
							 | SetIndex_For_ActorIndexingComp(EttIdx)
							 | RegisterComponent;
	
	if(Chr.IsErr()) return Err{ Chr.GetError() };
	
	return FETT {
		.AChrPtr = Chr | YC_Cast<AEntity> | Unwrap,
		.Chr = FOtChr {.EttType = static_cast<EttType::EType>(PlayerType + 1)},
		.EttIdx = EttIdx,
	};
}

} //namespace YC::System::Character

