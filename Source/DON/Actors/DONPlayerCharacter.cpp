// Fill out your copyright notice in the Description page of Project Settings.


#include "DONPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "DON/DONGameModeBase.h"
#include "DON/Gameplay/DONGameInstance.h"
#include "DON/Actors/DONBullet.h"
#include "DON/Abilities/DONPowerupEffect.h"
#include "DON/Gameplay/DONPlayerState.h"
#include "DON/DONData.h"
#include "DON/Abilities/PowerupUI.h"

// Sets default values
ADONPlayerCharacter::ADONPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bAbilitiesInitialized = false;
	bAlive = true;
	bShouldMoveToStart = false;
	bRemovingBullets = false;
	GravityScale = 3.8f;
	AbilityIndex = 0;
	bHasBulletAtStart = true;

	AbilitySystemComponent = CreateDefaultSubobject<UDONAbilitySystemComponent>(TEXT("Ability System"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("Attributes"));
	// Add callback listeners for all attributes here
	if (Attributes) {
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetSpeedAttribute()).AddUObject(this, &ADONPlayerCharacter::SpeedChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetBulletSpeedAttribute()).AddUObject(this, &ADONPlayerCharacter::BulletSpeedChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetBulletScaleAttribute()).AddUObject(this, &ADONPlayerCharacter::BulletScaleChanged);
	}

	GetCharacterMovement()->SetWalkableFloorAngle(60.f);

	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ADONPlayerCharacter::BeginPlay()
{
	SetDefaults();

	PlayerBaseMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	GetMesh()->SetMaterial(0, PlayerBaseMaterial);

	if (GetController()) {
		auto Color = GetGameInstance<UDONGameInstance>()->GetPlayerColor(Cast<APlayerController>(GetController())->GetPlatformUserId());
		SetPlayerColor(Color);
	}
	Super::BeginPlay();

}

UAbilitySystemComponent* ADONPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADONPlayerCharacter::AddStartupGameplayAbilities(bool effectOnly)
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && (!bAbilitiesInitialized || effectOnly)) {
		if (!effectOnly)
			for (auto& ability : GameplayAbilities) {
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
					ability, 1,
					-1,
					this
				));
			}
		for (auto& effect : DefaultEffects) {
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
				effect, 1, EffectContext);

			if (NewHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveEffectHandle =
					AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}

}

void ADONPlayerCharacter::NextPowerup()
{
	static FGameplayTag powerupTag = FGameplayTag::RequestGameplayTag(FName("Ability.Powerup"));
	TArray<FGameplayAbilitySpecHandle> Array;
	AbilitySystemComponent->FindAllAbilitiesWithTags(Array, powerupTag.GetSingleTagContainer(), false);

	if (Array.Num() < 2) return;

	AbilityIndex++;
	if (AbilityIndex >= Array.Num()) {
		AbilityIndex = 0;
	}
	ActivePowerup = Array[AbilityIndex];
	OnActivePowerupChanged.Broadcast(ActivePowerup, AbilitySystemComponent->FindAbilitySpecFromHandle(ActivePowerup)->GameplayEffectHandle);
}

void ADONPlayerCharacter::PreviousPowerup()
{
	static FGameplayTag powerupTag = FGameplayTag::RequestGameplayTag(FName("Ability.Powerup"));
	TArray<FGameplayAbilitySpecHandle> Array;
	AbilitySystemComponent->FindAllAbilitiesWithTags(Array, powerupTag.GetSingleTagContainer(), false);

	if (Array.Num() < 2) return;

	AbilityIndex--;
	if (AbilityIndex < 0) {
		AbilityIndex = Array.Num() - 1;
	}
	ActivePowerup = Array[AbilityIndex];
	OnActivePowerupChanged.Broadcast(ActivePowerup, AbilitySystemComponent->FindAbilitySpecFromHandle(ActivePowerup)->GameplayEffectHandle);
}

void ADONPlayerCharacter::ActivatePowerup()
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.HasBullet")))) return;

	if (AbilitySystemComponent->TryActivateAbility(ActivePowerup)) {
		FGameplayCueParameters Params;
		AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Powerup")), Params);
	}
}

