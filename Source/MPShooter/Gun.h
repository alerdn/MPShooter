#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class MPSHOOTER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	AGun();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Shoot();

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxRange;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate;

	FTimerHandle ShootCooldown;
	bool bCanFire;

	void AllowFire();

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Impact;

	bool GunTrace(FHitResult& HitResult, const AController *Controller,  FVector& ShotDirection);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnSoundAndParticles(FVector ImpactPoint, FVector ShotDirection);

};
