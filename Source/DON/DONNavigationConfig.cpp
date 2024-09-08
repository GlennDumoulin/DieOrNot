// Fill out your copyright notice in the Description page of Project Settings.

#include "DONNavigationConfig.h"
#include "GameFramework/InputSettings.h"

FDONNavigationConfig::FDONNavigationConfig()
{
	KeyEventRules.Reset();
	bTabNavigation = false;
	bKeyNavigation = true;
	bAnalogNavigation = true;
	AnalogNavigationHorizontalThreshold = 0.5f;
	AnalogNavigationVerticalThreshold = 0.5f;
	AnalogHorizontalKey = EKeys::Gamepad_LeftX;
	AnalogVerticalKey = EKeys::Gamepad_LeftY;
	
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

	KeyEventRules.Emplace(EKeys::Left, EUINavigation::Left);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Left, EUINavigation::Left);

	KeyEventRules.Emplace(EKeys::Right, EUINavigation::Right);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Right, EUINavigation::Right);

	KeyEventRules.Emplace(EKeys::Up, EUINavigation::Up);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Up, EUINavigation::Up);

	KeyEventRules.Emplace(EKeys::Down, EUINavigation::Down);
	KeyEventRules.Emplace(EKeys::Gamepad_DPad_Down, EUINavigation::Down);
}

EUINavigationAction FDONNavigationConfig::GetNavigationActionForKey(const FKey& InKey) const
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