// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Actors/StartGameZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DON/Gameplay/MainMenu/MenuGameMode.h"

// Sets default values
AStartGameZone::AStartGameZone()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collider"));
	RootComponent = BoxCollider;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	/*TextRenderer = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	TextRenderer->SetRelativeLocation(FVector{ 0.f, 0.f, 50.f });
	TextRenderer->SetupAttachment(RootComponent);*/

	bIsTimerEnabled = false;
	bHasGameStarted = false;
	bPrevCanStartGame = false;
}

// Called when the game starts or when spawned
void AStartGameZone::BeginPlay()
{
	// Bind overlap events
	BoxCollider->OnComponentBeginOverlap.Clear();
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AStartGameZone::OnBeginOverlap);
	BoxCollider->OnComponentEndOverlap.Clear();
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AStartGameZone::OnEndOverlap);

	// Set required players
	RequiredPlayerAmount = GIsEditor ? 1 : 2;

	// Set initial text
	SetText("Not enough players connected");

	Super::BeginPlay();
}

void AStartGameZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is a player
	auto Player = Cast<ADONPlayerCharacter>(OtherActor);
	if (Player != nullptr)
	{
		// Increase current amount of present players
		++CurrentPlayerAmount;

		// Check if we can start game
		if (CurrentPlayerAmount >= RequiredPlayerAmount)
		{
			bIsTimerEnabled = true;
			
			OnTimerStart();
		}

		// Update text
		UpdateText();
	}
}

void AStartGameZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if the overlapping actor was a player
	auto Player = Cast<ADONPlayerCharacter>(OtherActor);
	if (Player != nullptr)
	{
		// Decrease current amount of present players
		--CurrentPlayerAmount;

		// Check if the timer needs to stop
		if (CurrentPlayerAmount < RequiredPlayerAmount)
		{
			ElapsedStartGameTime = 0.f;
			bIsTimerEnabled = false;
			
			OnTimerEnd();
		}

		// Update text
		UpdateText();
	}
}

void AStartGameZone::UpdateText()
{
	FString Msg = FString("");

	if (CurrentPlayerAmount < RequiredPlayerAmount)
	{
		Msg.Append(FString::FromInt(CurrentPlayerAmount));
		Msg.Append(" out of ");
		Msg.Append(FString::FromInt(RequiredPlayerAmount));
		Msg.Append(" players");
	}
	else if (bHasGameStarted)
	{
		Msg.Append("Game starting!");
	}
	else
	{
		Msg.Append("Starting in ");
		Msg.Append(FString::SanitizeFloat(ceilf((StartGameTime - ElapsedStartGameTime) * 10) / 10));
	}

	SetText(Msg);
}

//void AStartGameZone::SetText(const FString& NewText)
//{
//	//TextRenderer->SetText(FText::FromString(NewText));
//}

// Called every frame
void AStartGameZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if the game has started already
	if (bHasGameStarted) return;

	// Check if we can start a game
	auto MenuGameMode = Cast<AMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MenuGameMode != nullptr)
	{
		bool bCanStartGame = MenuGameMode->CanStartGame();

		if (bCanStartGame)
		{
			if (bCanStartGame != bPrevCanStartGame)
			{
				bPrevCanStartGame = bCanStartGame;
				UpdateText();
			}

			// Check if the timer should update
			if (bIsTimerEnabled)
			{
				// Update the start game timer
				ElapsedStartGameTime += DeltaTime;

				// Update text
				UpdateText();

				if (ElapsedStartGameTime >= StartGameTime)
				{
					bHasGameStarted = true;
					HandleStartGame();
				}

				// Return so the timer does not get reset
				return;
			}
		}
		else
		{
			if (bCanStartGame != bPrevCanStartGame)
			{
				bPrevCanStartGame = bCanStartGame;
				SetText("Not enough players connected");
			}
		}
	}

	// If the timer wa not updated reset it
	ElapsedStartGameTime = 0.f;
}

