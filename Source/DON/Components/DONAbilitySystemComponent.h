// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInput/Public/InputActionValue.h"
#include "DONAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class DON_API UDONAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

private:
	TMap<FGameplayAbilitySpecHandle, uint32> Bindings;

	void AbilityActivationInput(const FInputActionValue& Value, FGameplayAbilitySpecHandle handle);
};
