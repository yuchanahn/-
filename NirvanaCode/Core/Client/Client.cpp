#include "YC/Core/Client/Client.hpp"

#include "Dongjun/UI/Inventory/InventoryWidget.h"
#include "Dongyun/GAS/AttributeComponent.h"
#include "Dongyun/UI/InGameWidget.h"

#include <ranges>

#include "FMODBlueprintStatics.h"
#include "FMODStudioModule.h"
#include "YC/Core/GameLoop.h"
#include "YC/Core/FMod/FModPlayer.hpp"
#include "YC/Core/Server/Server.hpp"
#include "..\Static\ResourceMap\ResSound.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "YC/Core/UI/DisplayEffects.h"

//===================================================================================
// Curry 함수 쓸 때 커링 되는 함수에 & 래퍼런스 정보 사라짐. 넣으면 안됨,
// 굳이 하고 싶으면, 포인터 넘겨 주던가. 리턴 값으로 받아서 업데이트 해야함. 오류는 안남;
//===================================================================================

using namespace YC::Client;
using namespace YC;

float Client::Health(const UAttributeComponent* InComp) { return InComp->GetHealth(); }
float Client::HealthMax(const UAttributeComponent* InComp) { return InComp->GetHealthMax(); }
bool Client::IsDead(FChr Chr) {
	const auto MyChrDead = Chr | [](FMyChr MyChr) { return MyChr.Stt.Hp <= 0; };
	const auto OtChrDead = Chr | [](FOtChr OtChr) { return OtChr.Stt.Hp <= 0; };
	const auto MobDead = Chr | [](FMob Mob) { return Mob.Stt.Hp <= 0; };
	return MyChrDead.Or(false) | OtChrDead.Or(false) | MobDead.Or(false);
}
void UpdateHealth(float Hp, float MaxHp, UAttributeComponent* Comp) {
	if( Comp->GetOwner()->HasAuthority() ) return;
	Comp->Health = Hp;
	Comp->HealthMax = MaxHp;
}

void AddHealth(float Hp, UAttributeComponent* Comp) {
	if( Comp->GetOwner()->HasAuthority() ) return;
	Comp->Health += Hp;
}

FWorld HealthChanged(FWorld InWorld, FPac_HealthChanged Pac) {

	YC::Log::Push("HealthChanged");
	if(InWorld.Etts.size() <= Pac.EttIdx) {
		YC::Log::Push("HealthChanged : Etts size is less than Pac.EttIdx");
		return InWorld;
	}
	auto& [Ptr, Chr, Idx] = InWorld.Etts[Pac.EttIdx];
	if(Ptr.IsErr() && Ptr.GetError() != EttStateType::BeforeSpawn){
		return InWorld;
	}
	
	auto Apply = [&](FChr C){ Chr = C; }; 
	#define THIS_Apply \
	C.Stt.Hp = Pac.Health; C.Stt.HpMax = Pac.MaxHealth; return C;
	
	Chr | [&](FMyChr C) {
		InWorld.HUD | YC_Cast<UInGameWidget> | Curry(UInGameWidget::SetHealth)(Pac.Health, Pac.MaxHealth);
		THIS_Apply
	} | Apply;
	Chr | [&](FOtChr C) { THIS_Apply } | Apply;
	Ptr | Comp<UAttributeComponent> | Curry(UpdateHealth)(Pac.Health, Pac.MaxHealth);
	
	return InWorld;
}
int32 GetEttIdxOfHealthChangedTarget(const FPac_HealthChanged& Pac) { return Pac.EttIdx; }
void GamePause(UObject* G, ANetPC* InPC, FPac_GamePause Pac) {
	InPC->SetPause(static_cast<bool>(Pac.Pause));
	if(Pac.Pause) {
		InPC->Pause();
		InPC->ServerPause();
		Cast<AActor>(G)->SetActorTickEnabled(true);
	}
}


