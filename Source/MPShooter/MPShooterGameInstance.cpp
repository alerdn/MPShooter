// Fill out your copyright notice in the Description page of Project Settings.


#include "MPShooterGameInstance.h"

FString UMPShooterGameInstance::GetRandomName()
{
    FString Name = DummyNames[FMath::RandRange(0, DummyNames.Num() - 1)];
    FString Adjective = DummyAdjectives[FMath::RandRange(0, DummyAdjectives.Num() - 1)];
    return FString::Printf(TEXT("%s %s"), *Name, *Adjective);
}

FString UMPShooterGameInstance::GetPlayerName()
{
    if (PlayerName.IsEmpty())
    {
        SetPlayerName(GetRandomName());
    }

    return PlayerName; 
}

