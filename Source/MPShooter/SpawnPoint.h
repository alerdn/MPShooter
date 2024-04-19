#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class MPSHOOTER_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnPoint();

	FVector GetSpawnLocation();
	FRotator GetSpawnRotation();

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;
};
