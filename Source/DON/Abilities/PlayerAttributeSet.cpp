// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "../DONData.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, BulletSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, BulletScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ReactionTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, DashDistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, JumpPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, CooldownMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, DashDuration, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Atrribute, float& NewValue)
{
}

void UPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetSpeedAttribute()) {
		//GetOwningActor()->GetComponentByClass<UMovementComponent>()
	}
}

bool UPlayerAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return true;
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	
}

void UPlayerAttributeSet::OnRep_Speed(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Speed, old);
}

void UPlayerAttributeSet::OnRep_CooldownMultiplier(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, CooldownMultiplier, old);
}

void UPlayerAttributeSet::OnRep_BulletSpeed(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, BulletSpeed, old);
}

void UPlayerAttributeSet::OnRep_BulletScale(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, BulletScale, old);
}

void UPlayerAttributeSet::OnRep_DashDistance(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, DashDistance, old);
}

void UPlayerAttributeSet::OnRep_DashDuration(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, DashDuration, old);
}

void UPlayerAttributeSet::OnRep_ReactionTime(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ReactionTime, old);
}

void UPlayerAttributeSet::OnRep_JumpPower(const FGameplayAttributeData& old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, JumpPower, old);
}

void UPlayerAttributeSet::SetDefaults(UDataTable* Table, const FName& row)
{
	if (!Table) return;
	FString context("Set player defaults");

	auto Data = Table->FindRow<FPlayerData>(row, context);

	SetSpeed(Data->Speed);
	SetCooldownMultiplier(Data->CooldownMultiplier);
	SetBulletSpeed(Data->BulletSpeed);
	SetBulletScale(Data->BulletScale);
	SetDashDistance(Data->DashDistance);
	SetDashDuration(Data->DashDuration);
	SetReactionTime(Data->ReactionTime);
	SetJumpPower(Data->JumpPower);
}
