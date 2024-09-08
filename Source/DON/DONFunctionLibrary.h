// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DONFunctionLibrary.generated.h"

class UGameplayEffect;

UCLASS()
class DON_API UDONFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
	static float GetGameplayEffectMagnitude(const TSubclassOf<UGameplayEffect>& EffectClass, int index);

	UFUNCTION(BlueprintPure)
	static const TArray<FLinearColor>& GetPlayerColors();

	UFUNCTION(BlueprintCallable, Category = Loading)
	static void FadeScreen(float Time, bool bToBlack);

};
