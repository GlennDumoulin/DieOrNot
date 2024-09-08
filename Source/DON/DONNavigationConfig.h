// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/NavigationConfig.h" // from Slate

class DON_API FDONNavigationConfig : public FNavigationConfig
{
public:
	FDONNavigationConfig();

	virtual EUINavigationAction GetNavigationActionForKey(const FKey& InKey) const override;
};
