#pragma once

#include <YC/Coroutine/Coro.hpp>
#include <YC/ErrorOr/ErrorOr.hpp>

#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "NetGameInstance.h"

#include "YC/Core/Utils/Functional.hpp"
#include "YC/Core/Utils/String.hpp"
#include "YC/Core/Static/Static.hpp"
#include "..\Static\ResourceMap\ResWidget.h"
#include "YC/Core/Log/Log.hpp"
#include "YC/Core/NetPacket/PacketAction.hpp"
#include "YC/Core/NetPacket/PacketTypes.h"
#include "YC/Core/GameWorld/GWorld.hpp"
#include "YC/Core/Input/Input.hpp"
#include "YC/Core/GameCharacter/CharacterSystem.hpp"
#include "YC/Core/UI/UI.hpp"
#include "Dongjun/UI/Inventory/InventorySlotWidget.h"
#include "Dongjun/UI/Inventory/PlayerItemWidget.h"

#define PACKET_BIND(PacType) PacType::Bind([PacBuf](PacType Pac) { PacBuf->PlayerPacs.Add(Pac); }, Queue);
#define PACKET_BIND_GUI(PacType) PacType::Bind([PacBuf](PacType Pac) { PacBuf->GUIPacs.Add(Pac); }, Queue);

class UCanvasPanelSlot;

namespace YC::Client {

struct FPacBuf {
	TArray<Chanel_Player> PlayerPacs;
	TArray<Chanel_GUI> GUIPacs;
	void Clear() {
		PlayerPacs.Empty();
		GUIPacs.Empty();
	}
};

#define DefaultErr Err{ std::string("Not Implemented") }

struct FWorld {
	std::vector<FETT> Etts;

	// GUI //
	ErrorOr<UUserWidget*>
		HUD		 = DefaultErr;
	ErrorOr<YcButton>
		Btn_SelectItem = DefaultErr,
		Btn_InvClose   = DefaultErr;

	ErrorOr<FLevelUp*> LevelUp = DefaultErr;
	ErrorOr<FInventory*> Inventory = DefaultErr;
	ErrorOr<FInGameUIManager*> InGameUIManager = DefaultErr;
	
	// Time //
	ErrorOr<int64> TimeOffset = DefaultErr;
	bool IsCharacterPicked = false;
	FWorldStatus WorldStatus {};
	
	int32 WorldClientCount = 1;
	bool bHasMyCharacter = false;
	std::unordered_map<UUserWidget*, UUserWidget*> WidgetMap;

