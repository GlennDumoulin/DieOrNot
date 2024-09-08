// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DONGameInstance.generated.h"

class ADONPlayerCharacter;

UCLASS()
class DON_API UDONGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColor(FPlatformUserId Player);
	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColorInSlot(int Player);

	void SetPlayerColor(FPlatformUserId Player);

	const auto& GetPlayers() const { return Players; }

	UFUNCTION(BlueprintPure)
	ADONPlayerCharacter* GetPlayerInSlot(int slot);

	UFUNCTION(BlueprintPure)
	int GetPlayerSlot(ADONPlayerCharacter* Player);

	void JoinPlayer(FPlatformUserId Player);
	void RemovePlayer(FPlatformUserId Player);
	void ClearPlayers();

	UFUNCTION(BlueprintCallable)
	void SetEnableChallenges(bool enabled);

	UFUNCTION(BlueprintPure)
	bool GetEnableChallenges() const { return bChallengesEnabled; }

	UFUNCTION(BlueprintCallable)
	void SetPointLimit(int Limit);

	UFUNCTION(BlueprintPure)
	int GetPointLimit() const { return PointLimit; }

	UFUNCTION(BlueprintCallable)
	void SetEnableEasyPowers(bool enabled);

	UFUNCTION(BlueprintPure)
	bool GetEnableEasyPowers() const { return bEasyEnabled; }

	UFUNCTION(BlueprintCallable)
	void SetEnableActivePowers(bool enabled);

	UFUNCTION(BlueprintPure)
	bool GetEnableActivePowers() const { return bActivesEnabled; }

	UFUNCTION(BlueprintCallable)
	void SetEnableTutorials(bool enabled);

	UFUNCTION(BlueprintPure)
	bool GetEnableTutorials() const { return bTutorialsEnabled; }

private:

	bool bChallengesEnabled;
	bool bEasyEnabled;
	bool bActivesEnabled;
	bool bTutorialsEnabled;
	int PointLimit;

	TMap<int, FPlatformUserId> Players;
	TMap<FPlatformUserId, int> TakenColors;

	TMap<FPlatformUserId, FLinearColor> Colors;
};
