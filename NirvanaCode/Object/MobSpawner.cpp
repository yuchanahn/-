#include "MobSpawner.h"

#include <YC/Coroutine/Coro.hpp>

#include "YCUtils.h"
#include "Serialization/Csv/CsvParser.h"
#include "Web/Http.hpp"
#include "YC/Core/Server/Server.hpp"

AMobSpawner::AMobSpawner(): BattleFiledID(0) {
	PrimaryActorTick.bCanEverTick = true;

	BP_Monsters.Add(0, Res::BP_Monster_Gluttony_Melee);
	BP_Monsters.Add(1, Res::BP_Monster_Gluttony_Ranged);
	BP_Monsters.Add(2, Res::BP_Monster_Bambam);
}

void AMobSpawner::BeginPlay() {
	Super::BeginPlay();
}

void AMobSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

void AMobSpawner::BeforeSpawn(std::function<void(TArray<AMonster*>)> Callback) {
	bStartSpawn = true;
	this->Callback = Callback;
}

void AMobSpawner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(!HasAuthority()) return;

	if(bStartSpawn) {
		bStartSpawn = false;
		//auto& Rows = FCsvParser(CSV).GetRows();
		//const auto& Data = Rows[BattleFiledID+1];
		//YC::Log::Push_S(YC_FMT("Data num : {}", Data.Num()));
		//for(int i = 1; i < Data.Num(); i++) {
		//	if(FCString::Strlen(Data[i]) == 0) {
		//		for(const auto D : Data) {
		//			YC::Log::Push_S(YC_FMT("YC_Data : {}", FString(D)));
		//		}
		//		YC::Log::Push_S(YC_FMT("YC_Data----------"));
		//		break;
		//	}
		//	FString str = FString(Data[i]);
		//	if (i % 2) MobKey.Add(FCString::Atoi(*str));
		//	else MobCount_.Add(FCString::Atoi(*str));
		//	//YC::Log::Push_S(YC_FMT("Atoi : {}", FCString::Atoi(*FString(Data[i]))));
		//}
		MobKey = {0, 1, 2};
		MobCount_ = {2, 2, 2};
		
		for (auto i = 0; i < MobKey.Num(); i++) {
			if(MobCount_.Num() <= i) break;
			for(int j = 0; j < MobCount_[i]; j++) MonsterQueue.Enqueue(MobKey[i]);
		}
		YC::Log::Push_S(YC_FMT("MonsterQueue Size : {},{},{}", MonsterQueue.IsEmpty(), MobKey.Num(), MobCount_.Num()));
		bIsSpawned = false;
	}

	if(!bIsSpawned) {
		SpawnTimer += DeltaTime;
		if(SpawnTimer >= 0.5f && !MonsterQueue.IsEmpty()) {
			SpawnTimer = 0;
			int32 MobKey = 0;
			MonsterQueue.Dequeue(MobKey);
			MonsterList.Add(SpawnMonster(BP_Monsters[MobKey]));
			if(MonsterQueue.IsEmpty()) {
				bIsSpawned = true;
				Callback(MonsterList);
			}
		}
	}
}

AMonster* AMobSpawner::SpawnMonster(TSubclassOf<AMonster> BP_Monster) const {
	if (!HasAuthority()) return nullptr;
	//if (Activator->GetAttributeComp()->GetHealth() <= 0) return;
	
	const FTransform SpawnTransform(FRotator::ZeroRotator, GetActorLocation());
	const auto Monster = GetWorld()->SpawnActorDeferred<AMonster>(BP_Monster, SpawnTransform, nullptr, nullptr,
																  ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(Monster))
	{
		//Monster->AggroTarget = Activator;
		Monster->SetLevelData();
		Monster->FinishSpawning(SpawnTransform);
		return Monster;
	} else {
		UE_LOG(LogTemp, Error, TEXT("Failed to Spawn a Monster!"));
	}
	return nullptr;
}

