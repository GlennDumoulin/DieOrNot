// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParryPickup.generated.h"

class USphereComponent;
class UGameplayEffect;
class APickupSpawnIndicator;

UCLASS()
class DON_API AParryPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParryPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereCollider;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> ParryChargeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime;

	void BeginSpawning();

	UFUNCTION(BlueprintNativeEvent)
	void EnablePickup();
	UFUNCTION(BlueprintNativeEvent)
	void DisablePickup();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateProgress(float percentage);

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bHasSpawned;

	float ElapsedSpawnTime;

	float Interval;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadwrite)
	TArray<APickupSpawnIndicator*> Indicators;
};
