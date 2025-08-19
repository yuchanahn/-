#pragma once

#include <ranges>
#include <YC/ErrorOr/ErrorOr.hpp>

#include "YCUtils.h"
#include "YC/Core/GameCharacter/CharacterSystem.hpp"

#include "../NetPC.h"
#include "../NetPacket/PacketAction.hpp"
#include "../NetPacket/PacketTypes.h"
#include "YC/Core/GameWorld/GWorld.hpp"


#define PACKET_BIND_SRV(PacType)\
PacType::ServerBind([](const PacType& Packet, const int ClientId)\
{ FServer::PlayerPacs.push_back({ ClientId, Packet }); }, Queue);

class AItemObject;

namespace YC::Server {
constexpr int __MaxClientCount = 100;

namespace PlayerUIState {
enum EType {
	Nothing,
	PickItem,
	CloseInvWhenLvUp
};
}

namespace GameUIState {
enum EType {
	Nothing,
	OpenInvWhenLvUp,
};
}

struct ClientData {
	ANetPC* Ptr;
	int ID;
	PlayerUIState::EType UIState = PlayerUIState::Nothing;
	// Test
	ErrorOr<int, EttStateType::EType> EttIdx = Err{ EttStateType::BeforeSpawn };
};

class FServer {
public:
	static bool IsServerInit;
	static std::vector<std::pair<int, Chanel_Player_Srv>> PlayerPacs;
	static std::unordered_map<ANetPC*, ClientData> PC2CD;
	static std::vector<ANetPC*> PCs;
	static std::vector<FETT> Etts;
	static ErrorOr<FLevelUp*> LevelUp;
	static ErrorOr<FInventory*> Inv;
	static FWorldStatus WorldStatus;
	static std::vector<int32> DisconnectTargets;
	static GameUIState::EType GameUIState;
	static TMap<int, ANetPC*> DragItems;
	static ErrorOr<AItemObject*> StageStart;
	static bool OpenItemObject;
	inline static bool IsChrPick = false; 
	inline static bool bLvLoaded = false;
	inline static std::vector<std::pair<int32, FPac_SpawnAndPossess>> PlayerSpawnPacket {};
	inline static int64_t WaitTimeOfSpawnDt = 0;
	
	static void ClearPacs() { PlayerPacs.clear(); }
	static int TargetCount;
	static void OnGameEnded();
};


template <std::ranges::range R>
auto to_vector(R&& r) {
	auto r_common = r | std::views::common;
	return std::vector(r_common.begin(), r_common.end());
}

inline void OnClientConnect(ANetPC* Client) {
	Log::Push("@@@@@@ Client Connected @@@@@@");
	
	FServer::PC2CD[Client] = {
		.Ptr = Client,
		.ID = static_cast<int>(FServer::PC2CD.size()),
		.UIState = PlayerUIState::Nothing,
		.EttIdx = Err{ EttStateType::BeforeSpawn }
	};
	FServer::PCs[FServer::PC2CD.size() - 1] = Client;

	Log::Push(YC_FMT("[YCDebug] New PC : {}", FServer::PC2CD.size() - 1));
}

void OnClientDisconnect(ANetPC* Client);

inline ErrorOr<int> GetClientID(const ANetPC* Client) {
	if (Client == nullptr) return Err{std::string("Client is not valid")};
	if (!FServer::PC2CD.contains(const_cast<ANetPC*>(Client))) return Err{std::string("!ClientList.contains(Client)")};
	return FServer::PC2CD[const_cast<ANetPC*>(Client)].ID;
}

void Server_SendAll(is_packet auto Packet) {
	for (auto& [Ptr, ID, _, _0] : FServer::PC2CD | std::views::values)
		Server_Send(Packet, Ptr) | WhenErr | Log::Push_Err_S;
}

template <is_packet T>
void Server_SendAllBut(T Packet, const ANetPC* Client) {
	for (auto& [Ptr, ID, _, _0] : FServer::PC2CD | std::views::values)
		if (Ptr != Client) Server_Send(Packet, Ptr) | WhenErr | Log::Push_Err_S;
}

inline int32 GenEttIdx(bool Reset = false) {
	static int32 ID = 0;
	if (Reset) ID = 0;
	return ID++;
}

inline ErrorOr<void> SpawnChr_(const int ClientId, const UObject* G, FPac_SpawnAndPossess Packet) {
	const Res::EPlayer TypeOfChr = static_cast<Res::EPlayer>(Packet.SelectedCharacterID);
	const auto Chr = Sys::Chr::SpawnPlayer(G, GenEttIdx(), TypeOfChr);

	if (Chr.IsErr()) return Err{Chr.GetError()};
	
	ACharacter* AChrPtr = Cast<ACharacter>(Chr.Unwrap().AChrPtr.Unwrap());

	AChrPtr->SetReplicates(true);
	AChrPtr->SetReplicateMovement(true);

	const auto& PC = FServer::PCs[ClientId];
	
	if(PC != nullptr) {
		PC->Possess(AChrPtr);
		Log::Push(YC_FMT("[YCDebug] Possess Success : {}", ClientId));
	}else {
		return Err {std::string("[YCDebug] PC is nullptr")};
	}
	FServer::Etts[Chr.Unwrap().EttIdx] = Chr.Unwrap();
	FServer::PC2CD[PC].EttIdx = Chr.Unwrap().EttIdx;
	
	Server_SendAllBut(FPac_SpawnedChrInSrv{Chr.Unwrap().EttIdx, Packet.SelectedCharacterID}, PC);
	Server_Send(FPac_MyChrIdx{Chr.Unwrap().EttIdx, Packet.SelectedCharacterID}, PC);

	Log::Push(YC_FMT("Spawn Success!"));
	
	return {};
}
//SpawnChr_(int, UObject*, FPac_SpawnAndPossess) -> ErrorOr<void>
inline auto SpawnChr = Curry(SpawnChr_);


void OnNewEttSpawned(AActor*);
void OnEttDead(AActor*);
bool AddItem(const int ItemId, FLevelUp* LvUp);
bool IsDead(FChr Chr);
void ServerLoad(UObject* G, srv_packet_queue& Queue);
void ServerTick(const UObject* G);
int32 GetPcNum();
void Test_AddItem(TArray<int> ItemID, ANetPC* Player);
void ServerRenderTick(const UObject* G);
void Move2NextLv(const UObject* G);
void SetVisible(AEntity* Ett, float InGroundFriction, float InGravityScale, const bool bVisible);

enum class EShowTarget {
	All,
	OnlyMe
};

void Send_ShowWBP(const APlayerBase* Player, UClass* TargetWbp, bool bActive, EShowTarget ShowTarget = EShowTarget::All);
ErrorOr<AEntity*> GetDeadPlayer();
}
