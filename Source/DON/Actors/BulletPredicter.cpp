// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Actors/BulletPredicter.h"
#include "BulletPredictionMarker.h"
#include "DONBullet.h"

// Sets default values
ABulletPredicter::ABulletPredicter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABulletPredicter::RegisterBullet(ADONBullet* Bullet)
{
	Bullets.Add(Bullet);
}

void ABulletPredicter::DeregisterBullet(ADONBullet* Bullet)
{
	Bullets.Remove(Bullet);
}

// Called when the game starts or when spawned
void ABulletPredicter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletPredicter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UsedMarkers = 0;
	for (auto& m : Markers) {
		m->SetActorLocation({ 0.f, 0.f, -1000.f });
	}

	for (int i = 0; i < Bullets.Num(); ++i) {
		for (int j = i; j < Bullets.Num(); ++j) {
			auto& b1 = Bullets[i];
			auto& b2 = Bullets[j];

			auto RelLoc = b1->GetActorLocation() - b2->GetActorLocation();
			auto Distance = b1->GetRadius() + b2->GetRadius();
			Distance *= Distance;

			if (RelLoc.SquaredLength() <= Distance) {
				continue;
			}

			auto RelVel = b1->GetVelocity() - b2->GetVelocity();

			auto SquaredRelVel = RelVel.SquaredLength();
			auto RelDot = RelVel.Dot(RelLoc);

			if (RelDot > 0.f || FMath::IsNearlyZero(SquaredRelVel)) {
				continue;
			}

			auto Value = (-1 * RelDot) / SquaredRelVel;
			auto DistCheck = (RelVel * Value + RelLoc).SquaredLength();

			if (DistCheck > Distance) {
				continue;
			}

			auto FinalPos = b1->GetActorLocation() + b1->GetVelocity() * (Value - sqrt((Distance - DistCheck) / SquaredRelVel));

			if (Markers.IsValidIndex(UsedMarkers)) {
				Markers[UsedMarkers]->SetActorLocation(FinalPos);
			}
			else {
				auto Marker = GetWorld()->SpawnActor<ABulletPredictionMarker>(MarkerClass);
				Markers.Add(Marker);
				Marker->SetActorLocation(FinalPos);
			}
			UsedMarkers++;
		}
	}
}

