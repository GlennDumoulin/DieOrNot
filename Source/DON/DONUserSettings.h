// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "DONUserSettings.generated.h"
#define DON_GAMEUSERSETTINGS_VERSION 9
/**
 * 
 */
UCLASS(config = GameUserSettings, configdonotcheckdefaults)
class DON_API UDONUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	// Wipes serialized data if version is invalid
	virtual bool IsVersionValid();
	virtual void UpdateVersion();

	virtual void SetToDefaults() override;

	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ApplyNonResolutionSettings() override;

	UFUNCTION(BlueprintCallable, Category = Settings)
	static UDONUserSettings* GetDONUserSettings();

	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetEnableChallenges(bool enabled);

	UFUNCTION(BlueprintPure, Category = Settings)
		bool GetEnableChallenges() const { return bChallengesEnabled; }

	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetPointLimit(int Limit);

	UFUNCTION(BlueprintPure, Category = Settings)
		int GetPointLimit() const { return PointLimit; }

	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetEnableEasyPowers(bool enabled);

	UFUNCTION(BlueprintPure, Category = Settings)
		bool GetEnableEasyPowers() const { return bEasyEnabled; }

	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetEnableActivePowers(bool enabled);

	UFUNCTION(BlueprintPure, Category = Settings)
		bool GetEnableActivePowers() const { return bActivesEnabled; }

	UFUNCTION(BlueprintCallable, Category = Settings)
		void SetEnableTutorials(bool enabled);

	UFUNCTION(BlueprintPure, Category = Settings)
		bool GetEnableTutorials() const { return bTutorialEnabled; }

protected:

	UPROPERTY(config)
	bool bChallengesEnabled;

	UPROPERTY(config)
	int PointLimit;

	UPROPERTY(config)
	bool bEasyEnabled;

	UPROPERTY(config)
	bool bActivesEnabled;
	
	UPROPERTY(config)
	bool bTutorialEnabled;

};
