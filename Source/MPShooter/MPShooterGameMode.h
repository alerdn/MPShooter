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

public:
	AMPShooterGameMode();

	UPROPERTY(BlueprintReadWrite)
	TArray<AMPShooterPlayerController*> PlayerList;

};



