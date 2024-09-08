// Fill out your copyright notice in the Description page of Project Settings.


#include "DON/Gameplay/MainMenu/MenuGameMode.h"
#include "DON/Gameplay/DONGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DON/Gameplay/MainMenu/MenuController.h"
#include "DON/DONFunctionLibrary.h"
#include "DON/Actors/DONPlayerCharacter.h"
#include "DON/DONUserSettings.h"
#include "DON/DONNavigationConfig.h"

bool AMenuGameMode::CanStartGame()
{
	if (JoinedPlayers.IsEmpty()) return false;
	int ReadyCount = 0;
	for (auto& Player : JoinedPlayers) {
		if (Player->IsReady()) ReadyCount++;
	}
	return GIsEditor ? ReadyCount > 0 : ReadyCount > 1;
}

void AMenuGameMode::ClearPlayers()
{
	for (auto pla = GetWorld()->GetPlayerControllerIterator(); pla; ++pla) {
		bool Found = false;
		for (auto& [id, Platform] : GetGameInstance<UDONGameInstance>()->GetPlayers()) {
			if (Platform == pla->Get()->GetPlatformUserId()) {
				Found = true;
				break;;
			}
		}
		if (!Found) UGameplayStatics::RemovePlayer(pla->Get(), true);
	}
}

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	UDONUserSettings::GetDONUserSettings()->ApplySettings(true);
	FSlateApplication::Get().SetNavigationConfig(MakeShared<FDONNavigationConfig>());

	UDONFunctionLibrary::FadeScreen(0.5f, false);

	GetGameInstance<UDONGameInstance>()->ClearPlayers();
	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this, &AMenuGameMode::HandleNewPlayer);

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		TArray<FPlatformUserId> Users;
		IPlatformInputDeviceMapper::Get().GetAllActiveUsers(Users);

		for (auto& User : Users) {
			CreatePlayer(User);
		}
	});
}

void AMenuGameMode::HandleNewPlayer(EInputDeviceConnectionState NewConnectionState, FPlatformUserId PlatformUserId, FInputDeviceId Id)
{
	switch (NewConnectionState) {
	case EInputDeviceConnectionState::Connected: {
		CreatePlayer(PlatformUserId);
	} break;

	default: {
		if (PlatformUserId.GetInternalId() != 0) {
			AMenuController* Control = *(JoinedPlayers.FindByPredicate([PlatformUserId](AMenuController* Controller) { return Controller->GetPlatformUserId() == PlatformUserId; }));
			Control->GetPawn<ADONPlayerCharacter>()->TryKillPlayer(nullptr);
			JoinedPlayers.Remove(Control);
			GetGameInstance<UDONGameInstance>()->RemovePlayer(PlatformUserId);
		}
	}
	}
}

void AMenuGameMode::CreatePlayer(FPlatformUserId PlatformUserId)
{
	auto Player = UGameplayStatics::GetPlayerControllerFromPlatformUser(this, PlatformUserId);
	if (!Player)
		Player = UGameplayStatics::CreatePlayerFromPlatformUser(this, PlatformUserId);

	if (auto p = Cast<AMenuController>(Player); Player != nullptr) JoinPlayer(p);
	OnPlayerJoined.Broadcast(true, Cast<AMenuController>(Player));

	UE_LOG(LogTemp, Display, TEXT("Player created"));
}

void AMenuGameMode::JoinPlayer(AMenuController* Controller)
{
	if (!JoinedPlayers.ContainsByPredicate([Controller](AMenuController* c){ return c->GetPlatformUserId() == Controller->GetPlatformUserId(); }))
		JoinedPlayers.Add(Controller);
}

int AMenuGameMode::QueryConnectionState(int Player)
{
	auto& Players = GetGameInstance<UDONGameInstance>()->GetPlayers();
	if (Players.Find(Player) != nullptr && Cast<AMenuController>(UGameplayStatics::GetPlayerControllerFromPlatformUser(this, Players[Player]))->IsReady()) return 2;
	if (!JoinedPlayers.IsValidIndex(Player)) return 0;

	return 1;
}
