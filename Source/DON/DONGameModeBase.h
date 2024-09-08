// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DONGameModeBase.generated.h"

class ADONPlayerCharacter;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPowerupSelectChanged, ADONPlayerState*, Player, bool, bGrantBoost);

UENUM(BlueprintType)
enum class EWinReason : uint8
{
	Points,
	Suicides,
	Draw,
};

UCLASS()
class DON_API ADONGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

	void RegisterPlayerDeath(ADONPlayerCharacter* dead, ADONPlayerCharacter* killer);

	// Game logic

	UFUNCTION(BlueprintPure)
	float GetRoundTime() const { return RoundTimer; }

	UFUNCTION(BlueprintPure)
	bool CanBeginRound() const;

	UFUNCTION(BlueprintPure)
	bool CanSelectPowerups(bool& WasGameEnd);

	UFUNCTION(BlueprintCallable)
	void BeginRound();
	void EndRound();

	bool CheckRoundEnd();
	bool CheckGameEndCondition();

	void EndGame();

	UFUNCTION(BlueprintCallable)
	void RoundStartTimerElapsed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRoundStart();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RoundStartDuration;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRoundEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int TargetScore;

	UFUNCTION(BlueprintCallable)
	ADONPlayerCharacter* PowerupSelected(FDataTableRowHandle Powerup, bool bIsUnique);

	UPROPERTY(BlueprintAssignable)
	FPowerupSelectChanged OnPowerupSelectChanged;

	UFUNCTION(BlueprintCallable)
	void StartPowerupSelection();

	UFUNCTION(BlueprintCallable)
	void SkipPowerups();

	UFUNCTION(BlueprintCallable)
	void SwitchNavigationMode(bool isMenu);
protected:
#pragma region Loading
	void LoadRandomLevel();
	void SwitchLevel();

	UFUNCTION()
	void FinishedLoading();

	UFUNCTION()
	void FinishedSwitching();

	UFUNCTION()
	void UnloadFinished();

	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	bool bCanStartRound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDataTable* MapData;

	TSoftObjectPtr<UWorld> CurrentLevelRef;
	TSoftObjectPtr<UWorld> CurrentChallengeRef;

	TSoftObjectPtr<UWorld> NextLevel;
	TSoftObjectPtr<UWorld> NextChallenge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> UsedSpawns;
#pragma endregion

	bool bRoundInProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerController*> PlayersAlive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ADONPlayerCharacter* Winner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ADONPlayerCharacter*> GameWinners;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWinReason WinReason;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ADONPlayerState*> DeathOrder;

#pragma region Powerups

	bool bFinishedPowerups;

	int CurrentSelectIndex;

	bool ShouldReceiveBoost(ADONPlayerState* Player);

#pragma endregion

private:
	
	float RoundTimer;
};
