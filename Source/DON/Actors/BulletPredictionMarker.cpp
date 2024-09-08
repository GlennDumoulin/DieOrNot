// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Actors/BulletPredictionMarker.h"

// Sets default values
ABulletPredictionMarker::ABulletPredictionMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABulletPredictionMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletPredictionMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

