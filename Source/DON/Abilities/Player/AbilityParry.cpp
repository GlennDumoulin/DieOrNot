// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityParry.h"
#include "DON/Actors/DONPlayerCharacter.h"

void UAbilityParry::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Enable parry component
	auto Player = Cast<ADONPlayerCharacter>(ActorInfo->AvatarActor);
	Player->EnableParry();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	RemoveGrantedByEffect();
	//Player->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(FGameplayTag::RequestGameplayTag(FName("Player.Status.Parry")).GetSingleTagContainer());

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
