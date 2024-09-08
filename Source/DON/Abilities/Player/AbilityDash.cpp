// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityDash.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "DON/Abilities/PlayerAttributeSet.h"

void UAbilityDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	auto Player = Cast<ADONPlayerCharacter>(ActorInfo->AvatarActor);
	auto CharacterMovement = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent);
	auto ASC = ActorInfo->AbilitySystemComponent;

	// Get Dash distance
	bool bFoundAttr = false;
	float DashDistance = ASC->GetGameplayAttributeValue(UPlayerAttributeSet::GetDashDistanceAttribute(), bFoundAttr);
	if (!bFoundAttr) DashDistance = 2000.f;

	FVector& DashDirection = Player->DashDirection;

	// Check for very small inputs or possible stick drift
	const bool IsPlayerAiming{ DashDirection.Length() > .1f };

	// Calculate dash velocity
	DashDirection.Normalize();
	FVector DashVelocity = (IsPlayerAiming) ? DashDirection * DashDistance : FVector(0.f, 0.f, DashDistance);

	// Set dash settings
	CharacterMovement->GravityScale = 1.f;
	CharacterMovement->MaxWalkSpeed = 9999.f;
	CharacterMovement->Velocity = DashVelocity;
	CharacterMovement->UpdateComponentVelocity();

	// Enable is dashing
	Player->bIsDashing = true;

	//// Handle dash duration
	//float DashDuration = ASC->GetGameplayAttributeValue(UPlayerAttributeSet::GetDashDurationAttribute(), bFoundAttr);
	//if (!bFoundAttr) DashDuration = .2f;

	//FTimerHandle UnusedHandle;
	//FTimerDelegate TimerElapsedDelegate{ FTimerDelegate::CreateUObject(Player, &ADONPlayerCharacter::DashDurationElapsed) };
	//Player->GetWorldTimerManager().SetTimer(UnusedHandle, TimerElapsedDelegate, DashDuration, false);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Remove ability to dash
	ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.CanDash")));

	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UAbilityDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	auto Player = Cast<ADONPlayerCharacter>(ActorInfo->AvatarActor);
	bool bFoundAttr = false;
	float PlayerSpeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(UPlayerAttributeSet::GetSpeedAttribute(), bFoundAttr);

	// Reset dash settings
	Player->GetCharacterMovement()->GravityScale = Player->GravityScale;
	Player->GetCharacterMovement()->MaxWalkSpeed = PlayerSpeed * 1.75f;

	// Disable is dashing
	Player->bIsDashing = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}