void ADONPlayerCharacter::MovePlayerCharacter(const float ActionValueX)
{
	// Ignore very small inputs or possible stick drift
	if (abs(ActionValueX) <= .1f) return;

	// Get right vector
	FRotator ControlRotation{ GetControlRotation() };
	ControlRotation.Pitch = 0;

	const FRotationMatrix ControlRotationMatrix{ ControlRotation };
	const FVector RightVector{ 0, 1, 0 };

	// Add movement input
	AddMovementInput(RightVector, ActionValueX);
}

bool ADONPlayerCharacter::HandleJumping(const float JumpVel)
{
	// Ignore if player jumped already
	if (bHasJumped) return false;
	
	// Launch the player
	FVector LaunchVel{ 0.f, 0.f, JumpVel };
	LaunchCharacter(LaunchVel, false, false);

	// Set Gravity Scale
	GetCharacterMovement()->GravityScale = GravityScale;

	return true;
}

void ADONPlayerCharacter::SetHasJumped(const bool HasJumped)
{
	bHasJumped = HasJumped;

	if (bHasJumped)
	{
		// Add CanDash tag
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.CanDash")));
	}
	else
	{
		// Remove CanDash tag
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.CanDash")));
	}
}

void ADONPlayerCharacter::HandleBeginFalling(const float CoyoteTimeDuration)
{
	// Ignore if player already jumped
	if (bHasJumped) return;

	// Disable Gravity Scale
	GetCharacterMovement()->GravityScale = 1.0;

	// Handle Coyote Time
	FTimerHandle UnusedHandle;
	FTimerDelegate TimerElapsedDelegate{ FTimerDelegate::CreateUObject(this, &ADONPlayerCharacter::CoyoteTimeElapsed) };
	GetWorldTimerManager().SetTimer(UnusedHandle, TimerElapsedDelegate, CoyoteTimeDuration, false);
}

void ADONPlayerCharacter::CoyoteTimeElapsed()
{
	// Set Gravity Scale
	GetCharacterMovement()->GravityScale = GravityScale;

	// Make sure player can't jump anymore
	SetHasJumped(true);
}

void ADONPlayerCharacter::EnableParry()
{
	if (ParryComponent)
	{
		// Enable parry component
		ParryComponent->SetHiddenInGame(false);
		ParryComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		// Handle disabling parry
		bool bFoundAttr = false;
		float ReactionTime = AbilitySystemComponent->GetGameplayAttributeValue(UPlayerAttributeSet::GetReactionTimeAttribute(), bFoundAttr);

		FTimerHandle UnusedHandle;
		FTimerDelegate TimerElapsedDelegate{ FTimerDelegate::CreateUObject(this, &ADONPlayerCharacter::DisableParry) };
		GetWorldTimerManager().SetTimer(UnusedHandle, TimerElapsedDelegate, ReactionTime, false);
	}
}

void ADONPlayerCharacter::DisableParry()
{
	if (ParryComponent)
	{
		// Disable parry component
		ParryComponent->SetHiddenInGame(true);
		ParryComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called every frame
void ADONPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (bShouldMoveToStart) {
		SetActorLocation(FMath::interp(GetActorLocation(), Target, DeltaTime, RoundStartMoveSpeed));

		if ((GetActorLocation() - Target).SquaredLength() < 10.f) {
			bShouldMoveToStart = false;
			Reset();
		}
	}*/
}

void ADONPlayerCharacter::PossessedBy(AController* InController)
{
	Super::PossessedBy(InController);

	AddStartupGameplayAbilities();
}

// Called to bind functionality to input
void ADONPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetMainInputMapping();
}

void ADONPlayerCharacter::SetInputMapping(class UInputMappingContext* InputMapping)
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);
}

void ADONPlayerCharacter::SetMainInputMapping()
{
	SetInputMapping(MainInputMapping);
}

void ADONPlayerCharacter::SetRoundStartInputMapping()
{
	SetInputMapping(RoundStartInputMapping);
}

void ADONPlayerCharacter::SetDefaults()
{
	if (Attributes) {
		Attributes->SetDefaults(PlayerDefaults, "Default");
	}
}

