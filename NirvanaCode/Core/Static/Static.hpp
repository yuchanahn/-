#pragma once
#include "EngineUtils.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Button.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CheckBox.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "YC/ErrorOr/ErrorOr.hpp"
#include "YC/Curry/Curry.hpp"
#include "YC/Core/NetPC.h"
#include "YC/Core/ActorIndexingComp.h"
#include "YC/Core/UI/UI.hpp"

class UCameraComponent;

namespace Mem {
inline ErrorOr<void> FreeU(UObject*& Ptr) {
	if (Ptr) {
		Ptr->ConditionalBeginDestroy();
		Ptr = nullptr;
		return {};
	}
	return Err{std::string("Ptr is nullptr!")};
}
}

namespace YC {
static ErrorOr<APlayerController*> GetController(const ACharacter* InChr) {
	const auto PlayerController = InChr->GetController<APlayerController>();
	if (IsValid(PlayerController)) { return PlayerController; }
	return Err{std::string("PlayerController is not valid")};
}

static ErrorOr<UEnhancedInputLocalPlayerSubsystem*> GetSubsystem(const APlayerController* InContr) {
	const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InContr->GetLocalPlayer());
	if (IsValid(Subsystem)) { return Subsystem; }
	return Err{std::string("Subsystem is not valid")};
}

static ErrorOr<UEnhancedPlayerInput*> GetPlayerInput(const UEnhancedInputLocalPlayerSubsystem* InContr) {
	const auto InputComp = InContr->GetPlayerInput();
	if (IsValid(InputComp)) { return InputComp; }
	return Err{std::string("InputComp is not valid")};
}

static ErrorOr<USkeletalMeshComponent*> GetMesh(const ACharacter* InChr) {
	if(!IsValid(InChr)) return Err{std::string("Character is not valid")};
	const auto Mesh = InChr->GetMesh();
	if (IsValid(Mesh)) { return Mesh; }
	return Err{std::string("Character Mesh is not valid")};
}

static ErrorOr<void> SetSkeletalMesh(USkeletalMesh* InMesh, USkeletalMeshComponent* InMeshComponent) {
	if (IsValid(InMeshComponent) && IsValid(InMesh)) {
		InMeshComponent->SetSkeletalMesh(InMesh);
		return {};
	}
	return Err{std::string("MeshComponent or Mesh is not valid")};
}

static ErrorOr<void> SetRelativeLocation(const FVector& InLocation, USkeletalMeshComponent* InMeshComponent) {
	if (IsValid(InMeshComponent)) {
		InMeshComponent->SetRelativeLocation(InLocation);
		return {};
	}
	return Err{std::string("MeshComponent is not valid")};
}

static ErrorOr<void> SetRelativeRotation(const FRotator& InRotation, USkeletalMeshComponent* InMeshComponent) {
	if (IsValid(InMeshComponent)) {
		InMeshComponent->SetRelativeRotation(InRotation);
		return {};
	}
	return Err{std::string("MeshComponent is not valid")};
}

static ErrorOr<void> SetAnimInstance(UClass* InAnimInstance, USkeletalMeshComponent* InMeshComponent) {
	if (IsValid(InMeshComponent) && IsValid(InAnimInstance)) {
		InMeshComponent->SetAnimInstanceClass(InAnimInstance);
		return {};
	}
	return Err{std::string("MeshComponent or AnimInstance is not valid")};
}

static ErrorOr<UInputAction*> NewIA_(UClass*& C, UObject* InInputComp) {
	if (IsValid(InInputComp) && IsValid(C)) { return NewObject<UInputAction>(InInputComp, C); }
	return Err{std::string(!IsValid(C) ? "StaticClass is not valid" : "InputComp is not valid")};
}
//NewIA_(UClass*& C, UObject* InInputComp) -> ErrorOr<UInputAction*>
static auto NewIA = Curry(NewIA_);

static ErrorOr<void> AddMappingContext_(const UInputMappingContext* InImc, const int32 InPriority,
										UEnhancedInputLocalPlayerSubsystem* InSubSys) {
	if (IsValid(InSubSys) && IsValid(InImc)) {
		InSubSys->AddMappingContext(InImc, InPriority);
		return {};
	}
	return Err{std::string("Subsystem or InputMappingContext is not valid")};
}
//AddMappingContext_(const UInputMappingContext* InImc, const int32 InPriority, UEnhancedInputLocalPlayerSubsystem* InSubSys) -> ErrorOr<void>
static auto AddMappingContext = Curry(AddMappingContext_);

