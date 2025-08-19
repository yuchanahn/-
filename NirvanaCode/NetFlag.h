#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetFlag.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NIRVANA_API UNetFlag : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNetFlag();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "YC_NET|OwnID", Replicated)
	FGuid OwnID;
};