bool ADONPlayerCharacter::TryKillPlayer(ADONBullet* killer)
{
	static FGameplayTag dead = FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead"));
	static FGameplayTag immortal = FGameplayTag::RequestGameplayTag(FName("Player.Status.Immortal"));
	static FGameplayTag deadEffect = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Player.Death"));
	static FGameplayTag selfImmortal = FGameplayTag::RequestGameplayTag(FName("Player.Status.ImmuneToSelf"));

	FGameplayTagContainer filter;
	filter.AddTagFast(dead);
	filter.AddTagFast(immortal);
	if (AbilitySystemComponent->HasAnyMatchingGameplayTags(filter)) {
		return false;
	}

	if (killer)
		if (killer->PlayerOwner == this && AbilitySystemComponent->HasMatchingGameplayTag(selfImmortal)) {
			return false;
		}

	AbilitySystemComponent->AddLooseGameplayTag(dead);
	bAlive = false;

	FGameplayCueParameters Params;
	Params.EffectCauser = killer;
	Params.Location = killer ? killer->GetActorLocation() : GetActorLocation();
	AbilitySystemComponent->ExecuteGameplayCue(deadEffect, Params);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetMovementComponent()->Deactivate(); 

	if (auto Gamemode = Cast<ADONGameModeBase>(UGameplayStatics::GetGameMode(this)); Gamemode != nullptr) {
		SetRoundStartInputMapping();
		Gamemode->RegisterPlayerDeath(this, killer ? killer->PlayerOwner : nullptr);
	}
	OnDeath(killer ? killer->PlayerOwner : nullptr);

	return true;
}

void ADONPlayerCharacter::Reset()
{
	static FGameplayTag dead = FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead"));
	static FGameplayTag hasBullet = FGameplayTag::RequestGameplayTag(FName("Player.HasBullet"));
	static FGameplayTag canDash = FGameplayTag::RequestGameplayTag(FName("Player.CanDash"));
	static FGameplayTag cooldown = FGameplayTag::RequestGameplayTag(FName("Cooldown"));

	SetDefaults();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	GetMovementComponent()->Activate();
	GetCharacterMovement()->GravityScale = GravityScale;
	SetMainInputMapping();
	DisableParry();

	GetController<APlayerController>()->FlushPressedKeys();

	bHasJumped = false;
	bAlive = true;

	AimDirection = FVector{ 0.f, 0.f, 0.f };
	DashDirection = FVector{ 0.f, 0.f, 0.f };

	AbilitySystemComponent->RemoveLooseGameplayTag(dead);
	AbilitySystemComponent->RemoveLooseGameplayTag(canDash);

	AbilitySystemComponent->RemoveAllGameplayCues();

	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(cooldown.GetSingleTagContainer());
	AddStartupGameplayAbilities(true);

	bRemovingBullets = true;
	for (auto& b : Bullets) {
		b->DestroyBullet();
	}
	Bullets.Empty();
	bRemovingBullets = false;
	if (bHasBulletAtStart && BulletCarryEffect) AbilitySystemComponent->ApplyGameplayEffectToSelf(BulletCarryEffect->GetDefaultObject<UGameplayEffect>(), 1.f, AbilitySystemComponent->MakeEffectContext());
	if (PowerupCooldownEffect) AbilitySystemComponent->ApplyGameplayEffectToSelf(PowerupCooldownEffect->GetDefaultObject<UGameplayEffect>(), 1.f, AbilitySystemComponent->MakeEffectContext());

	//TArray<FGameplayAbilitySpecHandle> Specs;
	//AbilitySystemComponent->GetAllAbilities(Specs);
	//for (auto& s : Specs) {
	//	AbilitySystemComponent->FindAbilitySpecFromHandle(s)->Ability->CommitAbilityCooldown(s, AbilitySystemComponent->AbilityActorInfo.Get(), {}, true);
	//}
}

void ADONPlayerCharacter::CanShoot()
{
	AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.HasBullet")));
}

void ADONPlayerCharacter::Shoot()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTag::RequestGameplayTag(FName("Ability.Player.Shoot")).GetSingleTagContainer());
}

void ADONPlayerCharacter::SetPlayerColor(FLinearColor color)
{
	PlayerColor = color;
	PlayerBaseMaterial->SetVectorParameterValue("Color", PlayerColor);
	OnPlayerColorChange(PlayerColor);
}

void ADONPlayerCharacter::RoundEnd()
{
	static FGameplayTag immortal = FGameplayTag::RequestGameplayTag(FName("Player.Status.Immortal"));
	GetCharacterMovement()->GravityScale = 0.f;
	DisableInput(Cast<APlayerController>(GetController()));
	AbilitySystemComponent->AddLooseGameplayTag(immortal);

	GetCharacterMovement()->StopMovementImmediately();

	bRemovingBullets = true;
	for (auto& b : Bullets) {
		b->DestroyBullet();
	}
	Bullets.Empty();
	bRemovingBullets = false;
}