static ErrorOr<UWorld*> GetWorld(const UObject* InObj) {
	if (!IsValid(InObj)) return Err{std::string("InObj is not valid")};
	UWorld* World = InObj->GetWorld();
	if (IsValid(World)) { return World; }
	return Err{std::string("World is not valid")};
}

static ErrorOr<ANetPC*> GetNetPC(const UWorld* InWorld) {
	if (!IsValid(InWorld)) return Err{std::string("InWorld is not valid")};
	if (!IsValid(InWorld->GetFirstPlayerController())) return Err{
		std::string("InWorld->GetFirstPlayerController() is not valid")
	};
	const auto NetPC = Cast<ANetPC>(InWorld->GetFirstPlayerController());
	if (IsValid(NetPC)) { return NetPC; }
	return Err{std::string("NetPC is not valid")};
}

static ErrorOr<bool> HasAuthority(const ANetPC* InNetPC) {
	if (IsValid(InNetPC)) { return InNetPC->HasAuthority(); }
	return Err{std::string("InNetPC is not valid")};
}

template <typename Type>
static ErrorOr<Type*> AddComp(UObject* InObj) {
	if (IsValid(InObj)) {
		if (auto R = NewObject<Type>(InObj, Type::StaticClass())) { return R; }
		return Err{std::string("NewObject<Type> is Failed")};
	}
	return Err{std::string("InObj is not valid")};
}

template <typename Type>
static ErrorOr<Type*> YC_Cast(UObject* InObj) {
	if (IsValid(InObj)) {
		if (auto R = Cast<Type>(InObj)) { return R; }
		return Err{std::string("Cast<Type> is Failed")};
	}
	return Err{std::string("InObj is not valid")};
}

template <typename Type>
static Type* ACast(AActor* InObj) {
	if (IsValid(InObj)) {
		if (auto R = Cast<Type>(InObj)) { return R; }
	}
	return nullptr;
}


static ErrorOr<USceneComponent*> GetRootComp(const AActor* InActor) {
	if (IsValid(InActor)) { return InActor->GetRootComponent(); }
	return Err{std::string("InActor is not valid")};
}

static ErrorOr<void> SetupAttachment_(USceneComponent* InParent, FName InSocketName, USceneComponent* InComp) {
	if (IsValid(InComp) && IsValid(InParent)) {
		InComp->SetupAttachment(InParent, InSocketName);
		return {};
	}
	return Err{std::string("InChild or InParent is not valid")};
}

//SetupAttachment_(USceneComponent* InParent, FName InSocketName, USceneComponent* InComp) -> ErrorOr<void>
inline auto SetupAttachment = Curry(SetupAttachment_);

static ErrorOr<void> SetUsePawnControlRotation_(bool InUse, UCameraComponent* InComp) {
	if (IsValid(InComp)) {
		InComp->bUsePawnControlRotation = InUse;
		return {};
	}
	return Err{std::string("InComp is not valid")};
}

static ErrorOr<void> SetUsePawnControlRotation_2(bool InUse, USpringArmComponent* InComp) {
	if (IsValid(InComp)) {
		InComp->bUsePawnControlRotation = InUse;
		return {};
	}
	return Err{std::string("InComp is not valid")};
}
//SetUsePawnControlRotation_(bool InUse, UCameraComponent* InComp) -> ErrorOr<void>
inline auto SetUsePawnControlRotation = Curry(SetUsePawnControlRotation_);
//SetUsePawnControlRotation_2(bool InUse, USpringArmComponent* InComp) -> ErrorOr<void>
inline auto SetUsePawnControlRotation_SpringArm = Curry(SetUsePawnControlRotation_2);

static ErrorOr<void> SetTargetArmLength_(const float InLength, USpringArmComponent* InComp) {
	if (IsValid(InComp)) {
		InComp->TargetArmLength = InLength;
		return {};
	}
	return Err{std::string("InComp is not valid")};
}

