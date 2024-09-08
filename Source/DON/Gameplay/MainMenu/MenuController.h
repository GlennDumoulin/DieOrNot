// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

/**
 * 
 */
UCLASS()
class DON_API AMenuController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AMenuController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	bool IsReady() const { return bReady; }

	UFUNCTION(BlueprintCallable)
	void SetReady(bool ready);

	UFUNCTION(BlueprintCallable)
	void NextPlayerColor();

private:

	bool bReady;
};
