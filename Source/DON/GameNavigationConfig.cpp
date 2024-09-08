// Fill out your copyright notice in the Description page of Project Settings.

#include "GameNavigationConfig.h"
#include "GameFramework/InputSettings.h"

FGameNavigationConfig::FGameNavigationConfig()
{
	KeyEventRules.Reset();
	bTabNavigation = false;
	bKeyNavigation = false;
	bAnalogNavigation = false;

	UInputSettings* InputSettings = UInputSettings::GetInputSettings();



	// Assign Previous input navigation
	TArray<FInputActionKeyMapping> PreviousKeyMapping;
	InputSettings->GetActionMappingByName(TEXT("MenuPrevious"), PreviousKeyMapping);
	for (int i = 0; i < PreviousKeyMapping.Num(); i++)
	{
		KeyEventRules.Emplace(PreviousKeyMapping[i].Key, EUINavigation::Previous);
	}

	// Assign Next input navigation
	TArray<FInputActionKeyMapping> NextKeyMapping;
	InputSettings->GetActionMappingByName(TEXT("MenuNext"), NextKeyMapping);
	for (int i = 0; i < NextKeyMapping.Num(); i++)
	{
		KeyEventRules.Emplace(NextKeyMapping[i].Key, EUINavigation::Next);
	}
}

EUINavigation FGameNavigationConfig::GetNavigationDirectionFromAnalog(const FAnalogInputEvent& InAnalogEvent)
{
	// Disable analog direction as thumbsticks can be assigned to action key mapping.
	return EUINavigation::Invalid;
}

EUINavigationAction FGameNavigationConfig::GetNavigationActionForKey(const FKey& InKey) const
{
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();

	// Assign Accept action mapping
	TArray<FInputActionKeyMapping> AcceptKeyMapping;
	InputSettings->GetActionMappingByName(TEXT("MenuAccept"), AcceptKeyMapping);
	for (int i = 0; i < AcceptKeyMapping.Num(); i++)
		if (InKey == AcceptKeyMapping[i].Key)
			return EUINavigationAction::Accept;

	// Assign Back action mapping
	TArray<FInputActionKeyMapping> BackKeyMapping;
	InputSettings->GetActionMappingByName(TEXT("MenuBack"), BackKeyMapping);
	for (int i = 0; i < BackKeyMapping.Num(); i++)
		if (InKey == BackKeyMapping[i].Key)
			return EUINavigationAction::Back;

	return EUINavigationAction::Invalid;
}