// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Abilities/Player/AbilityBounce.h"
#include "AbilityBounce.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "DON/Actors/DONBullet.h"

void UAbilityBounce::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
