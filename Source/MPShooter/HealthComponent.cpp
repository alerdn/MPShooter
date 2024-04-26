#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "MPShooterCharacter.h"
#include "Gun.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MaxHealth = 100,
	Health = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

bool UHealthComponent::IsDead() const
{
	return Health == 0;
}

float UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, AController* KillerController, AActor *Weapon)
{
	Damage = FMath::Floor(Damage);
	if (Damage <= 0.f || IsDead())
	{
		return 0.f;
	}

	float DamageToApply = FMath::Min(Damage, Health);
	Health -= DamageToApply;

	if (IsDead())
	{
		AMPShooterCharacter *MyOwner = Cast<AMPShooterCharacter>(DamagedActor); 
		AMPShooterCharacter* Killer = Cast<AMPShooterCharacter>(KillerController->GetPawn());
		AGun* Gun = Cast<AGun>(Weapon);
		if (MyOwner->HasAuthority())
		{
			MyOwner->OnDead.Broadcast(MyOwner, Killer, Gun);
		}
	}

	return DamageToApply;
}