//SetTargetArmLength_(const float InLength, USpringArmComponent* InComp) -> ErrorOr<void>
inline auto SetTargetArmLength = Curry(SetTargetArmLength_);

static ErrorOr<UActorComponent*> RegisterComponent(UActorComponent* InComp) {
	if (IsValid(InComp)) {
		InComp->RegisterComponent();
		return InComp;
	}
	return Err{std::string("InComp is not valid")};
}

static void PrintError(is_t_err_opt auto InErr) {
	if (InErr.IsErr()) { UE_LOG(LogTemp, Error, TEXT("%s"), InErr.GetError().c_str()); }
}

static ErrorOr<UUserWidget*> CreateWidget_(const TSubclassOf<UUserWidget> C, APlayerController* InObj) {
	if(!IsValid(InObj)) return Err{std::string("APlayerController* is not valid")};
	if(!IsValid(C)) return Err{std::string("UUserWidget is not valid")};
	
	return CreateWidget<UUserWidget>(InObj, C);
}
//CreateWidget_(const TSubclassOf<UUserWidget> C, APlayerController* InObj) -> ErrorOr<UUserWidget*>
inline auto CreateWidget = Curry(CreateWidget_);

static ErrorOr<void> AddToViewport(UUserWidget* InWidget) {
	if (IsValid(InWidget)) {
		InWidget->AddToViewport();
		return {};
	}
	return Err{std::string("InWidget is not valid")};
}

static ErrorOr<UActorIndexingComp*> SetIndex_For_ActorIndexingComp_(const int32 InIndex, UActorIndexingComp* InComp) {
	if (IsValid(InComp)) {
		InComp->OwnID = InIndex;
		return InComp;
	}
	return Err{std::string("InComp is not valid")};
}

//SetIndex_For_ActorIndexingComp_(const int32 InIndex, UActorIndexingComp* InComp) -> ErrorOr<UActorIndexingComp*>
inline auto SetIndex_For_ActorIndexingComp = Curry(SetIndex_For_ActorIndexingComp_);

static ErrorOr<void> SetInputMode(APlayerController* InPC, const FInputModeDataBase& InMode) {
	if (IsValid(InPC)) {
		InPC->SetInputMode(InMode);
		return {};
	}
	return Err{std::string("InPC is not valid")};
}

//SetInputMode_GameOnly(APlayerController* InPC) -> ErrorOr<void>
inline auto SetInputMode_GameOnly = Curry(SetInputMode)(FInputModeGameOnly());
//SetInputMode_UIOnly(APlayerController* InPC) -> ErrorOr<void>
inline auto SetInputMode_UIOnly = Curry(SetInputMode)(FInputModeUIOnly());
//SetInputMode_GameAndUI(APlayerController* InPC) -> ErrorOr<void>
inline auto SetInputMode_GameAndUI = Curry(SetInputMode)(FInputModeGameAndUI());

static std::vector<AActor*> GetAllActors(const UObject* InObj) {
	if (IsValid(InObj)) {
		std::vector<AActor*> R;
		const auto World = GetWorld(InObj);
		if(World.IsErr()) { return {}; }
		for (TActorIterator<AActor> ActorItr(World.Unwrap()); ActorItr; ++ActorItr)
		{
			R.push_back(*ActorItr);
		}
		return R;
	}
	return {};
}

static std::vector<AActor*> FindAllEtt(const UObject* InObj) {
	std::vector<AActor*> R = {};
	for(auto i : GetAllActors(InObj)) {
		if(i->FindComponentByClass<UActorIndexingComp>()) {
			R.push_back(i);
		}
	}
	return R;
}



static ErrorOr<YcButton> GetWidgetFormName_(const FName InName, const UUserWidget* InWidget) {
	if (IsValid(InWidget)) {
		auto Widget = InWidget->GetWidgetFromName(InName);
		if(Widget == nullptr) { return Err{std::string("Widget Name Not Founded!")}; }
		const auto Button = Cast<UButton>(Widget);
		if(Button != nullptr) { return YcButton { Button, false }; }
		return Err{std::string("Widget is not UButton")};
	}
	return Err{std::string("Widget is not Valid")};
}

//GetButton_(const FName, const UUserWidget) -> ErrorOr<UButton*>
inline auto GetButton = Curry(GetWidgetFormName_);

