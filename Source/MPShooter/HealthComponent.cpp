#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "MPShooterCharacter.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MaxHealth = 100.f,
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

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

bool UHealthComponent::IsDead() const
{
	return Health == 0;
}

float UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, AActor *DamageCauser)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s recebeu dano de %s"), *DamagedActor->GetActorNameOrLabel(), *DamageCauser->GetActorNameOrLabel()));
	if (Damage <= 0.f)
	{
		return 0.f;
	}

	float DamageToApply = FMath::Min(Damage, Health);
	Health -= DamageToApply;

	if (IsDead())
	{
		
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s morto por %s"), *DamagedActor->GetActorNameOrLabel(), *DamageCauser->GetActorNameOrLabel()));
		AMPShooterCharacter* MyOwner = Cast<AMPShooterCharacter>(GetOwner());
		MyOwner->GetController()->SetIgnoreMoveInput(true);

		MyOwner->GetWorldTimerManager().SetTimer(ReviveTimer, this, &UHealthComponent::Revive, 5.f);
	}

	return DamageToApply;
}


void UHealthComponent::Revive()
{
	AMPShooterCharacter* MyOwner = Cast<AMPShooterCharacter>(GetOwner());
	MyOwner->GetController()->SetIgnoreMoveInput(false);
	Health = MaxHealth;
}