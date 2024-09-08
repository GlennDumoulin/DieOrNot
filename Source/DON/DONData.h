#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DONData.generated.h"

class UDONPowerupEffect;

USTRUCT(BlueprintType)
struct FPlayerData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CooldownMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BulletSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BulletScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DashDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DashDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ReactionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float JumpPower;
};

USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> Map;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> Challenges;
};

USTRUCT(BlueprintType)
struct FPowerupData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Tooltip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDONPowerupEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPassive{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = "0", ClampMax = "100"))
	int Chance{100};
};