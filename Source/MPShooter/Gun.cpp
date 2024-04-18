#include "Gun.h"
#include "MPShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Damage = 20.f;
	MaxRange = 1500.f;
	FireRate = .1f;
	bCanFire = true;
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::Shoot()
{
	if (!bCanFire)
	{
		return;
	}
	bCanFire = false;

	AMPShooterCharacter *MyOwner = Cast<AMPShooterCharacter>(GetOwner());
	if (!MyOwner)
	{
		return;
	}

	AController *Controller = MyOwner->GetController();
	if (!Controller)
	{
		return;
	}
 
	// TODO: Play sounds and emit effects

	FVector ShotDirection;
	FHitResult HitResult;
	bool HasHit = GunTrace(HitResult, Controller, ShotDirection);
	if (HasHit)
	{
		AActor *HitActor = HitResult.GetActor();
		if (HitActor)
		{
			MulticastRPCSpawnSoundAndParticles(HitResult.ImpactPoint, ShotDirection);

			FString who = HasAuthority() ? "servidor" : "cliente";

			if (HitActor)
			{
				FPointDamageEvent DamageEvent(Damage, HitResult, ShotDirection, nullptr);
				HitActor->TakeDamage(Damage, DamageEvent, Controller, this);
			}
		}
	}
	
	GetWorldTimerManager().SetTimer(ShootCooldown, this, &AGun::AllowFire, FireRate);
}

bool AGun::GunTrace(FHitResult &HitResult, const AController *Controller, FVector& ShotDirection)
{
	FVector VPLocation;
	FRotator VPRotation;
	Controller->GetPlayerViewPoint(VPLocation, VPRotation);
	ShotDirection = -VPRotation.Vector();

	FVector End = VPLocation + VPRotation.Vector() * MaxRange;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Owner);

	return GetWorld()->LineTraceSingleByChannel(HitResult, VPLocation, End, ECC_GameTraceChannel1, Params);
}

void AGun::MulticastRPCSpawnSoundAndParticles_Implementation(FVector ImpactPoint, FVector ShotDirection)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Impact, ImpactPoint, ShotDirection.Rotation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, ImpactPoint);
}

void AGun::AllowFire()
{
	bCanFire = true;
}
