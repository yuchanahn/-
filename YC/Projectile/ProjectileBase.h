// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class NIRVANA_API AProjectileBase : public AActor {
	GENERATED_BODY()

public:
	AProjectileBase();
	UPROPERTY(EditAnywhere, Category = "Projectile|Mesh")
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Comp")
	class UProjectileMovementComponent* ProjectileMovementComp;
	
	UPROPERTY(EditAnywhere, Category = "Projectile|Data")
	float Speed;
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetDir(const FVector& InDir) const;
	virtual void Tick(float DeltaTime) override;
};
