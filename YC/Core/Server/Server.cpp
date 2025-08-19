#include "YC/Core/Server/Server.hpp"

#include <YC/Time/Time.hpp>

#include "Dongyun/Faema/NirvanaFaemaCharacter.h"
#include "Dongyun/GAS/AttributeComponent.h"
#include "Taehun/NirvanaPadmaCharacter.h"
#include "YC/Core/Client/Client.hpp"

#include <ranges>

#include "../Static/ResourceMap/ResSound.h"
#include "Dongjun/Monster/Monster.h"
#include "../../Object/ItemObject.h"
#include "Components/CapsuleComponent.h"
#include "YC/Core/DT/ServerSetting.h"

using namespace YC;
using namespace Server;

bool FServer::IsServerInit = false;

std::vector<std::pair<int, Chanel_Player_Srv>> FServer::PlayerPacs {};
std::unordered_map<ANetPC*, ClientData> FServer::PC2CD {};
std::vector<ANetPC*> FServer::PCs(__MaxClientCount);
std::vector<FETT> FServer::Etts;
ErrorOr<AItemObject*> FServer::StageStart = Err { std::string("StageStart is not valid") };
bool FServer::OpenItemObject = false;
ErrorOr<FLevelUp*> FServer::LevelUp = Err { std::string("LevelUp is not valid") };
ErrorOr<FInventory*> FServer::Inv = Err { std::string("Inventory is not valid") };
std::vector<int32> FServer::DisconnectTargets = {};
GameUIState::EType FServer::GameUIState = GameUIState::Nothing;
TMap<int, ANetPC*> FServer::DragItems;
int FServer::TargetCount;

FWorldStatus FServer::WorldStatus {
	.Exp = 0,
	.Lv = 1,
};

bool Server::IsDead(FChr Chr) {
	const auto MyChrDead = Chr | [](FMyChr MyChr) { return MyChr.Stt.Hp <= 0; };
	const auto OtChrDead = Chr | [](FOtChr OtChr) { return OtChr.Stt.Hp <= 0; };
	const auto MobDead = Chr | [](FMob Mob) { return Mob.Stt.Hp <= 0; };
	return MyChrDead.Or(false) | OtChrDead.Or(false) | MobDead.Or(false);
}

bool Server::AddItem(const int ItemId, FLevelUp* LvUp) {
	if(ItemId == 0) return false;

	const auto Inv = LvUp->Inventory;
	Inv->AddItem(ItemId, nullptr);
	
	const auto Item = Inv->AllItems[ItemId];
	switch (Item->OwnerType)
	{
	case Shared:
		if (Item->bIsUpgraded)
			FLevelUp::RemoveItemFrom(&LvUp->ItemListShared, ItemId);
		break;
	case Padma:
		FLevelUp::RemoveItemFrom(&LvUp->ItemListPadma, ItemId);
	case Faema:
		FLevelUp::RemoveItemFrom(&LvUp->ItemListFaema, ItemId);
		break;
	}
	
	return true;
}

TArray<int> GetRandomItem(const int Lv, const FLevelUp* LvUp) {
	// TODO: 다시 돌리기 기능 만들기
	// TODO: 시트 파싱해서 전용/공용 구분하기
	TArray<int> Result;
	auto SharedItems = LvUp->PickRandomItemsFrom(LvUp->ItemListShared, 1); // TODO: 6으로 바꾸기
	auto PadmaItems = LvUp->PickRandomItemsFrom(LvUp->ItemListPadma, 3);
	auto FaemaItems = LvUp->PickRandomItemsFrom(LvUp->ItemListFaema, 3);
	
	Result.Append(PadmaItems);
	Result.Append(FaemaItems);
	return Result;
}

