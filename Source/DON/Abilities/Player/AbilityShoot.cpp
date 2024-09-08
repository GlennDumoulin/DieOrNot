// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityShoot.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UAbilityShoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

ADONBullet* UAbilityShoot::Shoot()
{
	
	auto Player = Cast<ADONPlayerCharacter>(GetActorInfo().AvatarActor);

	FVector& AimDirection = Player->AimDirection;

	// Check for very small inputs or possible stick drift
	const bool IsPlayerAiming{ AimDirection.Length() > .1f };

	FRotator PlayerRot = Player->GetActorRotation();
	if (!(FMath::IsNearlyEqual(PlayerRot.Yaw, 90.f, 0.1f) || FMath::IsNearlyEqual(PlayerRot.Yaw, -90.f, 0.1f) || FMath::IsNearlyEqual(PlayerRot.Yaw, 270.f, 0.1f))) {
		UE_LOG(LogTemp, Warning, TEXT("Rotation was: %s"), *PlayerRot.ToString());
		PlayerRot.Yaw = 90.f;
	}
	// Calculate spawn transform
	FRotator SpawnRotation{ (IsPlayerAiming) ? AimDirection.Rotation() : PlayerRot };
	
	FVector SpawnLocation{ Player->GetActorLocation() + SpawnRotation.Vector() * 100.f };

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Player->GetActorLocation(), SpawnLocation, ECC_Visibility)) {
		SpawnLocation = Hit.Location;
	}

	bool Found;
	FVector Scale = FVector(GetActorInfo().AbilitySystemComponent->GetGameplayAttributeValue(UPlayerAttributeSet::GetBulletScaleAttribute(), Found));
	FTransform SpawnTransform{ SpawnRotation, SpawnLocation, Scale };

	// Spawn bullet
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	ADONBullet* Bullet = Cast<ADONBullet>(UGameplayStatics::BeginDeferredActorSpawnFromClass(Player, BulletActor, SpawnTransform));
	if (Bullet) {

		Bullet->PlayerOwner = Player;
		Player->Bullets.Add(Bullet);
		Bullet->FinishSpawning(SpawnTransform);
		Player->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(FGameplayTag::RequestGameplayTag(FName("Player.Status.Bullet")).GetSingleTagContainer());
		Player->OnBulletSpawned.Broadcast(Bullet);
	}

	return Bullet;
}
