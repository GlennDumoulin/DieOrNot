// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletPredicter.generated.h"

class ADONBullet;
class ABulletPredictionMarker;

UCLASS()
class DON_API ABulletPredicter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletPredicter();

	void RegisterBullet(ADONBullet* Bullet);
	void DeregisterBullet(ADONBullet* Bullet);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<ADONBullet*> Bullets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<ABulletPredictionMarker*> Markers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Defaults")
	TSubclassOf<ABulletPredictionMarker> MarkerClass;

	int UsedMarkers;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
