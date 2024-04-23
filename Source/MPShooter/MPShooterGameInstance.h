// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MPShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTER_API UMPShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FString GetRandomName();

	UFUNCTION(BlueprintCallable)
	FString GetPlayerName();
	UFUNCTION(BlueprintCallable)
	void SetPlayerName(FString NewPlayerName) { PlayerName = NewPlayerName; }

private:
	FString PlayerName;

	TArray<FString> DummyNames = { 
		TEXT("Zumbi"), 
		TEXT("Backend"), 
		TEXT("Frontend"),
		TEXT("QA"),
		TEXT("Head"),
	};
	TArray<FString> DummyAdjectives = {
		TEXT("Jr"),
		TEXT("Sênior"),
		TEXT("Anônimo"),
		TEXT("Energético"),
		TEXT("Calvo"),
		TEXT("Careca"),
	};
	
};
