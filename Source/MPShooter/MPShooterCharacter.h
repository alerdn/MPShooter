// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "MPShooterCharacter.generated.h"

class AGun;
class UHealthComponent;
class UInputAction;

UCLASS(config=Game)
class AMPShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

public:
	AMPShooterCharacter();
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Shoot();
	void Run(const FInputActionValue& Value);			
	void Aim(const FInputActionValue& Value);			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UHealthComponent* HealthComp;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Respawn();

	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() { return PlayerName; }
	UFUNCTION(BlueprintCallable)
	void SetPlayerName(FString NewPlayerName) { PlayerName = NewPlayerName; }

	UFUNCTION(Client, Reliable)
	void ClientRPCEnableInputs();
	UFUNCTION(Client, Reliable)
	void ClientRPCDisableInputs();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UPROPERTY(Replicated)
	FString PlayerName;

	UPROPERTY(EditAnywhere)
	float MaxRunSpeed;
	UPROPERTY(EditAnywhere)
	float MaxAimingWalkSpeed;
	UPROPERTY(EditAnywhere)
	float MaxAimingRunSpeed;

	float OriginalMaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	AGun* Gun;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float AimInterpSpeed;
	UPROPERTY(EditAnywhere)
	float AimArmLength;

	float OriginalArmLength;

	void HandleAim(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerRPCShoot();

	UFUNCTION(Server, Unreliable)
	void ServerRPCHandleWalkSpeed(float CurrentSpeed);
	UFUNCTION(Server, Unreliable)
	void ServerRPCHandleIsAiming(bool IsAiming);

};

