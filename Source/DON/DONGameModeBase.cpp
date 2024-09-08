// Copyright Epic Games, Inc. All Rights Reserved.


#include "DONGameModeBase.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "DON/Gameplay/DONGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "DON/DONData.h"
#include "GameFramework/GameStateBase.h"
#include "Don/Gameplay/DONPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DONFunctionLibrary.h"
#include "DONNavigationConfig.h"
#include "GameNavigationConfig.h"

void ADONGameModeBase::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	UE_LOG(LogTemp, Display, TEXT("Player created and defaults set"));
}

void ADONGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Display, TEXT("Player login"));

	if (GetGameInstance<UDONGameInstance>()->GetPlayers().Num() != 0) {
		for (auto& [id, Platform] : GetGameInstance<UDONGameInstance>()->GetPlayers()) {
			if (Platform == NewPlayer->GetPlatformUserId()) {
				RestartPlayer(NewPlayer);
				return;
			}	
		}
		/*if (GetWorld()->GetFirstPlayerController() != NewPlayer)*/ UGameplayStatics::RemovePlayer(NewPlayer, true);
	}
	else RestartPlayer(NewPlayer);
}

void ADONGameModeBase::BeginPlay()
{
	bCanStartRound = false;
	bFinishedPowerups = false;
	bRoundInProgress = false;
	GameWinners.Empty();

	TargetScore = GetGameInstance<UDONGameInstance>()->GetPointLimit();

	if (GetGameInstance<UDONGameInstance>()->GetPlayers().Num() != 0)
	for (auto pla = GetWorld()->GetPlayerControllerIterator(); pla; ++pla) {
		bool Found = false;
		for (auto& [id, Platform] : GetGameInstance<UDONGameInstance>()->GetPlayers()) {
			if (Platform == pla->Get()->GetPlatformUserId()) {
				Found = true;
				break;;
			}
		}
		if (!Found) UGameplayStatics::RemovePlayer(pla->Get(), true);
	}

	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It) {
		const TArray<ULocalPlayer*> LocalPlayers = GetGameInstance()->GetLocalPlayers();
		auto pla = GetWorld()->GetPlayerControllerIterator();
		GetWorld()->GetFirstPlayerController()->SetViewTarget(*It);
		break;
	}
	
	UDONFunctionLibrary::FadeScreen(0.5f, false);

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		Super::BeginPlay();

		LoadRandomLevel();
		BeginRound();

	});
}

void ADONGameModeBase::Tick(float Delta)
{
	if (bRoundInProgress)	
		RoundTimer += Delta;
}

void ADONGameModeBase::RegisterPlayerDeath(ADONPlayerCharacter* dead, ADONPlayerCharacter* killer)
{
	PlayersAlive.Remove(Cast<APlayerController>(dead->GetController()));
	auto State = dead->GetPlayerState<ADONPlayerState>();
	if (!State) return;

	bool powers = GetGameInstance<UDONGameInstance>()->GetEnableEasyPowers();
	if (killer && (killer != dead || powers)) {
		if (!killer->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead")))) killer->GetPlayerState<ADONPlayerState>()->GiveKillScore();
		State->SetCanReceivePowerup();
	}
	else State->SetSuicide();

	State->MarkDeath(killer == dead);
	DeathOrder.Push(State);

	if (CheckRoundEnd()) 
		EndRound();
}

bool ADONGameModeBase::CanBeginRound() const
{
	return bFinishedPowerups;
}

bool ADONGameModeBase::CanSelectPowerups(bool& WasGameEnd)
{
	WasGameEnd = CheckGameEndCondition();
	return !WasGameEnd && DeathOrder.ContainsByPredicate([](ADONPlayerState* player) { return player->CanReceivePowerup(); });
}

void ADONGameModeBase::BeginRound()
{
	bool bEndGame = CheckGameEndCondition();
	if (bEndGame) {
		EndGame();
	} 
	else if (bCanStartRound) {
		UsedSpawns.Empty();
		PlayersAlive.Empty();
		DeathOrder.Empty();
		SwitchLevel();
		bFinishedPowerups = false;
		RoundTimer = 0.f;
		APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (MyPlayer != nullptr)
		{
			MyPlayer->SetPause(false);

			// Give all players the round start input mappings and activate the round start HUD
			
			OnRoundStart();
		}
	}
	else {
		for (auto& p : GameState->PlayerArray) {
			if (auto c = p->GetPawn<ADONPlayerCharacter>(); c) {
				c->GetCharacterMovement()->GravityScale = 0.f;
				c->SetActorEnableCollision(false);
			}
		}
		GetWorldTimerManager().SetTimerForNextTick([this]() {
			BeginRound();
		});
	}
}

void ADONGameModeBase::EndRound()
{
	for (auto& p : GameState->PlayerArray) {
		if (p->GetPawn()) p->GetPawn<ADONPlayerCharacter>()->RoundEnd();
	}

	if (!PlayersAlive.IsEmpty()) {
		static FGameplayTag dead = FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead"));
		Winner = PlayersAlive[0]->GetPawn<ADONPlayerCharacter>();
		Winner->GetPlayerState<ADONPlayerState>()->GiveSurviveScore();
		Winner->OnVictory();
		Winner->GetAbilitySystemComponent()->AddLooseGameplayTag(dead);

	}

	APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != nullptr)
	{
		MyPlayer->SetPause(false);
	}

	bool bEndGame = CheckGameEndCondition();

	if (!bEndGame) {
		LoadRandomLevel();
	}
	OnRoundEnd();

	bRoundInProgress = false;
}

