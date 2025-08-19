#include "Updrafts.h"

#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Dongjun/Monster/Monster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Taehun/NirvanaCharacterBase.h"


AUpdrafts::AUpdrafts() {
	//PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);
}

void AUpdrafts::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const auto Character = Cast<ANirvanaCharacterBase>(OtherActor);
	
	if(IsValid(Character)) {
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		const auto Mob = Cast<AMonster>(OtherActor);
		if(IsValid(Mob)) {
			// Mob->AIController->MoveToLocation(Mob->GetActorLocation());
			Mob->Launch(Arrow->GetForwardVector() * Arrow->ArrowLength);
		} else {
			// Character->LaunchCharacter(Arrow->GetForwardVector() * Arrow->ArrowLength, true, true);
			Character->GetCharacterMovement()->Launch(Arrow->GetForwardVector() * Arrow->ArrowLength);
		}
	}
}

void AUpdrafts::BeginPlay() {
	Super::BeginPlay();

	if(!HasAuthority()) return;
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AUpdrafts::OnBeginOverlap);
}

void AUpdrafts::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