ErrorOr<void> Client::SendItemDragData(const ANetPC* NetPC, const FInventory* Inv) {
	if(const auto ItemW = Inv->DraggingItemWidget) {
		const auto Pos = YC_Cast<UCanvasPanelSlot>(ItemW->Slot) | GetSlotPos;
		if(Pos.IsErr()) return Err{ Pos.GetError() };
		return Send(FPac_ItemDrag { ItemW->Item->Id, Pos.Unwrap() }, NetPC);
	}
	return Err { std::string("ItemW is nullptr") };
}
std::tuple<FInGameUIManager*, FInventory*, FLevelUp*> Client::CreateGUI(const UWorld* InUWorld, ANetPC* InPC) {
	const auto UIMgr = new FInGameUIManager { };
	const auto Inv = new FInventory { };
	const auto LvUp = new FLevelUp { };
	
	UIMgr->Setup(InPC);
	Inv->Setup(InUWorld, InPC, UIMgr);
	LvUp->Setup(InUWorld, UIMgr, Inv);

	return { UIMgr, Inv, LvUp };
}
 
void Client::SoundPlay(const UObject* G, UFMODEvent* Event, const FVector& Target, const FString& Key) {
	if (IFMODStudioModule::IsAvailable())
	{
		if (FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime))
		{
			if(EventInstMap.Contains(Key)) {
				EventInstMap[Key].Instance->release();
				EventInstMap.Add(Key, UFMODBlueprintStatics::PlayEventAtLocation(
				const_cast<UObject*>(G),
				Event,
				FTransform(Target),
				true));
			} else {
				EventInstMap.Add(Key, UFMODBlueprintStatics::PlayEventAtLocation(
				const_cast<UObject*>(G),
				Event,
				FTransform(Target),
				true));
			}
		}
	}
}
void Client::SoundStop(const UObject* G, const FString& Key, bool bRelease) {
	if (IFMODStudioModule::IsAvailable())
	{
		if (IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime))
		{
			if(EventInstMap.Contains(Key)) {
				if(EventInstMap[Key].Instance->isValid()) {
					UFMODBlueprintStatics::EventInstanceStop(EventInstMap[Key], bRelease);
				} else {
					EventInstMap.Remove(Key);
					return;
				}
				if(bRelease) {
					EventInstMap.Remove(Key);
				}
			}
		}
	}
}

void Client::SetVisible(float InGroundFriction, float InGravityScale, const bool bVisible, AEntity* Ett) {
	Ett->GetMesh()->SetVisibility(bVisible);
	Ett->GetCharacterMovement()->Velocity = FVector(0.0f,0.0f,0.0f);
	
	if (bVisible) {
		Ett->GetCharacterMovement()->GroundFriction = InGroundFriction;
		Ett->GetCharacterMovement()->GravityScale = InGravityScale;

		Ett->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		Ett->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	} else {
		Ett->StopJumping();
		Ett->GetCharacterMovement()->GroundFriction = 0.0f;
		Ett->GetCharacterMovement()->GravityScale = 0.0f;
		
		Ett->SetActorRotation(Ett->GetActorForwardVector().Rotation());

		Ett->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		Ett->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	}
}

void Client::NewWidget(FPac_NewLocalWidget Pac, ANetPC* NetPC, FWorld& Ref_World) {
	const TSubclassOf<UUserWidget> Wbp = Res::Widget::Load(Pac.WBPIdx)->StaticClass();
	const auto R = CreateWidget_(Wbp, NetPC);
	R | AddToViewport;
	
	if(R.IsErr()) {
		R | WhenErr | Log::Push_Err_S;
		return;
	}

	Ref_World.WidgetMap[Res::Widget::Load(Pac.WBPIdx)] = R | Unwrap;
}

