// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartGameZone.generated.h"

class UBoxComponent;
class UTextRenderComponent;

UCLASS()
class DON_API AStartGameZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStartGameZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> BoxCollider;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float StartGameTime;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	int RequiredPlayerAmount;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	int CurrentPlayerAmount;

	UFUNCTION(BlueprintImplementableEvent)
	void HandleStartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTimerStart();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTimerEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void SetText(const FString& NewText);

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void UpdateText();

	float ElapsedStartGameTime;

	bool bIsTimerEnabled;

	bool bHasGameStarted;

	bool bPrevCanStartGame;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
