// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DON/Abilities/DONGameplayAbility.h"
#include "AbilityBounce.generated.h"

class ADONPlayerCharacter;

UCLASS()
class DON_API UAbilityBounce : public UDONGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpeedMultiplier;

};