FWorld Client::ClientLoad(const UObject* G, FPacBuf* PacBuf, const ANetPC* NetPC, clnt_packet_queue& Queue) {
	PACKET_BIND(FPac_SpawnedChrInSrv);
	PACKET_BIND(FPac_MyChrIdx);
	PACKET_BIND(FPac_TimeSync);
	PACKET_BIND(FPac_UpdateWorldStatus);
	PACKET_BIND(FPac_HealthChanged);
	PACKET_BIND(FPac_GamePause);
	PACKET_BIND(FPac_FModPlay);
	PACKET_BIND(FPac_SetVisibility);
	PACKET_BIND(FPac_GameMapLoaded);
	
	PACKET_BIND_GUI(FPac_ItemDrag);
	PACKET_BIND_GUI(FPac_AddItem);
	PACKET_BIND_GUI(FPac_InvLvUpClose);
	PACKET_BIND_GUI(FPac_LvUp);
	PACKET_BIND_GUI(FPac_ItemDrop);
	PACKET_BIND_GUI(FPac_Move2NextLv);
	PACKET_BIND_GUI(FPac_NewLocalWidget);
	
	SetInputMode_UIOnly(NetPC);

	FMod::CheckServer::PC = const_cast<ANetPC*>(NetPC);

	auto World = FWorld { std::vector<FETT>(10) };

	auto [UIMgr, Inv, LvUp] = CreateGUI(GetWorld(G) | Unwrap, const_cast<ANetPC*>(NetPC));
	
	World.Btn_SelectItem = GetWidgetFormName_("Confirm", LvUp->LevelUpWidget);
	World.Btn_InvClose = GetWidgetFormName_("Button", Inv->InventoryWidget);
	World.Inventory = Inv;
	World.LevelUp = LvUp;
	World.InGameUIManager = UIMgr;

	Cast<AGameLoop>(const_cast<UObject*>(G))->StartGameMapLoad();
	Cast<AGameLoop>(const_cast<UObject*>(G))->OnMapLoadingEnd = [&]() { World.CanSpawn = true; };
	World.CanSpawn = true;
	//const FLatentActionInfo LatentInfo;
	//UGameplayStatics::LoadStreamLevel(NetPC, "/Game/02_Graphics/Level/Theme_Forest/LV_0903Test", true, true, LatentInfo);
	
	return World;
}

void SetInv(FInventory* InInv, AActor* InActor) {
	if(Cast<APlayerBase>(InActor)) {
		Cast<APlayerBase>(InActor)->Inventory = InInv;
	} else {
		Panic("SetInv: InActor is not APlayerBase");
	}
}

