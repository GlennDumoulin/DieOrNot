// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DON_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPlayerAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Atrribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Speed
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed, Category = "Attributes")
		FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Speed);

	UFUNCTION()
		virtual void OnRep_Speed(const FGameplayAttributeData& old);

	// CooldownMultiplier
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CooldownMultiplier, Category = "Attributes")
		FGameplayAttributeData CooldownMultiplier;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CooldownMultiplier);

	UFUNCTION()
		virtual void OnRep_CooldownMultiplier(const FGameplayAttributeData& old);

	// Bullet speed
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BulletSpeed, Category = "Attributes")
		FGameplayAttributeData BulletSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, BulletSpeed);

	UFUNCTION()
		virtual void OnRep_BulletSpeed(const FGameplayAttributeData& old);

	// Bullet scale
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BulletScale, Category = "Attributes")
		FGameplayAttributeData BulletScale;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, BulletScale);

	UFUNCTION()
		virtual void OnRep_BulletScale(const FGameplayAttributeData& old);

	// Dash distance
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DashDistance, Category = "Attributes")
		FGameplayAttributeData DashDistance;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, DashDistance);

	UFUNCTION()
		virtual void OnRep_DashDistance(const FGameplayAttributeData& old);


	// Dash duration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DashDuration, Category = "Attributes")
		FGameplayAttributeData DashDuration;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, DashDuration);

	UFUNCTION()
		virtual void OnRep_DashDuration(const FGameplayAttributeData& old);


	// Reaction windows
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ReactionTime, Category = "Attributes")
		FGameplayAttributeData ReactionTime;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ReactionTime);

	UFUNCTION()
		virtual void OnRep_ReactionTime(const FGameplayAttributeData& old);


	// Jump power
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_JumpPower, Category = "Attributes")
		FGameplayAttributeData JumpPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, JumpPower);

	UFUNCTION()
		virtual void OnRep_JumpPower(const FGameplayAttributeData& old);


	void SetDefaults(UDataTable* Table, const FName& row);
	//	
};
