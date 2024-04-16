// Copyright Epic Games, Inc. All Rights Reserved.

#include "MPShooterGameMode.h"
#include "MPShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMPShooterGameMode::AMPShooterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
