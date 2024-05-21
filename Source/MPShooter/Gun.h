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
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent *Mesh;

	virtual void Tick(float DeltaTime) override;

	void Shoot();
	void Reload();

	UFUNCTION(BlueprintPure)
	FString GetGunName() { return GunName; }
	UFUNCTION(BlueprintPure)
	int32 GetMaxAmmo() { return MaxAmmo; }
	UFUNCTION(BlueprintPure)
	int32 GetCurrentAmmo() { return CurrentAmmo; }
	UFUNCTION(BlueprintPure)
	bool HasCrosshair() { return bHasCrosshair; }
	UFUNCTION(BlueprintPure)
	bool HasCustomFOV() { return bHasCustomFOV; }
	UFUNCTION(BlueprintPure)
	float GetCustomFOV() { return CustomFOV; }

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent *RootComp;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FString GunName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxAmmo;
	int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ReloadSpeed;
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 DamageBase;
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 DamageVariation;
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxRange;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate;
	
	UPROPERTY(EditAnywhere)
	bool bHasCrosshair;
	UPROPERTY(EditAnywhere)
	bool bHasCustomFOV;
	UPROPERTY(EditAnywhere)
	float CustomFOV;

	FTimerHandle ShootCooldown;
	bool bCanFire;

	void AllowFire();
	bool HasAmmo() { return CurrentAmmo > 0; }
	
	FTimerHandle ReloadDelay;
	bool bReloading;
	void ReloadAmmo();

	UPROPERTY(EditAnywhere)
	UParticleSystem *MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundBase *MuzzleSound;

	UPROPERTY(EditAnywhere)
	USoundBase *ImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem *Impact;

	bool GunTrace(FHitResult &HitResult, const AController *Controller, FVector &ShotDirection);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnSoundAndParticles(FVector ImpactPoint, FVector ShotDirection);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnMuzzleSoundAndParticles();
};