FWorld Client::ClientTick(const UObject* G, const FPacBuf& PacBuf, FWorld World) {
	auto UpdateWorld = [&](const FWorld& NewWorld) { World = NewWorld; };
	
	const auto NetPC = GetWorld(G) | GetNetPC;
	if (!NetPC.IsOk()) return World;
	
	for (auto& Pac : PacBuf.PlayerPacs) {
		auto NewEtt = [&](const FETT& Ett) { return World.Etts[Ett.EttIdx] = Ett; };
		Pac | NewPlayerSetup | NewEtt | WhenErr | Log::Push_Err_S;
		Pac | GetMyChr | NewEtt | WhenErr | Log::Push_Err_S;
		
		
		if(World.TimeOffset.IsOk()) {
			Pac | Unpack<FPac_TimeSync> | Curry(CalcTimeOffset)(World.TimeOffset.Unwrap())
										| [&](int64 Offset) { World.TimeOffset = Offset; };
		}
		Pac | Unpack<FPac_UpdateWorldStatus> | GetWorldStatus
											 | [&](FWorldStatus Ws) { World.WorldStatus = Ws; };
		
		Pac | Unpack<FPac_HealthChanged> | Curry(HealthChanged)(World) | UpdateWorld;
		Pac | Unpack<FPac_GamePause>	 | Curry(GamePause)(const_cast<UObject*>(G), NetPC.Unwrap());
		Pac | Unpack<FPac_SetVisibility> | [G](FPac_SetVisibility Pac) {
			FindActorByEttIdx(G, Pac.Ett) | YC_Cast<AEntity>
										  | Curry(SetVisible)(Pac.InGroundFriction, Pac.InGravityScale, Pac.bVisible)
										  | WhenErr
										  | Log::Push_Err_S;
		};
		Pac | [&](FPac_FModPlay Sound) {
			Log::Push(L"사운드 패킷 왔음");
			if(Sound.bStop) {
				Log::Push(L"사운드 스탑 패킷 왔음. 키 : " + Sound.EventKey);
				SoundStop(G, Sound.EventKey, Sound.bRelese);
			} else {
				SoundPlay(G, Res::FMod::Load(Sound.EventID), Sound.Pos, Sound.EventKey);
			}
		};
		Pac | Unpack<FPac_GameMapLoaded> | [NetPC, &World](FPac_GameMapLoaded) {
			Log::Push("GameMapLoaded");
			NetPC | SetInputMode_GameOnly;
			World.CanSpawn = true;
		};	
	}
	
	for(auto& Pac : PacBuf.GUIPacs) {
		if(World.LevelUp.IsOk()) {
			APlayerBase* Player = nullptr;
			for (auto& [AChrPtr, Chr, EttIdx] : World.Etts) 
				Chr | [&](FMyChr) {
					if(AChrPtr.IsOk()) { Player = Cast<APlayerBase>(AChrPtr.Unwrap()); }
				};
			
			Pac | Unpack<FPac_AddItem>
				| AddItem(Player, World.LevelUp.Unwrap(), World.Inventory.Unwrap());
		}
		
		Pac | [Inv = World.Inventory, NetPC] (FPac_InvLvUpClose) -> ErrorOr<void> {
			if(Inv.IsErr()) return Err { Inv.GetError() };
			
			const TArray<int> Items = Inv.Unwrap()->InventoryWidget->RemoveTrashedItems();
			if((NetPC | HasAuthority).Or(false)) {
				Send(FPac_RemoveItem{ Items }, NetPC.Unwrap());
			}
			Inv.Unwrap()->InventoryWidget->Hide();
			return {};
		};
		Pac | [LvUp = World.LevelUp](FPac_LvUp Pac) -> ErrorOr<void> {
			if(LvUp.IsOk()) {
				std::vector<int> Items;
				for(auto& i : Pac.Items) Items.push_back(i);
				LvUp.Unwrap()->OnLevelUp2(Items);
			}
			return {};
		};
		if(World.Inventory.IsOk()) {
			Pac | Unpack<FPac_ItemDrag> | RecvItemDragPos(World.Inventory.Unwrap());
			Pac | Unpack<FPac_ItemDrop> | Curry(OnItemDrop, World.Inventory.Unwrap());
		}
		Pac | Unpack<FPac_NewLocalWidget> | [NetPC, &World](FPac_NewLocalWidget Pac) {
			if(Pac.bShow) NewWidget(Pac, NetPC.Unwrap(), World);
			else {
				const auto Wd = Res::Widget::Load(Pac.WBPIdx);
				if(World.WidgetMap.contains(Wd)) {
					World.WidgetMap[Wd]->RemoveFromParent();
					World.WidgetMap.erase(Wd);
				}
			}
		};
		Pac | Unpack<FPac_Move2NextLv> | [NetPC](FPac_Move2NextLv) {
			Log::Push("Move2 Next lv");
			// TODO : 다음 레벨로 이동
			auto Wd = NetPC | CreateWidget(Res::WBP_FadeInOut) | YC_Cast<UDisplayEffects>;
			Wd | AddToViewport | WhenErr | Log::Push_Err_S;
			
			if(Wd.IsErr()) return;
			
			Wd.Unwrap()->FadeIn();
		};
	}
	
	return World;
}

