// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DONButton.generated.h"

/**
 * 
 */

class SDONButton : public SButton
{
public:
	void PlaySounds();
};

UCLASS()
class DON_API UDONButton : public UButton
{
	GENERATED_BODY()
public:
	FButtonStyle FocusedButtonStyle;
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;


	/** Handle the actual click event from slate and forward it on */
	FReply SlateHandleClicked_Good();
	void SlateHandlePressed_Good();
	void SlateHandleReleased_Good();
	void SlateHandleHovered_Good();
	void SlateHandleUnhovered_Good();


	void OnFocusChange(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath, const TSharedPtr<SWidget>& Widget,
		const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& MyNewWidget);
};