void RemoveItem(const int ItemId, FLevelUp* LvUp) {
	if (ItemId == 0 || LvUp == nullptr)
		return;
	
	const auto Item = LvUp->Inventory->AllItems[ItemId];
	
	LvUp->Inventory->RemoveItem(ItemId);
	
	if (Item->IsUpgradable())
	{
		switch (Item->OwnerType)
		{
		case Shared:
			LvUp->ItemListShared.Add(Item->Id);
			break;
		case Padma: 
		case Faema:
			break;
		}
	}
}
void PreRemoveItem(const int ItemId, APlayerBase* Player, FInventory* Inv) {
	Inv->ItemList.OnRemoveItem(ItemId, Player);
}

ErrorOr<FETT> GetEtt(const int EttIdx) {
	if(FServer::Etts.size() <= EttIdx) return Err { std::string("EttIdx is out of range") };
	return FServer::Etts[EttIdx];
}
ErrorOr<FETT> GetEttFormPC(ANetPC* PC) {
	if(!FServer::PC2CD.contains(PC)) return Err { std::string("PC is not valid") };
	const auto& [Ptr, ID, UIState, EttIdx] = FServer::PC2CD[PC];
	if(EttIdx.IsErr()) {
		return Err { std::format("EttIdx is Error Err ID : {}", static_cast<int>(EttIdx.GetError())) };
	}
	return GetEtt(EttIdx.Unwrap());
}

void SendItemsWhenLvUp(const ErrorOr<TArray<int>>& ItemList, const ANetPC* TargetClient, FETT Ett) {
	Ett.Chr | [&](FOtChr Chr) {
		switch (Chr.EttType) {
		case EttType::Padma:
			Server_Send(FPac_LvUp { (ItemList | Curry(TakeRng<int>)(0, 2)).Or({}) }, TargetClient);
		break;
		case EttType::Faema:
			Server_Send(FPac_LvUp { (ItemList | Curry(TakeRng<int>)(3, 5)).Or({}) }, TargetClient);
		break;
		default: ;
		}
	};
}

ErrorOr<bool> EttAlive(FETT Ett) {
	return Ett.Chr | [&](FOtChr C) {
		if(C.Stt.Hp > 0) return true;
		return false;
	};
}

bool IsPcEttAlive(ANetPC* PC) { return (GetEttFormPC(PC) | EttAlive).Or(false); };
bool IsPcValid(const ANetPC* PC) { return IsValid(PC); };

void FServer::OnGameEnded() {
	IsServerInit = false;
	PC2CD.clear();
	PCs.clear();
	Etts.clear();
	
	if(LevelUp.IsOk()) {
		delete LevelUp.Unwrap();
		LevelUp = Err { std::string("LevelUp is not valid") };
	}
	if(Inv.IsOk()) {
		delete Inv.Unwrap();
		Inv = Err { std::string("Inv is not valid") };
	}
	WorldStatus = FWorldStatus {
		.Exp = 0,
		.Lv = 1,
	};
	DisconnectTargets.clear();
}

void Server::OnClientDisconnect(ANetPC* Client) {
	const auto& [Ptr, ID, bCloseInvLvUp, EttIdx] = FServer::PC2CD[Client];
	FServer::PCs[ID] = nullptr;

	if(EttIdx.IsOk())
		FServer::DisconnectTargets.push_back(EttIdx.Unwrap());
	
	FServer::PC2CD.erase(Client);
}

FChr GetEttObject(AActor* Actor) {
	if(Cast<AMonster>(Actor)) return FMob {};
	return FWall { {1,1} , EttType::Wall };
}

void Server::OnNewEttSpawned(AActor* Ett) {
	const auto IdxComp = Comp<UActorIndexingComp>(Ett);
	if(IdxComp->OwnID == -1) {
		IdxComp->OwnID = GenEttIdx();
		
		while(IdxComp->OwnID >= FServer::Etts.size()) FServer::Etts.emplace_back();
		
		FServer::Etts[IdxComp->OwnID] = FETT {
			.AChrPtr = Cast<AEntity>(Ett),
			.Chr = GetEttObject(Ett),
			.EttIdx = IdxComp->OwnID,
		};
	}
}