FWorld Client::ClientRenderTick(const UObject* G, FWorld World) {
	const auto NetPC = GetWorld(G) | GetNetPC;
	if (!NetPC.IsOk()) return World;
	bool bServer = NetPC | HasAuthority | Unwrap;
	
	// TIME SYNC
	if(!bServer) { 
		if(World.TimeOffset.IsErr()) {
			World.TimeOffset = Time::Epoch_Ms();
			Send(FPac_TimeSync{}, NetPC.Unwrap());
		} else {
			Cast<UNetGameInstance>(GetWorld(G).Unwrap()->GetGameInstance())
			->LocalData.TimeOffset = World.TimeOffset.Unwrap();
		}
	}
	
	// UI
	{
		auto CreateMsg = [&](FString Msg) -> ErrorOr<void> {
			auto W = NetPC | CreateWidget(Res::WBP_ChrMenuMsg);
			W | AddToViewport | WhenErr | Log::Push_Err_S;
			W | GetTextBlock("Msg") | SetTextBlock(FText::FromString(Msg)) | WhenErr | Log::Push_Err_S;
			return {};
		};
		if(World.CanSpawn && !World.IsCharacterPicked && (NetPC | HasAuthority | Unwrap ? NetPC.Unwrap()->bInit : true)) {
			auto Gm = Cast<UNetGameInstance>((NetPC | Unwrap)->GetGameInstance());

			Log::Push(YC_FMT("[YCDebug] Send! {}", NetPC | HasAuthority | Unwrap ? "Server" : "Client"));
			
			if(!Gm->PickCharacterType)
				 Send(FPac_SpawnAndPossess{ Res::EPlayer::Padma }, NetPC.Unwrap()) | WhenErr | Log::Push_Err_S;
			else Send(FPac_SpawnAndPossess{ Res::EPlayer::Feama }, NetPC.Unwrap()) | WhenErr | Log::Push_Err_S;

			World.IsCharacterPicked = true;
		}
		
		World.Btn_SelectItem | IsBtnUp | [&World, NetPC](std::pair<YcButton, bool> R) {
			auto [NewBtn, Result] = R;
			World.Btn_SelectItem = NewBtn;
			if (Result) {
				const auto ItemId = World.LevelUp.Unwrap()->SelectedItemId;
				if(ItemId != 0) {
					World.LevelUp.Unwrap()->SelectedItemId = 0;
					Send(FPac_PickItem { ItemId }, NetPC.Unwrap());
				}
			}
		};
		World.Btn_InvClose | IsBtnUp | [&World, NetPC](std::pair<YcButton, bool> R) {
			auto [NewBtn, Result] = R;
			World.Btn_InvClose = NewBtn;
			if (Result) {
				if(World.Inventory.Unwrap()->WithLvUp) {
					Send(FPac_LocalInvLvUpClose {}, NetPC.Unwrap());
					(World.Inventory | Unwrap)->SetDragMode(false);
				} else {
					World.Inventory.Unwrap()->InventoryWidget->Hide();
					for (auto& [AChrPtr, Chr, EttIdx] : World.Etts) {
						if(AChrPtr.IsOk()) { 
							Chr | [AChrPtr](FMyChr) {
								Cast<APlayerBase>(AChrPtr.Unwrap())->SetIgnoreMoveInput(false);
							};
						}
					}
				}
			}
		};
		
		World.Inventory | Curry(SendItemDragData, NetPC.Unwrap());
		
		// TODO : 언리얼 Input System을 사용하도록 수정
		if(World.LevelUp.IsOk()) {
			if((NetPC | PressedKey(EKeys::Escape)).Or(false)) {
				if(!World.Inventory.Unwrap()->WithLvUp) {
					World.Inventory.Unwrap()->InventoryWidget->Hide();
					for (auto& [AChrPtr, Chr, EttIdx] : World.Etts) {
						if(AChrPtr.IsOk()) { 
							Chr | [AChrPtr](FMyChr) {
								Cast<APlayerBase>(AChrPtr.Unwrap())->SetIgnoreMoveInput(false);
							};
						}
					}
				}
			}
			if ((NetPC | PressedKey(EKeys::Tab)).Or(false))
				World.Inventory | [World](FInventory* Inv) {
				Inv->Show();
				for (auto& [AChrPtr, Chr, EttIdx] : World.Etts) {
					if(AChrPtr.IsOk()) {
						Chr | [AChrPtr](FMyChr) {
							Cast<APlayerBase>(AChrPtr.Unwrap())->SetIgnoreMoveInput(true);
						};
					}
				}
			};
			if ((NetPC | PressedKey(EKeys::L)).Or(false))
				World.LevelUp | [NetPC](FLevelUp*) {
					Send(FPac_LvUp{} , NetPC.Unwrap());
				};
		}
		
		World.HUD | YC_Cast<UInGameWidget> | [World](const UInGameWidget* HUD) {
			HUD->SetExp2(World.WorldStatus.Exp, World.WorldStatus.Lv);
		};
	}
	
	//Update Ett
	for (auto& [AChrPtr, Chr, EttIdx] : World.Etts) {
		if (AChrPtr.IsErr() && AChrPtr.GetError() == EttStateType::BeforeSpawn) {
			auto EttFind = FindActorByEttIdx(G, EttIdx);
			
			AChrPtr = EttFind.IsOk() ? ErrorOr<AEntity*, EttStateType::EType> { EttFind | YC_Cast<AEntity> | Unwrap }
									 : ErrorOr<AEntity*, EttStateType::EType> { EttStateType::BeforeSpawn };
			
			if(AChrPtr.IsErr()) continue;
			Chr | [&](FOtChr) {
				if(World.Inventory.IsErr()) {
					World.Inventory | WhenErr | Log::Push_Err_S;
				}
				
				World.Inventory.Expect();
				
				AChrPtr | Curry(SetInv)(World.Inventory | Unwrap);
			};
			Chr | [&](FMyChr C) {
				APlayerBase* Ptr = Cast<APlayerBase>(AChrPtr | Unwrap);
				Cast<AGameLoop>(const_cast<UObject*>(G))->Target = Ptr;

				if (Ptr == nullptr) return;
				
				Cast<INetChr>(Ptr)->NetBegin();
				NetPC | SetInputMode_GameOnly;
				World.HUD = NetPC | CreateWidget(Res::WBP_InGameWidget);
				World.HUD | YC_Cast<UInGameWidget> | Curry(UInGameWidget::SetHealth)(C.Stt.Hp, C.Stt.HpMax);
				World.HUD | YC_Cast<UInGameWidget> | Curry(UInGameWidget::SetMyPlayer)(Ptr);
				World.HUD | YC_Cast<UInGameWidget> | Curry(UInGameWidget::SetMyPortrait)(Res::LoadPortrait(C.EttType));
				World.HUD | AddToViewport;

				World.Inventory.Expect();
				
				Ptr->Inventory = World.Inventory | Unwrap;
			};
		}
	}
	enum ECharacterFindType { NotFound };
	ErrorOr<FETT, ECharacterFindType> MyChr = Err { NotFound };
	ErrorOr<FETT, ECharacterFindType> OtherChr = Err { NotFound };
	for (auto& Ett : World.Etts | std::views::filter(HasPtr)) {
		Ett.Chr | [&](FMyChr) { MyChr = Ett; };
		Ett.Chr | [&](FOtChr) { OtherChr = Ett; };
	}
	
	for (auto& Ett : World.Etts | std::views::filter(HasPtr)) {
		const auto bDead = IsDead(Ett.Chr);
		Ett.Chr | [&](FMyChr) {
			if(bDead) {
				AActor* Other = nullptr;
				if(OtherChr.IsOk()) Other = OtherChr.Unwrap().AChrPtr.Or(nullptr);
				Ett.AChrPtr | Curry(APlayerBase::SetDead)(Other);
			}
		};
		Ett.Chr | [&](FOtChr O) {
			World.HUD | YC_Cast<UInGameWidget>
					  | Curry(UInGameWidget::UpdateOt)(O.Stt.Hp, O.Stt.HpMax, Res::LoadPortrait(O.EttType));
			if(bDead) {
				AActor* Other = nullptr;
				if(MyChr.IsOk()) Other = MyChr.Unwrap().AChrPtr.Or(nullptr);
				Ett.AChrPtr | Curry(APlayerBase::SetDead)(Other);
			}
		};
		if(bDead) Ett.AChrPtr = EttStateType::Dead;
	}
	return World;
}