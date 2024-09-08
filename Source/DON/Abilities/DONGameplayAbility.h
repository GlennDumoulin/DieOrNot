// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DONGameplayAbility.generated.h"

class UInputAction;

UCLASS()
class DON_API UDONGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UDONGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	bool bActivateAbilityOnGranted;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	UInputAction* Input;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilities")
	float Cooldown;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	FGameplayTagContainer CooldownTags;

	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	UFUNCTION(BlueprintCallable, Category = Ability)
	FGameplayTagContainer GetAllCooldownTags() const;

};
