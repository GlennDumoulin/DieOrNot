// Fill out your copyright notice in the Description page of Project Settings.


#include "DONFunctionLibrary.h"
#include "GameplayEffect.h"
#include "DONViewportClient.h"
#include "GameFramework/PlayerController.h"

float UDONFunctionLibrary::GetGameplayEffectMagnitude(const TSubclassOf<UGameplayEffect>& EffectClass, int index)
{
	if (UClass* ActualPtr = EffectClass.Get())
	{
		const auto& Data = GetDefault<UGameplayEffect>(ActualPtr)->Modifiers;
		if (!Data.IsEmpty() && Data.IsValidIndex(index))
		{
			float Out;
			Data[index].ModifierMagnitude.GetStaticMagnitudeIfPossible(1.0, Out);
			if (Data[index].ModifierOp == EGameplayModOp::Multiplicitive)
				Out -= 1.f;
			return Out;
		}
	}
	return 0.0f;
}

const TArray<FLinearColor>& UDONFunctionLibrary::GetPlayerColors()
{
	static TArray<FLinearColor> PlayerColors = {
		{0.0, 1.0, 0.0, 1.0},
		{0.0, 0.0, 1.0, 1.0},
		{1.0, 0.0, 0.0, 1.0},
		{1.0, 1.0, 0.0, 1.0},
		{1.0, 0.0, 1.0, 1.0},
		{0.0, 1.0, 1.0, 1.0},
	};

	return PlayerColors;
}

void UDONFunctionLibrary::FadeScreen(float Time, bool bToBlack)
{
	UDONViewportClient* GameViewportClient = Cast<UDONViewportClient>(GWorld->GetGameViewport());
	if (GameViewportClient)
	{
		GameViewportClient->Fade(Time, bToBlack);
	}

}