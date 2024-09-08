// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DONPlayerState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int Survives;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int Kills;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int Deaths;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int Suicides;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int KillStreak;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int LoseStreak;

	FPlayerStats() {
		Kills = 0;
		Deaths = 0;
		Suicides = 0;
		KillStreak = 0;
		LoseStreak = 0;
		Survives = 0;
	}

	int CalculateScore() {
		return 100 * Survives + 70 * (Kills - 0.8 * Suicides - 0.5 * Deaths) + 80 * (KillStreak - LoseStreak);
	}
};


UCLASS()
class DON_API ADONPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADONPlayerState();

	void RoundStart(FTransform location);
	void GiveKillScore();
	void MarkDeath(bool suicide);
	void GiveSurviveScore();
	void SetSuicide();
	void SetCanReceivePowerup();
	void SetCanReceiveEasyPowerup();

	int GetSuicides() const { return Stats.Suicides; }
	
	UFUNCTION(BlueprintPure)
	FPlayerStats GetPlayerStats() const { return Stats; }

	UFUNCTION(BlueprintPure)
	int GetPlayerScore() { return PlayerScore; }
	UFUNCTION(BlueprintPure)
	int GetHiddenScore();

	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColor() const;

	UFUNCTION(BlueprintPure)
	bool CanReceivePowerup() const { return bCanReceivePowerup; }

	bool CanReceiveEasyPowerup() const { return bCanReceiveEasyPowerup; }

private:

	bool bReceivedSurviveScore;
	bool bReceivedKillScore;
	bool bCanReceivePowerup;
	bool bCanReceiveEasyPowerup;
	int PlayerScore;

	FPlayerStats Stats;
};
