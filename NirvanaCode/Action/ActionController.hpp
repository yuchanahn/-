#pragma once
#include <YC/ErrorOr/ErrorOr.hpp>

#include "Dongyun/Faema/NirvanaFaemaCharacter.h"
#include "Taehun/NirvanaPadmaCharacter.h"
#include "YC/Core/Server/Server.hpp"

enum EChrAction {
	PrimaryAttack,
};



struct FActionTriggerInfo{
    FString ActionName;
	int64 ActionInputTime;
	int32 Target;
};

// 임시 : 나중에 사이드 이펙트 없는 함수로 바꿉시다.
ErrorOr<void> ActionTrigger(FActionTriggerInfo& ActionTrigger){
	ACharacter* PlayerCharacter = YC::Server::PCs[ActionTrigger.Target]->GetPawn<ACharacter>();
	
	
	
}