	bool CanSpawn = false;
};

static TMap<FString, FFMODEventInstance> EventInstMap;


static ErrorOr<AActor*> FindActorByEttIdx(const UObject* G, const int32& EttIdx) {
	//TODO: Sync문제
	// 플레이어 두명이 접속 했을 때 스폰 되게 하기.
	const auto Actors = GetWorld(G) | GetAllActors;
	if (Actors.IsErr()) return Err{std::string("GetAllActors is failed")};
	
	for (const auto Actor : Actors.Unwrap()) {
		if (const auto Index = Actor->FindComponentByClass<UActorIndexingComp>()) {
	
			auto NetId = (GetWorld(G) | GetNetPC | HasAuthority).Or(false) ? "Server" : "Client";
			Log::Push_S(std::format("{} - FindEtt : {} == {}", NetId, Index->OwnID, EttIdx));
			
			if (Index->OwnID == EttIdx) { return Actor; }
		}
	}

	return Err{std::string("FindNetEntity is failed")};
}
static FETT NewPlayerSetup(FPac_SpawnedChrInSrv Pac) {
	YC::Log::Push("NewPlayerSetup");
	return {
		.AChrPtr = { EttStateType::BeforeSpawn },
		.Chr = FOtChr{ {1,1}, (EttType::EType)(Pac.CharacterType + 1) },
		.EttIdx = Pac.EttIdx,
	};
}
static FETT GetMyChr(FPac_MyChrIdx Pac) {
	YC::Log::Push("GetMyChr");
	auto R = NewPlayerSetup({Pac.NetEntityIndexOfServer, Pac.CharacterType});
	R.Chr = FMyChr{ {1,1}, (EttType::EType)(Pac.CharacterType + 1)  };
	return R;
}
static ErrorOr<void> AddItem_(APlayerBase* Player, FLevelUp* LvUp, FInventory* Inventory, FPac_AddItem Pac) {
	if(LvUp == nullptr) return Err{std::string("Inventory is not valid")};
	if(Pac.bMyItem) {
		LvUp->ConfirmSelectedItem2(Pac.ItemId, Player);
	}
	else {
		for(const auto Id : Pac.ItemId) {
			if (!Inventory->AllItems.Contains(Id))
				Inventory->AddItemUI(Id);
			Inventory->AddItem(Id, Player);
		}
	}
	return {};
}
//static ErrorOr<void> AddItem_(FPac_AddItem Pac, FInventory* Inventory)
inline auto AddItem = Curry(AddItem_);

static ErrorOr<void> OnItemDrop(FInventory* Inv, FPac_ItemDrop Pac) {
	if(!Inv->ItemWidgets.Contains(Pac.ItemId)) return Err{std::format("ItemWidget has not {}", Pac.ItemId)};

	Log::Push("OnItemDragEnd : " + Pac.ItemId);
	
	const auto DragEndItem = Inv->ItemWidgets[Pac.ItemId]; 
	DragEndItem->bIsOtherDragging = false;
	
	return YC_Cast<UCanvasPanelSlot>(Inv->ItemWidgets[Pac.ItemId]->Slot) | [Pac, Inv](UCanvasPanelSlot* Slot) {
		Inv->ItemWidgets[Pac.ItemId]->ResetInventoryGrid();
		Slot->SetPosition(Pac.Pos);
		Inv->ItemWidgets[Pac.ItemId]->UpdateInventoryGrid(Pac.Grid);
	};
}

inline ErrorOr<void> RecvItemDragPos_(const FInventory* Inv, FPac_ItemDrag Pac) {
	if(Inv == nullptr) return Err{std::string("Inventory is not valid")};
	if(!Inv->ItemWidgets.Contains(Pac.ItemId)) return Err{std::format("ItemWidget has not {}", Pac.ItemId)};

	Log::Push("OnItemDragging : " + Pac.ItemId);
	
	const auto DraggingItem = Inv->ItemWidgets[Pac.ItemId]; 
	DraggingItem->bIsOtherDragging = true;
	
	return YC_Cast<UCanvasPanelSlot>(DraggingItem->Slot) | [Pac](UCanvasPanelSlot* Slot) {
		Slot->SetPosition(Pac.Pos);
	};
}
inline auto RecvItemDragPos = Curry(RecvItemDragPos_);

ErrorOr<void> SendItemDragData(const ANetPC* NetPC, const FInventory* Inv);
inline int64 CalcTimeOffset(int64 InSavedTime, FPac_TimeSync Pac) { return Pac.ServerTime - (InSavedTime + (Time::Epoch_Ms() - InSavedTime) / 2); };
inline FWorldStatus GetWorldStatus(FPac_UpdateWorldStatus Pac) { return { Pac.Exp, Pac.Lv }; }
std::tuple<FInGameUIManager*, FInventory*, FLevelUp*> CreateGUI(const UWorld* InUWorld, ANetPC* InPC);

//무조건 모나드로 사용해야함!
float Health(const UAttributeComponent* InComp);
float HealthMax(const UAttributeComponent* InComp);
bool IsDead(FChr Chr);

FWorld ClientLoad(const UObject* G, FPacBuf* PacBuf, const ANetPC* NetPC, clnt_packet_queue& Queue);
FWorld ClientTick(const UObject* G, const FPacBuf& PacBuf, FWorld World);
FWorld ClientRenderTick(const UObject* G, FWorld World);

void SoundPlay(const UObject* G, UFMODEvent* Event, const FVector& Target, const FString& Key);
void SoundStop(const UObject* G, const FString& Key, bool bRelease);

void SetVisible(float InGroundFriction, float InGravityScale, const bool bVisible, AEntity* Ett);

//do not curried!
void NewWidget(FPac_NewLocalWidget Pac, ANetPC* NetPC, FWorld& Ref_World);

//페마 위젯 생성 하기. ㅇㅋ
// void ShowWidget();


}// namespace YC::Client