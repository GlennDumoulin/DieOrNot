// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Interface/DONSlider.h"
#include "Widgets/Input/SSlider.h"

class SDONSlider : public SSlider
{
private:
	FSlateSound ChangedSound;

	FSlateSound HoveredSound;

public:

	void SetHoverSound(const FSlateSound& Hover) {
		HoveredSound = Hover;
	}
	void SetChangedSound(const FSlateSound& Changed) {
		ChangedSound = Changed;
	}

	void PlayHoverSound() {
		FSlateApplication::Get().PlaySound(HoveredSound);
	}
	void PlayChangedSound() {
		FSlateApplication::Get().PlaySound(ChangedSound);
	}

};

TSharedRef<SWidget> UDONSlider::RebuildWidget()
{
	MySlider = SNew(SDONSlider)
		.OnValueChanged(BIND_UOBJECT_DELEGATE(FOnFloatValueChanged, SlateOnValueChanged_Good))
		.Style(&GetWidgetStyle())
		.IsFocusable(IsFocusable)
		;

	auto Slider = static_cast<SDONSlider*>(MySlider.Get());
	Slider->SetHoverSound(HoveredSlateSound);
	Slider->SetChangedSound(ChangedSlateSound);

	FSlateApplication& SlateApp = FSlateApplication::Get();

	SlateApp.OnFocusChanging().AddUObject(this, &UDONSlider::OnFocusChange);

	return MySlider.ToSharedRef();
}

void UDONSlider::SlateOnValueChanged_Good(float InValue)
{
	OnValueChanged.Broadcast(InValue);
	static_cast<SDONSlider*>(MySlider.Get())->PlayChangedSound();
}

void UDONSlider::OnFocusChange(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath, const TSharedPtr<SWidget>& Widget, const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& MyNewWidget)
{

	if (MySlider.IsValid())
	{
		if (MyNewWidget == MySlider) {  //GAIN FOCUS

			FocusedSliderStyle = GetWidgetStyle();
			FocusedSliderStyle.SetNormalBarImage(GetWidgetStyle().HoveredBarImage);
			FocusedSliderStyle.SetNormalThumbImage(GetWidgetStyle().HoveredThumbImage);
			MySlider->SetStyle(&FocusedSliderStyle);
			static_cast<SDONSlider*>(MySlider.Get())->PlayHoverSound();
		}
		else if (Widget == MySlider) {

			if (FocusEvent.GetCause() == EFocusCause::Mouse) {  //do not lose focus to mouse
				this->SetFocus();
			}
			else {
				MySlider->SetStyle(&GetWidgetStyle());
			}
		}
	}
}