#include "Gun.h"
#include "MPShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Components/ArrowComponent.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);

	DamageBase = 10;
	DamageVariation = 10;
	MaxRange = 1500;
	FireRate = .1f;
	ReloadSpeed = 1.f;
	bCanFire = true;
	bHasCrosshair = true;
}

void AGun::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::Shoot()
{
	if (!HasAmmo())
	{
		return;
	}

	if (!bCanFire || bReloading)
	{
		return;
	}
	CurrentAmmo--;
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

	MulticastRPCSpawnMuzzleSoundAndParticles();

	FVector ShotDirection;
	FHitResult HitResult;
	bool HasHit = GunTrace(HitResult, Controller, ShotDirection);
	if (HasHit)
	{
		AActor *HitActor = HitResult.GetActor();
		if (HitActor)
		{
			MulticastRPCSpawnSoundAndParticles(HitResult.ImpactPoint, ShotDirection);

			if (HitActor)
			{
				float Damage = DamageBase + FMath::RandRange(0, DamageVariation);
				FPointDamageEvent DamageEvent(Damage, HitResult, ShotDirection, nullptr);
				HitActor->TakeDamage(Damage, DamageEvent, Controller, this);
			}
		}
	}

	GetWorldTimerManager().SetTimer(ShootCooldown, this, &AGun::AllowFire, FireRate);
}

bool AGun::GunTrace(FHitResult &HitResult, const AController *Controller, FVector &ShotDirection)
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

void AGun::AllowFire()
{
	bCanFire = true;
}

void AGun::Reload()
{
	if (bReloading)
	{
		return;
	}

	bReloading = true;
	GetWorldTimerManager().SetTimer(ReloadDelay, this, &AGun::ReloadAmmo, ReloadSpeed);
}

void AGun::ReloadAmmo()
{
	bReloading = false;
	CurrentAmmo = MaxAmmo;
}

void AGun::MulticastRPCSpawnSoundAndParticles_Implementation(FVector ImpactPoint, FVector ShotDirection)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Impact, ImpactPoint, ShotDirection.Rotation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, ImpactPoint);
}

void AGun::MulticastRPCSpawnMuzzleSoundAndParticles_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlash"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlash"));
}
