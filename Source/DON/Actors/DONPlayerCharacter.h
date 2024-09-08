// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "../Components/DONAbilitySystemComponent.h"
#include "../Abilities/PlayerAttributeSet.h"
#include "../Abilities/DONGameplayAbility.h"
#include "DONPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActivePowerupChanged, FGameplayAbilitySpecHandle, NewHandle, FActiveGameplayEffectHandle, EffectHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBulletSpawned, ADONBullet*, Bullet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBounceActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FParryActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHasMultipleActivePowerups);

class ADONBullet;

UCLASS()
class DON_API ADONPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADONPlayerCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void AddStartupGameplayAbilities(bool effectOnly = false);

	UPROPERTY()
	bool bAbilitiesInitialized;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasJumped;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bHasBulletAtStart;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath(ADONPlayerCharacter* DamageInstigator);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerColorChange(FLinearColor newColor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Defaults")
	UDataTable* PlayerDefaults;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Defaults")
	FLinearColor PlayerColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Defaults")
	TSubclassOf<UGameplayEffect> BulletCarryEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Defaults")
	TSubclassOf<UGameplayEffect> PowerupCooldownEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Defaults")
	float RoundStartMoveSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	bool bAlive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDONAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ParryComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UDONGameplayAbility>> GameplayAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UDataTable* PowerupTable;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void NextPowerup();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void PreviousPowerup();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivatePowerup();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* MainInputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* RoundStartInputMapping;

	UFUNCTION(BlueprintCallable, Category = "CharacterMovement")
	void MovePlayerCharacter(const float ActionValueX);

	UFUNCTION(BlueprintCallable, Category = "CharacterMovement")
	bool HandleJumping(const float JumpVel);

	UFUNCTION(BlueprintCallable, Category = "CharacterMovement")
	void SetHasJumped(const bool HasJumped);

	UFUNCTION(BlueprintCallable, Category = "CharacterMovement")
	void HandleBeginFalling(const float CoyoteTimeDuration);

	UFUNCTION(Category = "CharacterMovement")
	void CoyoteTimeElapsed();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void DisableParry();

	UFUNCTION(BlueprintCallable)
	void RefreshHandle();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PossessedBy(AController* InController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetInputMapping(class UInputMappingContext* InputMapping);
	void SetMainInputMapping();
	void SetRoundStartInputMapping();

	UFUNCTION(BlueprintCallable)
	void SetDefaults();

	UFUNCTION(BlueprintCallable)
	bool TryKillPlayer(ADONBullet* killer);

	UFUNCTION(BlueprintImplementableEvent)
		void OnVictory();

	UFUNCTION(BlueprintCallable)
	void Reset() override;

	UFUNCTION(BlueprintCallable)
	void CanShoot();

	void Shoot();

	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColor() { return PlayerColor; }

	void SetPlayerColor(FLinearColor color);

	void RoundEnd();

	void RoundStart(FTransform location);

	UFUNCTION(BlueprintCallable)
	void ClearImmortalityTimer() { GetWorld()->GetTimerManager().ClearTimer(ImmortalityTimerHandle); }

	void BulletDestroyed(ADONBullet* Bullet);

	void ReceivePowerup(FDataTableRowHandle Powerup, bool bIsUnique);

	UFUNCTION(Category = "Abilities")
	void EnableParry();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ADONBullet*> Bullets;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector AimDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector DashDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDashing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float GravityScale;

	UPROPERTY(BlueprintAssignable)
	FActivePowerupChanged OnActivePowerupChanged;

	UPROPERTY(BlueprintAssignable)
	FBulletSpawned OnBulletSpawned;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FBounceActivated OnBounceActivated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FParryActivated OnParryActivated;

	UPROPERTY(BlueprintAssignable)
	FHasMultipleActivePowerups OnHasMultipleActivePowerups;

private:

	bool bShouldMoveToStart;
	bool bRemovingBullets;
	FTransform Target;

	void SpeedChanged(const FOnAttributeChangeData& Data);
	void BulletSpeedChanged(const FOnAttributeChangeData& Data);
	void BulletScaleChanged(const FOnAttributeChangeData& Data);

	int AbilityIndex;

	FTimerHandle ImmortalityTimerHandle;

	UPROPERTY()
	FGameplayAbilitySpecHandle ActivePowerup;

	UPROPERTY()
	UMaterialInstanceDynamic* PlayerBaseMaterial;
};
