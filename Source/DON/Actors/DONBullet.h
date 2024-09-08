// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DONBullet.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class ADONPlayerCharacter;
class UGameplayEffect;

UCLASS()
class DON_API ADONBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADONBullet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ADONPlayerCharacter* PlayerOwner;

	float GetSpeed() const { return Speed; }

	float GetRadius() const { return Radius; }
	void SetSpeedMultiplier(float value);

	UFUNCTION(BlueprintCallable)
	void DestroyBullet();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroyBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereCollider;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> PredictionRoot;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> CollisionPredictionRoot;



	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Defaults")
	TSubclassOf<UGameplayEffect> BounceEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Defaults")
	float Radius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Defaults")
	FVector NewDirection;

	UFUNCTION(BlueprintPure)
	FVector GetRotation(const FVector& Direction, const FVector& Normal);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxSpeed;

	float Multiplier;

private:
	virtual void Destroyed() override;

	float Age;

	float RedirectionCooldown;
	bool bCanRedirect;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
