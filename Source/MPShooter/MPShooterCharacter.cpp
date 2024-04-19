// Copyright Epic Games, Inc. All Rights Reserved.

#include "MPShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Gun.h"
#include "HealthComponent.h"
#include "MPShooterGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"

//////////////////////////////////////////////////////////////////////////
// AMPShooterCharacter

AMPShooterCharacter::AMPShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Speeds
	MaxRunSpeed = 1500.f;
	OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxAimingWalkSpeed = OriginalMaxWalkSpeed * .4f;
	MaxAimingRunSpeed = MaxRunSpeed * .4f;

	// Aim
	AimInterpSpeed = 3500.f;
	AimArmLength = 80.f;
	OriginalArmLength = CameraBoom->TargetArmLength;
}

void AMPShooterCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	Respawn();

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
}

void AMPShooterCharacter::Tick(float DeltaTime)
{
	HandleSpeed();
	HandleAim(DeltaTime);
}

void AMPShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPShooterCharacter, bRunning);
	DOREPLIFETIME(AMPShooterCharacter, bAiming);
}

void AMPShooterCharacter::Respawn()
{
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(this, ASpawnPoint::StaticClass(), SpawnPoints);
	if (SpawnPoints.Num() > 0)
	{
		int spawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		ASpawnPoint* SpawnPoint = Cast<ASpawnPoint>(SpawnPoints[spawnIndex]);
		if (SpawnPoint)
		{
			SetActorLocation(SpawnPoint->GetSpawnLocation());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// Input

void AMPShooterCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMPShooterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMPShooterCharacter::Look);

		// Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AMPShooterCharacter::Shoot);

		// Run
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AMPShooterCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMPShooterCharacter::Run);
		
		// Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMPShooterCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMPShooterCharacter::Aim);
	}
}

void AMPShooterCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMPShooterCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMPShooterCharacter::Shoot()
{
	if (HasAuthority())
	{
		Gun->Shoot();
	}
	else
	{
		ServerRPCShoot();
	}
}

void AMPShooterCharacter::ServerRPCShoot_Implementation()
{
	Gun->Shoot();
}

void AMPShooterCharacter::Run(const FInputActionValue &Value)
{
	bRunning = Value.Get<bool>();
}

void AMPShooterCharacter::Aim(const FInputActionValue &Value)
{
	bAiming = Value.Get<bool>();
}

void AMPShooterCharacter::HandleSpeed()
{
	if (bRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = bAiming ? MaxAimingRunSpeed : MaxRunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = bAiming ? MaxAimingWalkSpeed : OriginalMaxWalkSpeed;
	}
}

void AMPShooterCharacter::HandleAim(float DeltaTime)
{
	if (bAiming)
	{
		float ArmLength = FMath::FInterpConstantTo(CameraBoom->TargetArmLength, AimArmLength, DeltaTime, AimInterpSpeed);
		CameraBoom->TargetArmLength = ArmLength;
	}
	else 
	{
		float ArmLength = FMath::FInterpConstantTo(CameraBoom->TargetArmLength, OriginalArmLength, DeltaTime, AimInterpSpeed);
		CameraBoom->TargetArmLength = ArmLength;
	}
}

float AMPShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return HealthComp->DamageTaken(this, DamageAmount, DamageCauser);
}

void AMPShooterCharacter::ClientRPCEnableInputs_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

void AMPShooterCharacter::ClientRPCDisableInputs_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeUIOnly());
	}
}