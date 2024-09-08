// Fill out your copyright notice in the Description page of Project Settings.


#include "DONAbilitySystemComponent.h"
#include "DON/Abilities/DONGameplayAbility.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "EnhancedInput/Public/InputAction.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"

void UDONAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	auto Ability = Cast<UDONGameplayAbility>(AbilitySpec.Ability);

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(Cast<APlayerController>(Cast<ADONPlayerCharacter>(GetOwner())->GetController())->InputComponent);
	auto& Bind = PEI->BindAction(Ability->Input, ETriggerEvent::Triggered, this, &UDONAbilitySystemComponent::AbilityActivationInput, AbilitySpec.Handle);

	Bindings.Emplace(AbilitySpec.Handle, Bind.GetHandle());
}

void UDONAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	auto Controller = Cast<ADONPlayerCharacter>(GetOwner())->GetController();
	if (!Controller) return;
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(Cast<APlayerController>(Controller)->InputComponent);
	PEI->RemoveActionBinding(*Bindings.Find(AbilitySpec.Handle));
	UE_LOG(LogTemp, Display, TEXT("Ability binding removed"));
	Super::OnRemoveAbility(AbilitySpec);
}

void UDONAbilitySystemComponent::AbilityActivationInput(const FInputActionValue& Value, FGameplayAbilitySpecHandle handle)
{
	TryActivateAbility(handle);
}