bool ADONGameModeBase::CheckRoundEnd()
{
	if (!bRoundInProgress) return false;
	if (GetNumPlayers() > 1) {
		return PlayersAlive.Num() <= 1;
	}
	else return PlayersAlive.IsEmpty();
}

bool ADONGameModeBase::CheckGameEndCondition()
{
	int Highest = 0;
	for (auto& p : GameState->PlayerArray) {
		int Score = Cast<ADONPlayerState>(p)->GetPlayerScore();
		if (Score >= TargetScore) {
			if (Score > Highest) {
				GameWinners.Empty();
			}
			Highest = Score;
			GameWinners.Add(p->GetPawn<ADONPlayerCharacter>());
		}
	}
	if (!GameWinners.IsEmpty()) {
		WinReason = EWinReason::Points;

		if (GameWinners.Num() > 1) {
			WinReason = EWinReason::Draw;
			ADONPlayerCharacter* Win = nullptr;
			int Suicides = 1000;
			bool Equal = false;
			for (auto& p : GameWinners) {
				int s = p->GetPlayerState<ADONPlayerState>()->GetSuicides();
				if (s < Suicides) {
					Suicides = s;
					Win = p;
				}
				else if (s == Suicides) {
					Equal = true;
				}
			}
			if (!Equal) {
				GameWinners.Empty();
				GameWinners.Add(Win);
				WinReason = EWinReason::Suicides;
			}
		}

		return true;
	}
	return false;
}

void ADONGameModeBase::EndGame()
{
	OnGameEnd();
}

void ADONGameModeBase::RoundStartTimerElapsed()
{
	for (auto& p : GameState->PlayerArray) {
		auto PlayerCharacter = p->GetPawn<ADONPlayerCharacter>();

		// Handle automatically shooting and giving all players the game input mappings
		PlayerCharacter->Shoot();
		PlayerCharacter->SetMainInputMapping();
	}
}

void ADONGameModeBase::LoadRandomLevel()
{
	bCanStartRound = false;
	auto Rows = MapData->GetRowNames();
	if (Rows.IsEmpty()) return;

	auto& Row = Rows[FMath::RandRange(0, Rows.Num() - 1)];
	auto Data = MapData->FindRow<FLevelData>(Row, "Retrieve data");
	FLatentActionInfo info;
	info.CallbackTarget = this;
	info.ExecutionFunction = "FinishedLoading";
	info.Linkage = 0;
	info.UUID = 10;

	if (GetGameInstance<UDONGameInstance>()->GetEnableChallenges())
		NextChallenge = Data->Challenges;
		
	if (!NextChallenge.IsNull()) 
		UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, Data->Challenges, false, false, {});
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, Data->Map, false, false, info);

	NextLevel = Data->Map;
}

void ADONGameModeBase::SwitchLevel()
{
	FLatentActionInfo info;
	info.CallbackTarget = this;
	info.ExecutionFunction = "UnloadFinished";
	info.Linkage = 0;
	info.UUID = 1;
	if (!CurrentChallengeRef.IsNull()) {
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(this, CurrentChallengeRef, {}, true);
	}
	if (CurrentLevelRef) UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(this, CurrentLevelRef, info, true);
	else UnloadFinished();
}

void ADONGameModeBase::FinishedLoading()
{
	bCanStartRound = true;
}

