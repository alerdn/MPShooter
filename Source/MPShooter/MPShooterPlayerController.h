#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MPShooterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMPShooterPlayerControllerOnPlayerNameChanged);

UCLASS()
class MPSHOOTER_API AMPShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FMPShooterPlayerControllerOnPlayerNameChanged OnPlayerNameChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void IncreaseKills();
	UFUNCTION(BlueprintCallable)
	void IncreaseDeaths();

	UFUNCTION(BlueprintPure)
	int32 GetKillsCount() { return Kills; };
	UFUNCTION(BlueprintPure)
	int32 GetDeathsCount() { return Deaths; };

	UFUNCTION(BlueprintPure)
	FString GetPlayerName() { return PlayerName; }
	

private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerName)
	FString PlayerName;

	int32 Kills;
	int32 Deaths;

	UFUNCTION()
	void OnRep_PlayerName();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetPlayerName(const FString& CustomPlayerName);
};
