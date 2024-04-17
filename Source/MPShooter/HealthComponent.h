// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MPSHOOTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float DamageTaken(AActor *DamagedActor, float Damage, AActor *DamageCauser);
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealth() const { return Health; };
	void SetHealth(float NewHealth) { Health = NewHealth; };

	UFUNCTION(BlueprintPure)
	float GetHealthPercentage() const { return Health / MaxHealth; };

private:
	UPROPERTY(EditAnywhere)
	float MaxHealth;
	UPROPERTY(Replicated, VisibleAnywhere)
	float Health;

	FTimerHandle ReviveTimer;

	void Revive();

};