void ADONGameModeBase::FinishedSwitching()
{
	GetWorldTimerManager().SetTimerForNextTick([this]() {
		for (auto& p : GameState->PlayerArray) {
			if (!p->GetPawn()) continue;
			auto c = Cast<APlayerController>(p->GetPawn()->GetController());
			if (c) {
				auto Actor = ChoosePlayerStart(c);
				FTransform start{  };
				if (Actor) start = Actor->GetActorTransform();
				Cast<ADONPlayerState>(p)->RoundStart(start);
				for (auto& s : GameState->PlayerArray) {
					s->GetPawn<ADONPlayerCharacter>()->SetRoundStartInputMapping();
				}
				PlayersAlive.Add(c);
				
			}
		}
		bRoundInProgress = true;
	});
}

void ADONGameModeBase::UnloadFinished()
{
	GetWorldTimerManager().SetTimerForNextTick([this]() {
		CurrentChallengeRef = NextChallenge;
		CurrentLevelRef = NextLevel;
		NextLevel = nullptr;
		NextChallenge = nullptr;
		FLatentActionInfo info;
		info.CallbackTarget = this;
		info.ExecutionFunction = "FinishedSwitching";
		info.Linkage = 0;
		info.UUID = 1;
		if (!CurrentChallengeRef.IsNull())
			UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, CurrentChallengeRef, true, true, {});
		UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, CurrentLevelRef, true, true, info);
	});
}

AActor* ADONGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	APlayerStart* FoundPlayerStart = nullptr;
	UWorld* World = GetWorld();
	TArray<APlayerStart*> Starts;
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		Starts.Add(*It);
	}
	const int32 NumShuffles = Starts.Num() - 1;
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapIdx = FMath::RandRange(i, NumShuffles);
		Starts.Swap(i, SwapIdx);
	}
	for (auto& S : Starts) {
		if (UsedSpawns.Contains(S)) continue;

		UsedSpawns.Add(S);
		return S;
	}
	return nullptr;
}

bool ADONGameModeBase::ShouldReceiveBoost(ADONPlayerState* Player)
{
	if (!GetGameInstance<UDONGameInstance>()->GetEnableEasyPowers()) return false;

	if (!Player->CanReceiveEasyPowerup()) return false;
	int Lowest = 10000000;
	bool ReceiveByScore = false;
	for (auto p : GameState->PlayerArray) {
		if (auto state = Cast<ADONPlayerState>(p); state) {
			if (state->GetPlayerScore() > TargetScore / 2)
				ReceiveByScore = true;

			if (state->GetHiddenScore() < Lowest) Lowest = state->GetHiddenScore();
		}
	}

	if (!ReceiveByScore) return false;

	if (Player->GetHiddenScore() == Lowest) return true;

	return false;
}

ADONPlayerCharacter* ADONGameModeBase::PowerupSelected(FDataTableRowHandle Powerup, bool bIsUnique)
{
	auto Player = DeathOrder[CurrentSelectIndex]->GetPawn<ADONPlayerCharacter>();
	Player->ReceivePowerup(Powerup, bIsUnique);
	CurrentSelectIndex++;
	int i = CurrentSelectIndex;
	CurrentSelectIndex = DeathOrder.Num();
	for (; i < DeathOrder.Num(); ++i) {
		if (DeathOrder[i]->CanReceivePowerup()) {
			CurrentSelectIndex = i;
			break;
		}
	}
	if (CurrentSelectIndex >= DeathOrder.Num()) {
		bFinishedPowerups = true;
		OnPowerupSelectChanged.Broadcast(nullptr, false);
	}
	else {

		OnPowerupSelectChanged.Broadcast(DeathOrder[CurrentSelectIndex], ShouldReceiveBoost(DeathOrder[CurrentSelectIndex]));
	}

	return Player;
}

void ADONGameModeBase::StartPowerupSelection()
{
	if (CheckGameEndCondition()) {
		bFinishedPowerups = true;
		return;
	}
	CurrentSelectIndex = 0;
	for (int i = 0; i < DeathOrder.Num(); ++i) {
		if (DeathOrder[i]->CanReceivePowerup()) {
			CurrentSelectIndex = i;
			break;
		}
	}
	bFinishedPowerups = false;
	OnPowerupSelectChanged.Broadcast(DeathOrder[CurrentSelectIndex], ShouldReceiveBoost(DeathOrder[CurrentSelectIndex]));
}

void ADONGameModeBase::SkipPowerups()
{
	bFinishedPowerups = true;
}

void ADONGameModeBase::SwitchNavigationMode(bool isMenu)
{
	if (isMenu)
		FSlateApplication::Get().SetNavigationConfig(MakeShared<FDONNavigationConfig>());
	else
		FSlateApplication::Get().SetNavigationConfig(MakeShared<FGameNavigationConfig>());
}