void Server::OnEttDead(AActor* Ett) {
	const auto IdxComp = Comp<UActorIndexingComp>(Ett);
	
	if(IdxComp->OwnID == -1) return;
	if(IdxComp->OwnID >= FServer::Etts.size()) return;
	FServer::Etts[IdxComp->OwnID].Chr | [&](FWall) {
		FServer::Etts[IdxComp->OwnID] = FETT {
			.AChrPtr = EttStateType::Dead,
			.Chr = GetEttObject(Ett),
			.EttIdx = IdxComp->OwnID,
		};
	};

}


void Server::ServerLoad(UObject* G, srv_packet_queue& Queue) {
	GenEttIdx(true);

	FServer::Etts.clear();
	FServer::PlayerPacs.clear();
	
	PACKET_BIND_SRV(FPac_SpawnAndPossess);
	PACKET_BIND_SRV(FPac_TimeSync);
	PACKET_BIND_SRV(FPac_ItemDrag);
	PACKET_BIND_SRV(FPac_PickItem);
	PACKET_BIND_SRV(FPac_LocalInvLvUpClose);
	PACKET_BIND_SRV(FPac_LvUp);
	PACKET_BIND_SRV(FPac_ItemDrop);
	PACKET_BIND_SRV(FPac_RemoveItem);
	PACKET_BIND_SRV(FPac_FModPlay);
	PACKET_BIND_SRV(FPac_Input2Server);

	FServer::Etts.resize(__MaxClientCount * 100);
	FServer::PCs.resize(__MaxClientCount);
	
	for(int i = 0; i < __MaxClientCount; ++i)
		FServer::PCs[i] = nullptr;
	
	FServer::WorldStatus = { 0, 1 };
	FServer::Inv = Err {std::string("Inv is not valid")};
	FServer::LevelUp = Err {std::string("LevelUp is not valid")};
	FServer::GameUIState = GameUIState::Nothing;
	FServer::DragItems.Reset();
	FServer::StageStart = Err { std::string("StageStart is not valid") };
	FServer::TargetCount = 0;

	FServer::IsServerInit = true;
	FServer::bLvLoaded = false;

	FServer::PlayerSpawnPacket.clear();
	FServer::WaitTimeOfSpawnDt = Time::Epoch_Ms();
	Cast<UNetGameInstance>(G->GetWorld()->GetGameInstance())->FnOnInGameLevelLoaded = [] {
		FServer::bLvLoaded = true;
		Server_SendAll(FPac_GameMapLoaded{});
	};
	FServer::bLvLoaded = true;
}

void Server::Test_AddItem(TArray<int> ItemID, ANetPC* Player) {
	FServer::GameUIState = GameUIState::OpenInvWhenLvUp;

	for (const auto& Id : ItemID)
		FServer::LevelUp | Curry(AddItem)(Id);

	for (const auto P : FServer::PCs) {
		Server_Send(FPac_AddItem{ItemID, P == Player}, P);
	}
}

int32 Server::GetPcNum() {
	int32 Count = 0;
	for(const auto Pc : FServer::PCs) {
		Count = Pc != nullptr ? Count + 1 : Count;
	}
	return Count;
}

