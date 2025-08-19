#pragma once
#include <variant>
#include <YC/ErrorOr/ErrorOr.hpp>
#include "YCBase/Entity.h"

namespace EttType {
enum EType {
	None,
	Padma,
	Faema,
	Gluttony,
	Wall,
};
}

struct FStatus {
	float Hp = 0;
	float HpMax = 0;
};

struct FNoneChrSet {
};

struct FMyChr {
	FStatus Stt { 1, 1 };
	EttType::EType EttType = EttType::None;
};

struct FOtChr {
	FStatus Stt { 1, 1 };
	EttType::EType EttType = EttType::None;
};

struct FMob {
	FStatus Stt { 1, 1 };
	EttType::EType EttType = EttType::None;
};

struct FWall {
	FStatus Stt { 1, 1 };
	EttType::EType EttType = EttType::None;
};

using FChr = std::variant<FNoneChrSet, FMyChr, FOtChr, FMob, FWall>;

namespace EttStateType {
enum EType {
	UnwrapNullPtr,
	None,
	BeforeSpawn,
	Spawn,
	Dead,
	Disconnect,
};
}

struct FETT {
	ErrorOr<AEntity*, EttStateType::EType> AChrPtr { EttStateType::None };
	FChr Chr = FNoneChrSet {};
	int32 EttIdx;
};