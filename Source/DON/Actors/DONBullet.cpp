// Fill out your copyright notice in the Description page of Project Settings.


#include "DONBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "BulletPredicter.h"

// Sets default values
ADONBullet::ADONBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	Radius = 25.f;
	Multiplier = 1.f;

	RedirectionCooldown = 0.1f;
	bCanRedirect = true;

	SetActorEnableCollision(false);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere collider"));
	RootComponent = SphereCollider;
	SphereCollider->InitSphereRadius(Radius);
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ADONBullet::OnOverlap);
	SphereCollider->SetNotifyRigidBodyCollision(true);
	SphereCollider->OnComponentHit.AddDynamic(this, &ADONBullet::OnCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if (ProjectileMovement)
	{
		Speed = 1000.f;
		MaxSpeed = 2000.f;
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->ProjectileGravityScale = 0.f;
		ProjectileMovement->bShouldBounce = true;
		ProjectileMovement->Bounciness = 1.f;
		ProjectileMovement->Friction = 0.f;
		ProjectileMovement->SetPlaneConstraintNormal(FVector::ForwardVector);
		ProjectileMovement->bConstrainToPlane = true;
		ProjectileMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
		ProjectileMovement->UpdatedComponent = RootComponent;
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	PredictionRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Prediction"));
	PredictionRoot->SetupAttachment(RootComponent);

	CollisionPredictionRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionPrediction"));
	CollisionPredictionRoot->SetupAttachment(RootComponent);
}

void ADONBullet::SetSpeedMultiplier(float value)
{
	Multiplier = value;
}

void ADONBullet::DestroyBullet()
{
	if (PlayerOwner) {
		PlayerOwner->BulletDestroyed(this);
		PlayerOwner->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(FGameplayTag::RequestGameplayTag(FName("Player.Status.PerfectBounce")).GetSingleTagContainer());
	}
	auto Predicter = Cast<ABulletPredicter>(UGameplayStatics::GetActorOfClass(this, ABulletPredicter::StaticClass()));
	if (Predicter) {
		Predicter->RegisterBullet(this);
	}

	OnDestroyBullet();

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		Destroy();
	}, 5.f, false);
}

// Called when the game starts or when spawned
void ADONBullet::BeginPlay()
{
	Age = 0.f;
	Super::BeginPlay();

	auto Predicter = Cast<ABulletPredicter>(UGameplayStatics::GetActorOfClass(this, ABulletPredicter::StaticClass()));
	if (Predicter) {
		Predicter->RegisterBullet(this);
	}

	SphereCollider->SetSphereRadius(Radius);
	if (PlayerOwner) {
		auto Color = PlayerOwner->GetPlayerColor();

		bool bFound;
		Multiplier = PlayerOwner->GetAbilitySystemComponent()->GetGameplayAttributeValue(UPlayerAttributeSet::GetBulletSpeedAttribute(), bFound);
		ProjectileMovement->SetVelocityInLocalSpace({ Speed * Multiplier, 0.0, 0.0 });

		auto BaseMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
		Mesh->SetMaterial(0, BaseMaterial);

		BaseMaterial->SetVectorParameterValue("Color", Color);
	}
}

FVector ADONBullet::GetRotation(const FVector& Direction, const FVector& Normal)
{
	if (NewDirection.SquaredLength() > 0.5f)
	{
		float Angle = FMath::Atan2(Normal.Y * NewDirection.Z - Normal.Z * NewDirection.Y, Normal.Dot(NewDirection));
		Angle = FMath::RadiansToDegrees(Angle);

		//Ignore very small changes
		if (Angle > 150.f && Angle < 210.f)
		{
			return FMath::GetReflectionVector(Direction, Normal);
		}

		float OutAngle = FMath::Clamp(Angle > 180.f ? Angle - 360.f : Angle, -80.f, 80.f);

		return Normal.RotateAngleAxis(OutAngle, { 1, 0, 0 });
	}
	return FMath::GetReflectionVector(Direction, Normal);
}

void ADONBullet::Destroyed()
{
	Super::Destroyed();
}

void ADONBullet::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<ADONPlayerCharacter>(OtherActor);
	if (Player) {
		if (Player->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Status.AutoParry")))) {
			if (Player->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTag::RequestGameplayTag(FName("Ability.Player.Parry")).GetSingleTagContainer()))
				return;
		}
		if (Player->TryKillPlayer(this)) {
			DestroyBullet();
		}
	}
}

void ADONBullet::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bCanRedirect)
	{
		//Handle collision redirection
		FVector ReflectedDirection = GetRotation(GetVelocity(), Hit.ImpactNormal);
		SetActorRotation(ReflectedDirection.ToOrientationRotator());
		ProjectileMovement->SetVelocityInLocalSpace({ Speed, 0, 0 });

		//Handle redirection cooldown
		bCanRedirect = false;

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle,
			[this]() { bCanRedirect = true; },
			RedirectionCooldown,
			false
		);
	}

	PlayerOwner->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(FGameplayTag::RequestGameplayTag(FName("Player.Status.PerfectBounce")).GetSingleTagContainer());

	auto Context = PlayerOwner->GetAbilitySystemComponent()->MakeEffectContext();
	PlayerOwner->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(BounceEffect->GetDefaultObject<UGameplayEffect>(), 1.0, Context);

	static auto PerfectTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Bullet.PerfectBounce"));
	static auto RegularTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Bullet.RegularBounce"));
	static auto PerfectTest = FGameplayTag::RequestGameplayTag(FName("Player.Status.PerfectBounce"));
	FGameplayCueParameters Params;
	Params.Location = Hit.Location;
	Params.Normal = Hit.Normal;
	Params.SourceObject = this;
	PlayerOwner->GetAbilitySystemComponent()->ExecuteGameplayCue(RegularTag, Params);

}

// Called every frame
void ADONBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update bullet speed
	Age += DeltaTime;
	Speed = 0.01 * pow(Age, 3) + 1000.f;
	Speed = FMath::Clamp(Speed, 1000.f, MaxSpeed);
	Speed *= Multiplier;
	ProjectileMovement->SetVelocityInLocalSpace({ Speed, 0, 0 });

	//Update aiming bullet redirecting
	NewDirection = PlayerOwner->AimDirection;

	FHitResult out;
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(),
		GetActorLocation() + GetVelocity().GetSafeNormal() * 3000.f,
		Radius * GetActorScale3D().Z, {EObjectTypeQuery::ObjectTypeQuery1}, false, {}, EDrawDebugTrace::None, out, true))
	{
		PredictionRoot->SetWorldLocation(out.Location);

		auto Direction = GetRotation(GetVelocity(), out.ImpactNormal);
		PredictionRoot->SetWorldRotation(Direction.ToOrientationRotator());
		PredictionRoot->SetHiddenInGame(false, true);
	}
	else {
		PredictionRoot->SetHiddenInGame(true, true);
	}
}