void ADONPlayerCharacter::RoundStart(FTransform location)
{
	static FGameplayTag immortal = FGameplayTag::RequestGameplayTag(FName("Player.Status.Immortal"));
	GetWorld()->GetTimerManager().SetTimer(ImmortalityTimerHandle, [this]() {
		if (AbilitySystemComponent) AbilitySystemComponent->RemoveLooseGameplayTag(immortal);
	}, 2.f, false);
	
	EnableInput(Cast<APlayerController>(GetController()));
	GetCharacterMovement()->GravityScale = 0.f;
	//bShouldMoveToStart = true;
	location.SetScale3D({1.f, 1.f, 1.f});
	Target = location;
	SetActorTransform(Target);
	Reset();
}

void ADONPlayerCharacter::BulletDestroyed(ADONBullet* Bullet)
{
	if (!bRemovingBullets) Bullets.Remove(Bullet);
	if (Bullets.Num() < 1) {
		AbilitySystemComponent->ApplyGameplayEffectToSelf(BulletCarryEffect->GetDefaultObject<UGameplayEffect>(), 1.f, AbilitySystemComponent->MakeEffectContext());

		static FGameplayTag receivedBulletEffect = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Player.ReceivedBullet"));
		AbilitySystemComponent->ExecuteGameplayCue(receivedBulletEffect);
	}
}

void ADONPlayerCharacter::ReceivePowerup(FDataTableRowHandle Powerup, bool bIsUnique)
{
	if (!Powerup.DataTable) return;

	static const FString context = "Powerup lookup";
	FPowerupData* Data = Powerup.GetRow<FPowerupData>(context);

	auto Context = AbilitySystemComponent->MakeEffectContext();
	auto Spec = AbilitySystemComponent->MakeOutgoingSpec(Data->EffectClass, 1.f, Context);
	auto UI = Cast<UPowerupUI>(Spec.Data->Def->UIData);
	if (UI) {
		UI->Texture = Data->Icon.Get();
		UI->Name = Data->Name;
	}
	auto Handle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);

	if (!ActivePowerup.IsValid() && !Data->bIsPassive) {
		AbilityIndex = 0;
		ActivePowerup = AbilitySystemComponent->FindAbilitySpecFromGEHandle(Handle)->Handle;
		OnActivePowerupChanged.Broadcast(ActivePowerup, AbilitySystemComponent->FindAbilitySpecFromHandle(ActivePowerup)->GameplayEffectHandle);
	}

	static FGameplayTag powerupTag = FGameplayTag::RequestGameplayTag(FName("Ability.Powerup"));
	TArray<FGameplayAbilitySpecHandle> ActivePowerupsArray;
	AbilitySystemComponent->FindAllAbilitiesWithTags(ActivePowerupsArray, powerupTag.GetSingleTagContainer(), false);

	if (ActivePowerupsArray.Num() == 2) {
		OnHasMultipleActivePowerups.Broadcast();
	}

	if (bIsUnique) {
		GetPlayerState<ADONPlayerState>()->SetCanReceiveEasyPowerup();
	}
}

void ADONPlayerCharacter::SpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ADONPlayerCharacter::BulletSpeedChanged(const FOnAttributeChangeData& Data)
{
	for (auto& b : Bullets) {
		if (b->IsValidLowLevel()) b->SetSpeedMultiplier(Data.NewValue);
	}
}

void ADONPlayerCharacter::BulletScaleChanged(const FOnAttributeChangeData& Data)
{
	for (auto& b : Bullets) {
		if (b->IsValidLowLevel()) b->SetActorScale3D(FVector{ Data.NewValue });
	}
}

void ADONPlayerCharacter::RefreshHandle()
{
	static FGameplayTag powerupTag = FGameplayTag::RequestGameplayTag(FName("Ability.Powerup"));
	TArray<FGameplayAbilitySpecHandle> Array;
	AbilitySystemComponent->FindAllAbilitiesWithTags(Array, powerupTag.GetSingleTagContainer(), false);

	if (Array.Num() < 1) return;

	if (AbilityIndex >= Array.Num()) {
		AbilityIndex = 0;
	}
	ActivePowerup = Array[AbilityIndex];
	OnActivePowerupChanged.Broadcast(ActivePowerup, AbilitySystemComponent->FindAbilitySpecFromHandle(ActivePowerup)->GameplayEffectHandle);
}

