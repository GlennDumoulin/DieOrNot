// Fill out your copyright notice in the Description page of Project Settings.


#include "DONGameInstance.h"
#include "DON/DONFunctionLibrary.h"
#include "DON/DONNavigationConfig.h"
#include "Kismet/GameplayStatics.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "DON/DONUserSettings.h"
#include "AbilitySystemGlobals.h"

FLinearColor UDONGameInstance::GetPlayerColor(FPlatformUserId Player)
{
	if (!Colors.Contains(Player)) return { 1.0, 0.0, 0.0, 1.0 };
	return Colors[Player];
}

FLinearColor UDONGameInstance::GetPlayerColorInSlot(int Player)
{
	if (auto color = Players.Find(Player); color != nullptr) {
		return Colors[Players[Player]];
	}

	return FLinearColor(0.5, 0.5, 0.5, 1.0);
}

void UDONGameInstance::SetPlayerColor(FPlatformUserId Player)
{
	int Search = -1;

	if (int* C = TakenColors.Find(Player); C != nullptr) {
		Search = *C;
		TakenColors.Remove(Player);
	}

	auto& Col = UDONFunctionLibrary::GetPlayerColors();
	TArray<int> Taken;
	TakenColors.GenerateValueArray(Taken);
	int Current = Search + 1;

	while (Search != Current) {
		if (Current >= Col.Num()) Current = 0;

		if (!Taken.Contains(Current)) {
			TakenColors.Add(Player, Current);
			Colors.Add(Player, Col[Current]);
			break;
		}
		Current++;
	}

	
}

ADONPlayerCharacter* UDONGameInstance::GetPlayerInSlot(int slot)
{
	if (auto p = Players.Find(slot); p) {
		return UGameplayStatics::GetPlayerControllerFromPlatformUser(GetWorld(), *p)->GetPawn<ADONPlayerCharacter>();
	}
	return nullptr;
}

int UDONGameInstance::GetPlayerSlot(ADONPlayerCharacter* Player)
{
	auto User = Player->GetController<APlayerController>()->GetPlatformUserId();
	for (auto It = Players.CreateConstIterator(); It; ++It)
	{
		const FPlatformUserId& LoopValue = It.Value();
		if (User == LoopValue)
		{
			return It.Key();
		}
	}
	return -1;
}

void UDONGameInstance::JoinPlayer(FPlatformUserId Player)
{
	int Index = 0;
	for (int i = 0; i < 4; ++i) {
		if (!Players.Contains(i)) {
			Index = i;
			break;
		}
	}
	Players.Add(Index, Player);
	SetPlayerColor(Player);
}

void UDONGameInstance::RemovePlayer(FPlatformUserId Player)
{
	for (auto It = Players.CreateConstIterator(); It; ++It)
	{
		const FPlatformUserId& LoopValue = It.Value();
		if (Player == LoopValue)
		{
			Players.Remove(It.Key());
			TakenColors.Remove(Player);
			break;
		}
	}

	Colors.Remove(Player);
}

void UDONGameInstance::ClearPlayers()
{
	Players.Empty();
	Colors.Empty();
	TakenColors.Empty();
}

void UDONGameInstance::SetEnableChallenges(bool enabled)
{
	bChallengesEnabled = enabled;
	UDONUserSettings::GetDONUserSettings()->SetEnableChallenges(enabled);
}

void UDONGameInstance::SetPointLimit(int Limit)
{
	PointLimit = Limit;
	UDONUserSettings::GetDONUserSettings()->SetPointLimit(Limit);
}

void UDONGameInstance::SetEnableEasyPowers(bool enabled)
{
	bEasyEnabled = enabled;
	UDONUserSettings::GetDONUserSettings()->SetEnableEasyPowers(enabled);
}

void UDONGameInstance::SetEnableActivePowers(bool enabled)
{
	bActivesEnabled = enabled;
	UDONUserSettings::GetDONUserSettings()->SetEnableActivePowers(enabled);
}

void UDONGameInstance::SetEnableTutorials(bool enabled)
{
	bTutorialsEnabled = enabled;
	UDONUserSettings::GetDONUserSettings()->SetEnableTutorials(enabled);
}

void UDONGameInstance::Init()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
	Super::Init();

	auto set = UDONUserSettings::GetDONUserSettings();

	bChallengesEnabled = set->GetEnableChallenges();
	bEasyEnabled = set->GetEnableEasyPowers();
	bActivesEnabled = set->GetEnableActivePowers();
	bTutorialsEnabled = set->GetEnableTutorials();
	PointLimit = set->GetPointLimit();


	FSlateApplication::Get().SetNavigationConfig(MakeShared<FDONNavigationConfig>());
}

void UDONGameInstance::Shutdown()
{
#if WITH_EDITOR
	// Reset custom Slate input binding to default
	// Only done in editor for safety - runtime standalone process won't need this
	FSlateApplication::Get().SetNavigationConfig(MakeShared<FNavigationConfig>());
#endif
	UDONUserSettings::GetDONUserSettings()->ApplySettings(false);

	Super::Shutdown();
}