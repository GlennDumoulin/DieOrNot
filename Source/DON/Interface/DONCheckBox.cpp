// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Interface/DONCheckBox.h"

TSharedRef<SWidget> UDONCheckBox::RebuildWidget()
{
	MyCheckbox = SNew(SDONCheckBox)
		.OnCheckStateChanged(BIND_UOBJECT_DELEGATE(FOnCheckStateChanged, SlateOnCheckStateChangedCallback_Good))
		.Style(&GetWidgetStyle())
		.ClickMethod(GetClickMethod())
		.TouchMethod(GetTouchMethod())
		.PressMethod(GetPressMethod())
		.IsFocusable(IsFocusable)
		;


	if (GetChildrenCount() > 0)
	{
		MyCheckbox->SetContent(GetContentSlot()->Content ? GetContentSlot()->Content->TakeWidget() : SNullWidget::NullWidget);
	}

	FSlateApplication& SlateApp = FSlateApplication::Get();

	SlateApp.OnFocusChanging().AddUObject(this, &UDONCheckBox::OnFocusChange);

	return MyCheckbox.ToSharedRef();
}


void UDONCheckBox::SlateOnCheckStateChangedCallback_Good(ECheckBoxState NewState)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
	{
		OnCheckStateChanged.Broadcast(false);
		break;
	}
	case ECheckBoxState::Checked:
	{
		OnCheckStateChanged.Broadcast(true);
		break;
	}
	case ECheckBoxState::Undetermined:
	default:
		break;
	}
}

void UDONCheckBox::OnFocusChange(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath, const TSharedPtr<SWidget>& Widget, const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& MyNewWidget)
{

	if (MyCheckbox.IsValid())
	{
		if (MyNewWidget == MyCheckbox) {  //GAIN FOCUS

			FocusedCheckBoxStyle = GetWidgetStyle();
			FocusedCheckBoxStyle.SetCheckedImage(GetWidgetStyle().CheckedHoveredImage);
			FocusedCheckBoxStyle.SetUncheckedImage(GetWidgetStyle().UncheckedHoveredImage);
			MyCheckbox->SetStyle(&FocusedCheckBoxStyle);
			static_cast<SDONCheckBox*>(MyCheckbox.Get())->PlaySounds();

		}
		else if (Widget == MyCheckbox) {

			if (FocusEvent.GetCause() == EFocusCause::Mouse) {  //do not lose focus to mouse
				this->SetFocus();
			}
			else {
				MyCheckbox->SetStyle(&GetWidgetStyle());
			}
		}
	}
}

void SDONCheckBox::PlaySounds()
{
	PlayHoverSound();
}