static ErrorOr<UTextBlock*> GetTextBlock_(const FName InName, const UUserWidget* InWidget) {
	if (IsValid(InWidget)) {
		const auto Widget = InWidget->GetWidgetFromName(InName);
		if(Widget == nullptr) { return Err{std::string("Widget Name Not Founded!")}; }
		const auto TextBlock = Cast<UTextBlock>(Widget);
		if(TextBlock != nullptr) { return TextBlock; }
		return Err{std::string("Widget is not UButton")};
	}
	return Err{std::string("Widget is not Valid")};
}

//GetTBWidget_(const FName, const UUserWidget) -> ErrorOr<UTextBlock*>
inline auto GetTextBlock = Curry(GetTextBlock_);

static void SetTextBlock_(const FText& InText, UTextBlock* InTextBlock) {
	if (IsValid(InTextBlock)) {
		InTextBlock->SetText(InText);
	}
}
//SetTextBlock_(const FText, UTextBlock) -> void
inline auto SetTextBlock = Curry(SetTextBlock_);


static ErrorOr<YcButton> UpdateBtn(YcButton InButton) {
	if (InButton.Button != nullptr) {
		InButton.bWasPressed = InButton.Button->IsPressed();
		return InButton;
	}
	return Err{std::string("InButton is not valid")};
}

static ErrorOr<std::pair<YcButton, bool>> IsPressed(const YcButton InButton) {
	if (InButton.Button != nullptr) {
		const auto Updated_Btn = UpdateBtn(InButton);
		if(Updated_Btn.IsErr()) { return Err{Updated_Btn.GetError()}; }
		return std::make_pair(Updated_Btn.Unwrap(), InButton.Button->IsPressed());
	}
	return Err{std::string("InButton is not valid")};
}

static ErrorOr<std::pair<YcButton, bool>> IsHovered(const YcButton InButton) {
	if (InButton.Button != nullptr) {
		const auto Updated_Btn = UpdateBtn(InButton);
		if(Updated_Btn.IsErr()) { return Err{Updated_Btn.GetError()}; }
		return std::make_pair(Updated_Btn.Unwrap(), InButton.Button->IsHovered());
	}
	return Err{std::string("InButton is not valid")};
}

static ErrorOr<std::pair<YcButton, bool>> IsBtnDown(const YcButton InButton) {
	if (InButton.Button != nullptr) {
		const auto Updated_Btn = UpdateBtn(InButton);
		if(Updated_Btn.IsErr()) { return Err{Updated_Btn.GetError()}; }
		return std::make_pair(Updated_Btn.Unwrap(), !InButton.bWasPressed && InButton.Button->IsPressed());
	}
	return Err{std::string("InButton is not valid")};
}

static ErrorOr<std::pair<YcButton, bool>> IsBtnUp(const YcButton InButton) {
	if (InButton.Button != nullptr) {
		const auto Updated_Btn = UpdateBtn(InButton);
		if(Updated_Btn.IsErr()) { return Err{Updated_Btn.GetError()}; }
		return std::make_pair(Updated_Btn.Unwrap(), InButton.bWasPressed && !InButton.Button->IsPressed() && InButton.Button->IsHovered());
	}
	return Err{std::string("InButton is not valid")};
}


static ErrorOr<UCheckBox*> GetCheckBox_(const FName InName, const UUserWidget* InWidget) {
	if (IsValid(InWidget)) {
		const auto Widget = InWidget->GetWidgetFromName(InName);
		if(Widget == nullptr) { return Err{std::string("Widget Name Not Founded!")}; }
		const auto CheckBox = Cast<UCheckBox>(Widget);
		if(CheckBox != nullptr) { return CheckBox; }
		return Err{std::string("Widget is not UCheckBox")};
	}
	return Err{std::string("Widget is not Valid")};
}
//GetCheckBox_(const FName, const UUserWidget) -> ErrorOr<UCheckBox*>
inline auto GetCheckBox = Curry(GetCheckBox_);


static bool IsChecked(const UCheckBox* InCheckBox) {
	return InCheckBox->IsChecked();
}

