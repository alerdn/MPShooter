// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPShooterPlayerController.h"
#include "MPShooterGameMode.generated.h"

UCLASS(minimalapi)
class AMPShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ReloadLevel();

public:
	AMPShooterGameMode();

	UPROPERTY(BlueprintReadWrite)
	TArray<AMPShooterPlayerController*> PlayerList;

	UPROPERTY(BlueprintReadWrite)
	FString MapPath;

};



