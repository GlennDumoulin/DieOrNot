// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DON/Actors/DONBullet.h"
#include "DON/Abilities/DONGameplayAbility.h"
#include "AbilityShoot.generated.h"

UCLASS()
class DON_API UAbilityShoot : public UDONGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UFUNCTION(BlueprintCallable)
	ADONBullet* Shoot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADONBullet> BulletActor;
};