static ErrorOr<void> FinishSpawningActor_(const FTransform& InTf, AActor* InActor) {
	if (IsValid(InActor)) {
		InActor->FinishSpawning(InTf);
		if(!IsValid(InActor)) { return Err{std::string("FinishSpawning Actor is not valid")}; }
		return {};
	}
	return Err{std::string("InActor is not valid")};
}
//FinishSpawningActor_(const FTransform&, AActor*) -> ErrorOr<void>
inline auto FinishSpawningActor = Curry(FinishSpawningActor_);

static ErrorOr<void> SetOrientRot2Move_(const bool bInOrientRotationToMovement, const ACharacter* InChr) {
	if (IsValid(InChr)) {
		const auto MovementComp = InChr->GetCharacterMovement();
		if(IsValid(MovementComp)) {
			MovementComp->bOrientRotationToMovement = bInOrientRotationToMovement;
			return {};
		}
		return Err{std::string("MovementComp is not valid")};
	}
	return Err{std::string("InActor is not valid")};
}
//SetOrientRot2Move_(const bool, const ACharacter*) -> ErrorOr<void>
inline auto SetOrientRot2Move = Curry(SetOrientRot2Move_);


static ErrorOr<bool> PressedKey_(const FKey& InKey, const APlayerController* InPC) {
	if (IsValid(InPC)) {
		return InPC->WasInputKeyJustPressed(InKey);
	}
	return Err{std::string("InPC is not valid")};
}
//PressedKey_(const FKey&, const APlayerController*) -> ErrorOr<bool>
inline auto PressedKey = Curry(PressedKey_);

static ErrorOr<FVector2d> GetSlotPos(const UCanvasPanelSlot* Slot) {
	if (IsValid(Slot)) {
		return Slot->GetPosition();
	}
	return Err{std::string("Slot is not valid")};
}

static ErrorOr<void> RemoveFromParent(UWidget* InWidget) {
	if (IsValid(InWidget)) {
		InWidget->RemoveFromParent();
		return {};
	}
	return Err{std::string("InWidget is not valid")};
}

template <typename Type>
static Type* Comp(AActor* InObj) {
	if (!IsValid(InObj)) {
		return nullptr;
	}
	if (auto R = InObj->FindComponentByClass<Type>()) { return R; }
	return nullptr;
}

inline auto HasPtr = [](const auto& Ett) {
	return Ett.AChrPtr.IsOk() && Ett.AChrPtr.Unwrap() != nullptr;
};

template <typename T>
TArray<T> Take(int Count, TArray<T> Array) {
	TArray<T> Result;
	for (int i = 0; i < Count; ++i) {
		if (Array.Num() == 0) break;
		Result.Add(Array.Pop());
	}
	return Result;
}

template <typename T>
TArray<T> TakeRng(int Start, int End, TArray<T> Array) {
	TArray<T> Result;
	for (int i = Start; i <= End; ++i) {
		if (Array.Num() <= i) break;
		Result.Add(Array[i]);
	}
	return Result;
}

template <typename T>
TArray<T> RemoveRange(TArray<T> Range, TArray<T> Array) {
	for (const auto& Item : Range) Array.Remove(Item);
	return Array;
}


}// 마지막 중괄호!


template <typename Type>
static ErrorOr<Type*> Spawn(const UObject* InObj, const TSubclassOf<Type>& Class, const FVector& InLoc,
                            const FRotator& InRot) {
	const auto World = InObj->GetWorld();
	if (IsValid(World)) {
		auto Pram = FActorSpawnParameters{};
		const auto Actor = World->SpawnActor<Type>(Class, InLoc, InRot, Pram);
		if (IsValid(Actor)) { return Actor; }
		return Err{std::string("SpawnActor failed!")};
	}
	return Err{std::string("InObj->GetWorld() failed!")};
}

template <typename Type>
static ErrorOr<Type*> SpawnActorDef(const UObject* InObj, const TSubclassOf<Type>& Class, FTransform InTf) {
	const auto World = InObj->GetWorld();
	if (IsValid(World)) {
		const auto Actor = World->SpawnActorDeferred<Type>(Class, InTf, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (IsValid(Actor)) { return Actor; }
		return Err{std::string("SpawnActorDef failed!")};
	}
	return Err{std::string("InObj->GetWorld() failed!")};
}
