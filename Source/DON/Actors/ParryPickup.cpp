// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Actors/ParryPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "DON/Actors/PickupSpawnIndicator.h"
#include "GameplayEffect.h"

AParryPickup::AParryPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere collider"));
	RootComponent = SphereCollider;
	SphereCollider->InitSphereRadius(50.f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	bHasSpawned = true;
}

void AParryPickup::BeginPlay()
{
	SphereCollider->OnComponentBeginOverlap.Clear();
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AParryPickup::OnOverlap);
	Super::BeginPlay();
	Interval = SpawnTime;
	if (Indicators.Num()) Interval = SpawnTime / Indicators.Num();
	DisablePickup();
	BeginSpawning();
}

void AParryPickup::BeginSpawning()
{
	bHasSpawned = false;
	ElapsedSpawnTime = 0.f;
}

void AParryPickup::EnablePickup_Implementation()
{
	bHasSpawned = true;
	Mesh->SetHiddenInGame(false, true);
	SetActorEnableCollision(true);
}

void AParryPickup::DisablePickup_Implementation()
{
	Mesh->SetHiddenInGame(true, true);
	SetActorEnableCollision(false);
	for (auto& i : Indicators) {
		i->UpdateVisualization(0.0);
	}
}

void AParryPickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bHasSpawned) return;
	if (auto Player = Cast<ADONPlayerCharacter>(OtherActor); Player != nullptr) {
		if (ParryChargeEffect->IsValidLowLevel()) {
			auto Context = Player->GetAbilitySystemComponent()->MakeEffectContext();
			Player->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(ParryChargeEffect->GetDefaultObject<UGameplayEffect>(), 1.f, Context);
		}
		DisablePickup();
		BeginSpawning();
	}
}

void AParryPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasSpawned && !Indicators.IsEmpty() && Interval > 0.f) {
		ElapsedSpawnTime += DeltaTime;

		if (ElapsedSpawnTime >= SpawnTime) {
			EnablePickup();
		}

		float d = ElapsedSpawnTime / Interval;
		long index = long(d);
		float percentage = fmod(ElapsedSpawnTime, Interval) / Interval;
		if (index < Indicators.Num() && Indicators[index]) Indicators[index]->UpdateVisualization(percentage);

		UpdateProgress(ElapsedSpawnTime / SpawnTime);
	}
}