void Server::ServerTick(const UObject* G) {
	if(FServer::LevelUp.IsErr()) {
		FServer::Inv = new FInventory {};
		FServer::Inv.Unwrap()->Setup(nullptr, nullptr, nullptr);
		FServer::LevelUp = new FLevelUp {};
		FServer::LevelUp.Unwrap()->Setup(nullptr, nullptr, FServer::Inv.Unwrap());
	}
	
	for (const auto& [ID, Pac] : FServer::PlayerPacs) {
		auto CP = FServer::PCs[ID];
		auto SendTo = [CP]<is_packet T>(T Pac){ return Server_Send<T>(Pac, CP); };
		auto SendBut = [CP]<is_packet T>(T Pac){ return Server_SendAllBut<T>(Pac, CP); };
		auto SendAll = []<is_packet T>(T Pac){ return Server_SendAll<T>(Pac); };
		
		Pac | Unpack<FPac_SpawnAndPossess> | [ID](FPac_SpawnAndPossess Pac) {
			Log::Push(YC_FMT("[YCDebug] Packet Recv! FPac_SpawnAndPossess : ID: {}, CID : {}", std::to_string(ID), Pac.SelectedCharacterID));
			FServer::WaitTimeOfSpawnDt = Time::Epoch_Ms();
			FServer::PlayerSpawnPacket.push_back(std::make_pair(ID, Pac));
		};
		
		Pac | [SendTo](FPac_TimeSync) { return SendTo(FPac_TimeSync{Time::Epoch_Ms()}); };
		Pac | Unpack<FPac_FModPlay> | SendAll;
		Pac | Unpack<FPac_ItemDrag> | [&](FPac_ItemDrag Pac) {
			if( FServer::DragItems.Contains(Pac.ItemId)
			 && FServer::DragItems[Pac.ItemId] != CP) {
				return;
			}
			if(!FServer::DragItems.Contains(Pac.ItemId))
				FServer::DragItems.Add(Pac.ItemId, CP);
			SendBut(Pac);
		};
		
		Pac | [&](FPac_PickItem Data) {
			FServer::LevelUp | Curry(AddItem)(Data.ItemId) | [&](bool bSuccess) {
				if (!bSuccess) return;
				SendBut(FPac_AddItem{ { Data.ItemId }, false});
				SendTo(FPac_AddItem{ { Data.ItemId }, true});
			};
		};
		Pac | [](FPac_RemoveItem Data) {
			for (auto I : Data.Items) {
				for(auto& [AChrPtr, Chr, EttIdx] : FServer::Etts | std::views::filter(HasPtr)) {
					FServer::Inv | Curry(PreRemoveItem)(I)(Cast<APlayerBase>(AChrPtr | Unwrap));
				}
				FServer::LevelUp | Curry(RemoveItem)(I);
			}
		};

		Pac | Unpack<FPac_ItemDrop> | [&](FPac_ItemDrop Pac) {
			if(!FServer::DragItems.Contains(Pac.ItemId)) return;
			FServer::DragItems.Remove(Pac.ItemId);
			SendBut(Pac);
		};

		Pac | Unpack<FPac_Input2Server> | [](FPac_Input2Server Pac) {
			const auto Ett = GetEtt(Pac.EttIdx);
			Ett | WhenErr | YC::Log::Push_Err_S;
			if(Ett.IsErr()) return;
			(Ett.Unwrap().AChrPtr | Unwrap)->ExecuteInput(Pac.Input, Pac.Type);
		};
		
		if (FServer::GameUIState == GameUIState::OpenInvWhenLvUp) {
			Pac | [CP](FPac_LocalInvLvUpClose) {
				FServer::PC2CD[CP].UIState = PlayerUIState::CloseInvWhenLvUp;

				//Check All Player CloseInvWhenLvUp
				{
					using std::views::values;
					using std::views::filter;
	
					const bool bCheckAllPlayerCloseInvWhenLvUp =
						to_vector(FServer::PC2CD | values | filter([](auto Cd) {
						return Cd.UIState == PlayerUIState::CloseInvWhenLvUp;
					})).size()
					==
					to_vector(FServer::PC2CD | values | filter([](auto Cd) {
						return Cd.EttIdx.IsOk();
					})).size();

					if(bCheckAllPlayerCloseInvWhenLvUp) {
						Server_SendAll(FPac_GamePause { false });
						Server_SendAll(FPac_InvLvUpClose{});
						FServer::GameUIState = GameUIState::Nothing;
						for(auto& Cd: FServer::PC2CD | std::views::values) {
							Cd.UIState = PlayerUIState::Nothing;
						}
					}
				}
				
			};
		}
	}

	
	bool IsSpawnable = Time::Epoch_Ms() - FServer::WaitTimeOfSpawnDt > 2000;
		
#ifdef WITH_EDITOR
	if(FServer::PlayerSpawnPacket.size() >= GetPcNum() && IsSpawnable) {
#else
	if(FServer::PlayerSpawnPacket.size() >= GetPcNum() && IsSpawnable) {
#endif
		Log::Push(YC_FMT("[YCDebug] GetPcNum : {}", GetPcNum()));
		for(auto Pac_ : FServer::PlayerSpawnPacket) {
			Log::Push(YC_FMT("[YCDebug] Pac_.first : {}", std::to_string(Pac_.first)));
#ifdef WITH_EDITOR
			UServerSetting* Data = Cast<UServerSetting>(Res::DA_ServerSetting.Get());
			Pac_.second.SelectedCharacterID = Pac_.first == 0 ? (int8)Data->ServerCharacter : (int8)Data->ClientCharacter;
#endif
			auto NewSpawned = SpawnChr_(Pac_.first, G, Pac_.second);
			if(NewSpawned.IsOk()) {
				Server_SendAll(FPac_UpdateWorldStatus{ 
					.Exp = FServer::WorldStatus.Exp,
					.Lv = FServer::WorldStatus.Lv,
				});	
			}	
			NewSpawned | WhenErr | Log::Push_Err_S; 
		}
		FServer::PlayerSpawnPacket.clear();
	}
	
	// - # checking client disconnect.
	for(auto Target : FServer::DisconnectTargets) {
		auto Ett = GetEtt(Target);
		if(Ett.IsErr()) continue;
		if(Target <= FServer::Etts.size()) {
			FServer::Etts[Target].AChrPtr = EttStateType::Disconnect;
		} 
	}
	FServer::DisconnectTargets.clear();
	
	for(auto& [AChrPtr, Chr, EttIdx] : FServer::Etts | std::views::filter(HasPtr)) {
		auto Att = AChrPtr | Comp<UAttributeComponent>;
		
		auto HP = (Att | Client::Health).Or(0);
		auto HP_Max = (Att | Client::HealthMax).Or(0);
		const auto HpUpdateAndSend = [&](auto C) {
			if(HP != C.Stt.Hp || HP_Max != C.Stt.HpMax) {
				C.Stt.Hp = HP;
				C.Stt.HpMax = HP_Max;
				Server_SendAll(FPac_HealthChanged { EttIdx, C.Stt.Hp, C.Stt.HpMax });
			}
			return C;
		};
		Chr | [&](FOtChr C) { Chr = HpUpdateAndSend(C); };
		Chr | [&](FWall C) {
			if(AChrPtr.IsOk()) {
				//Chr = HpUpdateAndSend(C);
				Att | UAttributeComponent::ServerCall_HealthUpdate;
			}
		};
		Chr | [&](FMob C) {
			if(HP != C.Stt.Hp || HP_Max != C.Stt.HpMax) {
				C.Stt.Hp = HP;
				C.Stt.HpMax = HP_Max;
				Att | UAttributeComponent::ServerCall_HealthUpdate;
			}
			Chr = HpUpdateAndSend(C);
		};
	}
	
	// 경험치 얻는 로직
	for(auto& Ett : FServer::Etts | std::views::filter(HasPtr)) {
		Ett.AChrPtr | APlayerBase::PopExpList | [&](TArray<int32> ExpList) {
			int ExpSum = 0;
			for(const auto& Exp : ExpList) ExpSum += Exp;
			
			FServer::WorldStatus.Exp += ExpSum;
			if(FServer::WorldStatus.Exp > 100) {
				auto Target = GetDeadPlayer();
				if(Target.IsOk()) {
					Target | Comp<UAttributeComponent>
						   | [](UAttributeComponent* Att) {
						   Att->Health = Att->HealthMax;
						   return Att; }
						   | UAttributeComponent::ServerCall_HealthUpdate;
					
				}
			}
			if(ExpSum > 0){
				auto Pos = (Ett.AChrPtr | Unwrap)->GetActorLocation();
				Server_SendAll(FPac_FModPlay{
					.EventKey = "Exp",
					.EventID = Res::FMod::GetIdxOf(Res::SFX_PL_Exp),
					.Pos = Pos,
				});
				Server_SendAll(FPac_UpdateWorldStatus{
					.Exp = FServer::WorldStatus.Exp,
					.Lv = FServer::WorldStatus.Lv,
				});
			}
		};
	}

	int mobCount = 0;
	if(FServer::StageStart.IsOk() && (FServer::TargetCount == 0)) {
		if(!IsValid(FServer::StageStart.Unwrap())) FServer::StageStart = Err { std::string("Unwrap but Nullptr") }; 
		FServer::StageStart.Unwrap()->Start();
		Log::Push_S("Start Stage!!");
		FServer::StageStart = Err { std::string("StageStart is not valid") };
	}

	if(FServer::OpenItemObject) {
		FServer::OpenItemObject = false;
		
		for(const auto Pc : FServer::PCs | std::views::filter(IsPcEttAlive)) {
			GetEttFormPC(Pc) | Curry(SendItemsWhenLvUp)(FServer::LevelUp | Curry(GetRandomItem)(1), Pc);
		}
		Server_SendAll(FPac_GamePause { true });
		FServer::GameUIState = GameUIState::OpenInvWhenLvUp;
	}
	
	for(auto& Ett : FServer::Etts | std::views::filter(HasPtr)) {
		if(IsDead(Ett.Chr)) {
			//todo: 근본적인 해결 법은 아님
			//Ett.AChrPtr = EttStateType::Dead;
			Ett.Chr | [&](FMob) {
				FServer::TargetCount -= 1;
				Ett.AChrPtr = EttStateType::Dead;
			};
		}
	}

	int DeadCount = 0;
	for(const auto Pc : FServer::PCs) {
		auto Ett = GetEttFormPC(Pc);
		
		if(FServer::PC2CD.contains(Pc) && FServer::PC2CD[Pc].EttIdx.IsErr()) {
			if(FServer::PC2CD[Pc].EttIdx.GetError() == EttStateType::Dead) {
				DeadCount++;
				continue;
			}
		}
		if(Ett.IsErr()) {
			continue;
		}
		auto AChr = Ett.Unwrap().AChrPtr;
		if(AChr.IsErr() && (	AChr.GetError() == EttStateType::Dead
							 || AChr.GetError() == EttStateType::Disconnect)) {
			FServer::PC2CD[Pc].EttIdx = Err { AChr.GetError() };
			DeadCount++;
		}
	}
	
	if(DeadCount == 2) (GetWorld(G) | GetNetPC | Unwrap)->DestroySession();
	
	FServer::ClearPacs();
}

void Server::ServerRenderTick(const UObject* G) {
}

void Server::Move2NextLv(const UObject* G) {
	Server_SendAll(FPac_Move2NextLv{});
}

void Server::SetVisible(AEntity* Ett, float InGroundFriction, float InGravityScale, const bool bVisible) {
	Server_SendAll(FPac_SetVisibility{Comp<UActorIndexingComp>(Ett)->OwnID, InGroundFriction, InGravityScale, bVisible});
	YC::Log::Push("Send Packet : SetVisible");
}

void Server::Send_ShowWBP(const APlayerBase* Player, UClass* TargetWbp, const bool bActive, const EShowTarget ShowTarget) {
	const auto Pac = FPac_NewLocalWidget{bActive, Res::Widget::GetIdxOf(Cast<UUserWidget>(TargetWbp))};
	if(ShowTarget == EShowTarget::OnlyMe) {
		const auto TargetPC = Cast<ANetPC>(Player->GetController());
		if(Player->GetController() == nullptr) {
			Log::Push_Err("Player->GetController() == nullptr");
			return;
		}
		Server_Send(Pac, TargetPC);
	} else if(ShowTarget == EShowTarget::All) {
		Server_SendAll(Pac);
	}
}

ErrorOr<AEntity*> Server::GetDeadPlayer() {
	for(auto& [AChrPtr, Chr, EttIdx] : FServer::Etts | std::views::filter(HasPtr)) {
		auto R = Chr | [&](FOtChr C) -> ErrorOr<void> {
			if(C.Stt.Hp <= 0) return {};
			return Err { std::string("Dead Player is not found") };
		};
		if(R.IsOk()) return AChrPtr.Unwrap();
	}
	return Err { std::string("Dead Player is not found") };
}
