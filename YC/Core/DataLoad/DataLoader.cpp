// Fill out your copyright notice in the Description page of Project Settings.


#include "DataLoader.h"

#include "Web/Http.hpp"
#include "YC/Core/Log/Log.hpp"
#include "YC/Core/Utils/Functional.hpp"


// Sets default values
ADataLoader::ADataLoader() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	YC::Log::Push(L"DataLoader 생성자 호출");
	
	YC::Http::Spreadsheets::Get(L"13ToFTwycWXJz1R6CEBidvD--8lcMwAcTZvlId0xyH8w", L"MobSpawn", [](ErrorOr<FString> Msg) {
			Msg | YC::Log::Push;
			Msg | WhenErr | YC::Log::Push_Err_S;
			
			if(Msg.IsErr()) return;
			
			YC::Log::Push_Dp(Msg.Unwrap());
			
			MobSpawnData = Msg.Unwrap();
			YC::Log::Push("@@@ === " + Msg.Unwrap());
		});
}

// Called when the game starts or when spawned
void ADataLoader::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void ADataLoader::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

