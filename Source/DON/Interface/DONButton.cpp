// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Interface/DONButton.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ButtonSlot.h"

TSharedRef<SWidget> UDONButton::RebuildWidget()
{
	MyButton = SNew(SDONButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked_Good))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed_Good))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased_Good))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered_Good)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered_Good)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}


	FSlateApplication& SlateApp = FSlateApplication::Get();

	SlateApp.OnFocusChanging().AddUObject(this, &UDONButton::OnFocusChange);

	return MyButton.ToSharedRef();
}


FReply UDONButton::SlateHandleClicked_Good()
{
	OnClicked.Broadcast();

	return FReply::Handled();
}

void UDONButton::SlateHandlePressed_Good()
{
	OnPressed.Broadcast();
}

void UDONButton::SlateHandleReleased_Good()
{
	OnReleased.Broadcast();
}

void UDONButton::SlateHandleHovered_Good()
{
	this->SetFocus();
	OnHovered.Broadcast();
}

void UDONButton::SlateHandleUnhovered_Good()
{
	OnUnhovered.Broadcast();
}

void UDONButton::OnFocusChange(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath, const TSharedPtr<SWidget>& Widget, const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& MyNewWidget)
{

	if (MyButton.IsValid())
	{
		if (MyNewWidget == MyButton) {  //GAIN FOCUS

			OnHovered.Broadcast();
			FocusedButtonStyle = WidgetStyle;
			FocusedButtonStyle.SetNormal(WidgetStyle.Hovered);
			MyButton->SetButtonStyle(&FocusedButtonStyle);
			static_cast<SDONButton*>(MyButton.Get())->PlaySounds();
		}
		else if (Widget == MyButton) {

			if (FocusEvent.GetCause() == EFocusCause::Mouse) {  //do not lose focus to mouse
				this->SetFocus();
			}
			else {
				OnUnhovered.Broadcast();
				MyButton->SetButtonStyle(&WidgetStyle);
			}
		}
	}
}

void SDONButton::PlaySounds()
{
	PlayHoverSound();
}
