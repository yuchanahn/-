// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActionType.generated.h"

USTRUCT(BlueprintType)
struct FRangeFrame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(AllowPrivateAccess = true))
	float StartApplyFrame;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(AllowPrivateAccess = true))
	float EndApplyFrame;

	bool operator == (const FRangeFrame& Other) const
	{
		return StartApplyFrame == Other.StartApplyFrame && EndApplyFrame == Other.EndApplyFrame;
	}
};

UENUM(BlueprintType)
enum class EWarpingDir : uint8 {None = 0, Backward = 1, Forward = 2};

USTRUCT(BlueprintType)
struct FMotionWarpingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(AllowPrivateAccess = true))
	float StartWarpApplyFrame;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(AllowPrivateAccess = true))
	float EndWarpApplyFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(AllowPrivateAccess = true))
	float WarpDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta=(AllowPrivateAccess = true))
	float WarpSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	EWarpingDir WarpingDir = EWarpingDir::Forward;
};

/**
 * 
 */
UCLASS()
class NIRVANA_API UActionType : public UObject {
	GENERATED_BODY()
};
