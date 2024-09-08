// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Abilities/DONPowerupEffect.h"
#include "PowerupUI.h"

UDONPowerupEffect::UDONPowerupEffect()
{
	OngoingTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Status.PowerupsBlocked")));

	DurationPolicy = EGameplayEffectDurationType::Infinite;
}
