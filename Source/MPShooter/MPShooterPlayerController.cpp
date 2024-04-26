#include "MPShooterPlayerController.h"
#include "MPShooterGameInstance.h"
#include "Net/UnrealNetwork.h"

void AMPShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if(IsLocalController())
	{
		UMPShooterGameInstance* GameInstance = Cast<UMPShooterGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			ServerRPCSetPlayerName(GameInstance->GetPlayerName());
		}
	}
}

void AMPShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPShooterPlayerController, PlayerName);
}

void AMPShooterPlayerController::IncreaseKills()
{
	Kills++;
}

void AMPShooterPlayerController::IncreaseDeaths()
{
	Deaths++;
}

void AMPShooterPlayerController::OnRep_PlayerName()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, *PlayerName);
	OnPlayerNameChanged.Broadcast();
}

void AMPShooterPlayerController::ServerRPCSetPlayerName_Implementation(const FString& CustomPlayerName)
{
	PlayerName = CustomPlayerName;
	OnRep_PlayerName();
}

