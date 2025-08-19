#include "ItemObject.h"

#include <YC/Coroutine/Coro.hpp>

#include "YC/Core/Server/Server.hpp"


AItemObject::AItemObject() {
	PrimaryActorTick.bCanEverTick = true;
}

void AItemObject::BeginPlay() {
	Super::BeginPlay();
}

void AItemObject::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AItemObject::RegisterObject() {
	Coro::CoStart([](AItemObject* Self) -> coroutine {
		co_yield wait_time(0.3f);
		YC::Server::FServer::StageStart = Self;
		co_return;
	}(this));
}

void AItemObject::Start() {
	IsStart = true;
	if(HasAuthority()) {
		OnStart();
		const auto Pram = FActorSpawnParameters{ };
		MyMesh = GetWorld()->SpawnActor<AActor>(SpawnTargetMesh.Get(), GetActorLocation(), GetActorRotation(), Pram);
	}
}

void AItemObject::NMC_Start_Implementation() {
	OnStart();
}

void AItemObject::OpenObject() {
	if(!IsStart) return;
	
	YC::Server::FServer::OpenItemObject = true;
	Destroy();
	
	for(const auto Wall : Walls) {
		//Owner = Chr;
		Wall->NMC_UnBlockAll();
	}
	
	MyMesh->Destroy();
}
