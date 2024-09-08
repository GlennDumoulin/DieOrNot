// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CheckBox.h"
#include "DONCheckBox.generated.h"

class SDONCheckBox : public SCheckBox
{
public:
	void PlaySounds();
};

UCLASS()
class DON_API UDONCheckBox : public UCheckBox
{
	GENERATED_BODY()
public:
	FCheckBoxStyle FocusedCheckBoxStyle;
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void SlateOnCheckStateChangedCallback_Good(ECheckBoxState NewState);


	void OnFocusChange(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath, const TSharedPtr<SWidget>& Widget,
		const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& MyNewWidget);
};
