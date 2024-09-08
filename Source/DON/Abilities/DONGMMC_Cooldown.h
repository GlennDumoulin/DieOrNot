// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DONGMMC_Cooldown.generated.h"

/**
 * 
 */
UCLASS()
class DON_API UDONGMMC_Cooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UDONGMMC_Cooldown();

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const;

private:

	FGameplayEffectAttributeCaptureDefinition CooldownMultiplier;
};
