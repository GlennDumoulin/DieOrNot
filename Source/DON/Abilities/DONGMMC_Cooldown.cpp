// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Abilities/DONGMMC_Cooldown.h"
#include "DONGameplayAbility.h"
#include "DON/Components/DONAbilitySystemComponent.h"
#include "PlayerAttributeSet.h"

UDONGMMC_Cooldown::UDONGMMC_Cooldown()
{
	CooldownMultiplier.AttributeToCapture = UPlayerAttributeSet::GetCooldownMultiplierAttribute();
	CooldownMultiplier.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	CooldownMultiplier.bSnapshot = false;

	RelevantAttributesToCapture.Add(CooldownMultiplier);
}

float UDONGMMC_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	const UDONGameplayAbility* Ability = Cast<UDONGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}

	float Cooldown = 0.f;
	GetCapturedAttributeMagnitude(CooldownMultiplier, Spec, EvaluationParameters, Cooldown);
	Cooldown = FMath::Max<float>(Cooldown, 1.0f); // Avoid divide by zero

	return Ability->Cooldown * Cooldown;
}

