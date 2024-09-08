// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/NavigationConfig.h" // from Slate

class DON_API FGameNavigationConfig : public FNavigationConfig
{
public:
	FGameNavigationConfig();

	virtual EUINavigation GetNavigationDirectionFromAnalog(const FAnalogInputEvent& InAnalogEvent) override;

	virtual EUINavigationAction GetNavigationActionForKey(const FKey& InKey) const override;
};
