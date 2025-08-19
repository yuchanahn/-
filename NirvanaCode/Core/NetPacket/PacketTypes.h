#pragma once
#include "CoreMinimal.h"
#include "Packets.hpp"
#include "PRW.hpp"
#include "UObject/Object.h"
#include "EnhancedInputComponent.h"

#include "PacketTypes.generated.h"

#define YC_PACKET(Type) inline static uint8 __packet_id = YC_Internal::GetPacketID();			        \
	static void Bind(std::function<void(Type)> Fn, clnt_packet_queue& Queue) {							\
            UE_LOG(LogTemp, Warning, TEXT("Client : %hs, %d"), #Type, __packet_id);                     \
			Queue[__packet_id] = [Fn](const TArray<uint8>& Data) {										\
			Fn(YC::PRW::Deserialize<Type>(Data));														\
		};																								\
	}																									\
	static void ServerBind(std::function<void(Type, int)> Fn, srv_packet_queue& SrvQueue) {				\
			UE_LOG(LogTemp, Warning, TEXT("Server : %hs, %d"), #Type, __packet_id); 						\
			SrvQueue[__packet_id] = [Fn](const TArray<uint8>& Data, const int Client_ID) {					\
			Fn(YC::PRW::Deserialize<Type>(Data), Client_ID);												\
		};                                                                                              \
	}           																						\


namespace YC_Internal{
static uint8 __packet_id_counter = 0;
static uint8 GetPacketID() {
	return __packet_id_counter++;
}
}

/* =============== Packet =============== */
// 패킷 정의.
// 패킷은 반드시 YC_PACKET 매크로를 사용하여 정의해야 합니다.
// =========================================
struct FPac_MyChrIdx;
struct FPac_SpawnedChrInSrv;
struct FPac_SpawnAndPossess;
struct FPac_TimeSync;
struct FPac_ItemDrag;
struct FPac_AddItem;
struct FPac_PickItem;
struct FPac_LocalInvLvUpClose;
struct FPac_InvLvUpClose;
struct FPac_LvUp;
struct FPac_ItemDrop;
struct FPac_UpdateWorldStatus;
struct FPac_RemoveItem;
struct FPac_GamePause;
struct FPac_HealthChanged;
struct FPac_FModPlay;
struct FPac_Input2Server;
struct FPac_Move2NextLv;
struct FPac_SetVisibility;
struct FPac_NewLocalWidget;
struct FPac_GameMapLoaded;

/* =============== Chanel =============== */
// 패킷 채널링.
// 각 패킷의 채널을 정의 합니다.
// 같은 채널의 패킷들은 순서가 보장됩니다.
// =========================================
using Chanel_Player = std::variant<
	FPac_SpawnedChrInSrv,
	FPac_MyChrIdx,
	FPac_TimeSync,
	FPac_ItemDrag,
	FPac_PickItem,
	FPac_LocalInvLvUpClose,
	FPac_ItemDrop,
	FPac_UpdateWorldStatus,
	FPac_HealthChanged,
	FPac_GamePause,
	FPac_FModPlay,
	FPac_SetVisibility,
	FPac_GameMapLoaded,
	FPac_LvUp
>;

using Chanel_GUI = std::variant<
	FPac_ItemDrop,
	FPac_ItemDrag,
	FPac_AddItem,
	FPac_InvLvUpClose,
	FPac_Move2NextLv,
	FPac_NewLocalWidget,
	FPac_LvUp
>;

using Chanel_Player_Srv = std::variant<
	FPac_SpawnAndPossess,
	FPac_TimeSync,
	FPac_ItemDrag,
	FPac_PickItem,
	FPac_LocalInvLvUpClose,
	FPac_ItemDrop,
	FPac_RemoveItem,
	FPac_LvUp,
	FPac_Input2Server,
	FPac_FModPlay
>;

USTRUCT()
struct FPac_SpawnAndPossess {
	GENERATED_BODY()
	
	UPROPERTY()
	int8 SelectedCharacterID;
	
	YC_PACKET(FPac_SpawnAndPossess);
};

USTRUCT()
struct FPac_SpawnedChrInSrv {
	GENERATED_BODY()
	
	UPROPERTY()
	int32 EttIdx;

	UPROPERTY()
	int8 CharacterType;
	
	YC_PACKET(FPac_SpawnedChrInSrv);
};

USTRUCT()
struct FPac_MyChrIdx {
	GENERATED_BODY()
	
	UPROPERTY()
	int32 NetEntityIndexOfServer;

	UPROPERTY()
	int8 CharacterType;

	YC_PACKET(FPac_MyChrIdx);
};

USTRUCT()
struct FPac_TimeSync {
	GENERATED_BODY()
	
	UPROPERTY()
	int64 ServerTime;
	
	YC_PACKET(FPac_TimeSync);
};

// ---------------------------------------------------------
// Inventory

USTRUCT()
struct FPac_AddItem {
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<int> ItemId;

	UPROPERTY()
	bool bMyItem;
	
	YC_PACKET(FPac_AddItem);
};

USTRUCT()
struct FPac_ItemDrag {
	GENERATED_BODY()
	
	UPROPERTY()
	int ItemId;
	
	UPROPERTY()
	FVector2D Pos;
	
	YC_PACKET(FPac_ItemDrag);
};

USTRUCT()
struct FPac_PickItem {
	GENERATED_BODY()
	
	UPROPERTY()
	int ItemId;
	
	YC_PACKET(FPac_PickItem);
};

USTRUCT()
struct FPac_ItemDrop {
	GENERATED_BODY()
	
	UPROPERTY()
	int ItemId;

	UPROPERTY()
	FVector2D Pos;

	UPROPERTY()
	FIntVector2 Grid;
	
	YC_PACKET(FPac_ItemDrop);
};

USTRUCT()
struct FPac_LocalInvLvUpClose {
	GENERATED_BODY()
	
	YC_PACKET(FPac_LocalInvLvUpClose);
};

USTRUCT()
struct FPac_InvLvUpClose {
	GENERATED_BODY()
	
	YC_PACKET(FPac_InvLvUpClose);
};

USTRUCT()
struct FPac_LvUp {
	GENERATED_BODY()

	UPROPERTY()
	TArray<int> Items;
	
	YC_PACKET(FPac_LvUp);
};

USTRUCT()
struct FPac_UpdateWorldStatus {
	GENERATED_BODY()

	UPROPERTY()
	float Exp;

	UPROPERTY()
	int32 Lv;
	
	YC_PACKET(FPac_UpdateWorldStatus);
};

USTRUCT()
struct FPac_RemoveItem {
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<int> Items;
	
	YC_PACKET(FPac_RemoveItem);
};

USTRUCT()
struct FPac_HealthChanged {
	GENERATED_BODY()
	
	UPROPERTY()
	int32 EttIdx;

	UPROPERTY()
	float Health;

	UPROPERTY()
	float MaxHealth;
	
	YC_PACKET(FPac_HealthChanged);
};

USTRUCT()
struct FPac_GamePause {
	GENERATED_BODY()

	UPROPERTY()
	int32 Pause;
	
	YC_PACKET(FPac_GamePause);
};

USTRUCT()
struct FPac_FModPlay {
	GENERATED_BODY()

	UPROPERTY()
	FString EventKey;

	UPROPERTY()
	int32 EventID;
	
	UPROPERTY()
	int32 EttIdx;
	
	UPROPERTY()
	bool bAttach;

	UPROPERTY()
	FVector Pos;
	
	UPROPERTY()
	bool bStop;

	UPROPERTY()
	bool bRelese;

	
	YC_PACKET(FPac_FModPlay);
};

USTRUCT()
struct FPac_MobHit {
	GENERATED_BODY()

	UPROPERTY()
	int32 EttIdx;
	
	YC_PACKET(FPac_MobHit);
};

USTRUCT()
struct FPac_Input2Server {
	GENERATED_BODY()

	UPROPERTY()
	int32 EttIdx;
	
	UPROPERTY()
	FInputActionValue Input;
	
	UPROPERTY()
	int32 Type;
	
	YC_PACKET(FPac_Input2Server);
};


USTRUCT()
struct FPac_Move2NextLv {
	GENERATED_BODY()

	YC_PACKET(FPac_Move2NextLv);
};

USTRUCT()
struct FPac_SetVisibility {
	GENERATED_BODY()

	UPROPERTY()
	int32 Ett;

	UPROPERTY()
	float InGroundFriction;
	
	UPROPERTY()
	float InGravityScale;
	
	UPROPERTY()
	bool bVisible;
	
	YC_PACKET(FPac_SetVisibility);
};

USTRUCT()
struct FPac_NewLocalWidget {
	GENERATED_BODY()

	UPROPERTY()
	bool bShow;
	
	UPROPERTY()
	int32 WBPIdx;
	
	YC_PACKET(FPac_NewLocalWidget);
};

USTRUCT()
struct FPac_GameMapLoaded {
	GENERATED_BODY()

	YC_PACKET(FPac_GameMapLoaded);
};