// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Gameplay/MainMenu/MenuController.h"
#include "Kismet/GameplayStatics.h"
#include "DON/Gameplay/DONGameInstance.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "MenuGameMode.h"

AMenuController::AMenuController()
{
	bReady = false;
}

void AMenuController::BeginPlay()
{
	Super::BeginPlay();
}

void AMenuController::SetReady(bool ready)
{
	bReady = ready;

	if (bReady) {
		GetPawn()->SetActorTransform(UGameplayStatics::GetGameMode(this)->ChoosePlayerStart(this)->GetTransform());
		GetGameInstance<UDONGameInstance>()->JoinPlayer(GetPlatformUserId());
		GetPawn<ADONPlayerCharacter>()->SetPlayerColor(GetGameInstance<UDONGameInstance>()->GetPlayerColor(GetPlatformUserId()));
		GetPawn()->Reset();
	}
	else {
		GetGameInstance<UDONGameInstance>()->RemovePlayer(GetPlatformUserId());
		GetPawn<ADONPlayerCharacter>()->TryKillPlayer(nullptr);
	}
}

void AMenuController::NextPlayerColor()
{
	GetGameInstance<UDONGameInstance>()->SetPlayerColor(GetPlatformUserId());
	GetPawn<ADONPlayerCharacter>()->SetPlayerColor(GetGameInstance<UDONGameInstance>()->GetPlayerColor(GetPlatformUserId()));
}
