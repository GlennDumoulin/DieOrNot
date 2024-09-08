// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Actors/LevelDanger.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemInterface.h"
#include "DON/Components/DONAbilitySystemComponent.h"

// Sets default values
ALevelDanger::ALevelDanger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collider"));
	RootComponent = BoxCollider;
	BoxCollider->SetBoxExtent({100, 100, 100});
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ALevelDanger::OnOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ALevelDanger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelDanger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ApplyEffect) return;
	if (auto it = Cast<IAbilitySystemInterface>(OtherActor)) {
		auto context = it->GetAbilitySystemComponent()->MakeEffectContext();
		it->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(ApplyEffect.GetDefaultObject(), 1.f, context);
	}
}

// Called every frame
void ALevelDanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

