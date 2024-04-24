#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class UArrowComponent;

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

	UFUNCTION(BlueprintPure)
	FString GetGunName() { return GunName; }
	UFUNCTION(BlueprintCallable)
	void SetGunName(FString NewGunName) { GunName = NewGunName; }

private:
	UPROPERTY(EditAnywhere)
	FString GunName;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* MuzzlePoint;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamageBase;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamageVariation;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxRange;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate;

	FTimerHandle ShootCooldown;
	bool bCanFire;

	void AllowFire();

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere)
	USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Impact;

	bool GunTrace(FHitResult& HitResult, const AController *Controller,  FVector& ShotDirection);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnSoundAndParticles(FVector ImpactPoint, FVector ShotDirection);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnMuzzleSoundAndParticles();

};
