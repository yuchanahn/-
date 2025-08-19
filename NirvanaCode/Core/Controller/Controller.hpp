#pragma once

#include <YC/ErrorOr/ErrorOr.hpp>

#include "GameFramework/Character.h"
#include "YC_UE_Template01/NetPacket/PacketAction.hpp"
#include "YC_UE_Template01/NetPacket/PacketTypes.h"

namespace YC::Sys::Controller {

inline std::vector<APlayerController*> Controllers;

inline ErrorOr<void> PossessTo(APlayerController* Controller, ACharacter* InChr) {
	if (IsValid(InChr)) {
		if(IsValid(Controller)) {
			Controller->Possess(InChr);
			return {};
		}
		return Err { std::string("Controller is not valid") };
	}
	return Err { std::string("Character is not valid") };
}

}
