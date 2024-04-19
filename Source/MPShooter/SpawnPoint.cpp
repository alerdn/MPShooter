#include "SpawnPoint.h"
#include "Components/ArrowComponent.h"

ASpawnPoint::ASpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComp);
}

FVector ASpawnPoint::GetSpawnLocation()
{
	return GetActorLocation();
}

FRotator ASpawnPoint::GetSpawnRotation()
{
	return GetActorRotation();
}
