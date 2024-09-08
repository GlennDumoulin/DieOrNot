// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "DONSlider.generated.h"

UCLASS()
class DON_API UDONSlider : public USlider
{
	GENERATED_BODY()
public:
	FSliderStyle FocusedSliderStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Sound, meta = (DisplayName = "Hovered Sound"))
	FSlateSound HoveredSlateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Sound, meta = (DisplayName = "Value Changed Sound"))
	FSlateSound ChangedSlateSound;

	const FSlateSound& GetHoveredSlateSound() const { return HoveredSlateSound; }
	void SetHoveredSlateSound(const FSlateSound& InStyle) { HoveredSlateSound = InStyle; }

	const FSlateSound& GetChangedSlateSound() const { return ChangedSlateSound; }
	void SetChangedSlateSound(const FSlateSound& InStyle) { ChangedSlateSound = InStyle; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void SlateOnValueChanged_Good(float InValue);


	void OnFocusChange(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath, const TSharedPtr<SWidget>& Widget,
		const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& MyNewWidget);
};
