// Fill out your copyright notice in the Description page of Project Settings.


#include "DONUserSettings.h"
#include "Gameplay/DONGameInstance.h"

void UDONUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	bChallengesEnabled = false;
	bEasyEnabled = true;
	bActivesEnabled = true;
	bTutorialEnabled = true;
	PointLimit = 10;
}

bool UDONUserSettings::IsVersionValid()
{
	return (Version == DON_GAMEUSERSETTINGS_VERSION);
}

void UDONUserSettings::UpdateVersion()
{
	Version = DON_GAMEUSERSETTINGS_VERSION;
}

void UDONUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
}

void UDONUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
}

UDONUserSettings* UDONUserSettings::GetDONUserSettings()
{
	if (GEngine)
		return Cast<UDONUserSettings>(GEngine->GetGameUserSettings());
	else return nullptr;
}

void UDONUserSettings::SetEnableChallenges(bool enabled)
{
	bChallengesEnabled = enabled;
}

void UDONUserSettings::SetPointLimit(int Limit)
{
	PointLimit = Limit;
}

void UDONUserSettings::SetEnableEasyPowers(bool enabled)
{
	bEasyEnabled = enabled;
}

void UDONUserSettings::SetEnableActivePowers(bool enabled)
{
	bActivesEnabled = enabled;
}

void UDONUserSettings::SetEnableTutorials(bool enabled)
{
	bTutorialEnabled = enabled;
}
