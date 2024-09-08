// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Actors/PickupSpawnIndicator.h"

// Sets default values
APickupSpawnIndicator::APickupSpawnIndicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APickupSpawnIndicator::UpdateVisualization_Implementation(float percentage)
{
}

// Called when the game starts or when spawned
void APickupSpawnIndicator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupSpawnIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

