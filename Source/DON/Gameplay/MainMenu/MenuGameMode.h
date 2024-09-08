// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

class AMenuController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerJoined, bool, Joined, AMenuController*, Player);

UCLASS()
class DON_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	bool CanStartGame();

	UFUNCTION(BlueprintCallable)
		void ClearPlayers();

	UPROPERTY(BlueprintAssignable, Category = Gamepad)
	FPlayerJoined OnPlayerJoined;

	virtual void BeginPlay() override;

	void HandleNewPlayer(EInputDeviceConnectionState NewConnectionState, FPlatformUserId PlatformUserId, FInputDeviceId Id);

	void CreatePlayer(FPlatformUserId PlatformUserId);

	void JoinPlayer(AMenuController* Controller);

	UFUNCTION(BlueprintCallable)
	int QueryConnectionState(int Player);

	UFUNCTION(BlueprintCallable)
	AMenuController* GetPlayer(int Player) const { return JoinedPlayers[Player]; }

private:

	TArray<AMenuController*> JoinedPlayers;
};
