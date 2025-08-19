// Fill out your copyright notice in the Description page of Project Settings.


#include "MobSpawnTrigger.h"

#include "YCUtils.h"
#include "Components/BoxComponent.h"
#include "YC/Core/Log/Log.hpp"
#include "YC/Core/Server/Server.hpp"


// Sets default values
AMobSpawnTrigger::AMobSpawnTrigger() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
}

void AMobSpawnTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (!HasAuthority()) return;
	
	if (const auto Chr = Cast<ANirvanaCharacterBase>(OtherActor)) {
		if(Chr->GetAttributeComp()->GetHealth() <= 0) return;
		
		ItemObject->RegisterObject();

		ItemObject->Walls = Walls;
		for (auto Monster : SpawnedMonsters) {
			Monster->AggroTarget = Chr;
			Monster->bIsHiddenDone = true;
		}
		YC::Server::FServer::TargetCount = SpawnedMonsters.Num();
		for(const auto Wall : Walls) {
			Wall->NMC_BlockAll();
		}
	}
	Destroy();
}

// Called when the game starts or when spawned
void AMobSpawnTrigger::BeginPlay() {
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AMobSpawnTrigger::OnOverlapBegin);
	YC::Log::Push_S(YC_FMT("몬스터 스포너 준비중!"));
	Spawner->BeforeSpawn([&](TArray<AMonster*> Monsters) {
		SpawnedMonsters = Monsters;
		YC::Log::Push_S(YC_FMT("몬스터 스폰 완료!"));
	});
}

// Called every frame
void AMobSpawnTrigger::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

