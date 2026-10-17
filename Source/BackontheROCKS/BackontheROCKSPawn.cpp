#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "BOTRPlayerState.h"
#include "BOTRGameStateBase.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "BackontheROCKSPawn.h"
#include "BackontheROCKSWheelFront.h"
#include "BackontheROCKSWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ABackontheROCKSPawn::ABackontheROCKSPawn()
{
	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	AttackTraceOrigin =
		CreateDefaultSubobject<USceneComponent>(
			TEXT("AttackTraceOrigin")
		);

	AttackTraceOrigin->SetupAttachment(
		RootComponent
	);

}

void ABackontheROCKSPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ABackontheROCKSPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ABackontheROCKSPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ABackontheROCKSPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ABackontheROCKSPawn::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ABackontheROCKSPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ABackontheROCKSPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ABackontheROCKSPawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ABackontheROCKSPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ABackontheROCKSPawn::StopHandbrake);

		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ABackontheROCKSPawn::LookAround);

		// toggle camera 
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ABackontheROCKSPawn::ToggleCamera);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ABackontheROCKSPawn::ResetVehicle);

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABackontheROCKSPawn::Attack);
		}
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABackontheROCKSPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
	ApplySpeedLimit();
}

void ABackontheROCKSPawn::Steering(const FInputActionValue& Value)
{
	// get the input magnitude for steering
	float SteeringValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ABackontheROCKSPawn::Throttle(const FInputActionValue& Value)
{
	// get the input magnitude for the throttle
	float ThrottleValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void ABackontheROCKSPawn::Brake(const FInputActionValue& Value)
{
	// get the input magnitude for the brakes
	float BreakValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void ABackontheROCKSPawn::StartBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ABackontheROCKSPawn::StopBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ABackontheROCKSPawn::StartHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ABackontheROCKSPawn::StopHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void ABackontheROCKSPawn::LookAround(const FInputActionValue& Value)
{
	// get the flat angle value for the input 
	float LookValue = Value.Get<float>();

	// add the input
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
}

void ABackontheROCKSPawn::ToggleCamera(const FInputActionValue& Value)
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ABackontheROCKSPawn::ResetVehicle(const FInputActionValue& Value)
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

void ABackontheROCKSPawn::Attack()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ServerAttack();
}

void ABackontheROCKSPawn::ServerAttack_Implementation()
{
	if (!HasAuthority() ||
		!AttackTraceOrigin ||
		!GetController())
	{
		return;
	}

	const ABOTRGameStateBase* GS =
		GetWorld()->GetGameState<ABOTRGameStateBase>();

	if (!GS ||
		GS->CurrentState != EBOTRGameStateBase::Playing)
	{
		return;
	}

	const double Now = GetWorld()->GetTimeSeconds();

	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}

	LastAttackTime = Now;

	const FVector Start =
		AttackTraceOrigin->GetComponentLocation();

	const FVector End =
		Start +
		AttackTraceOrigin->GetForwardVector() * AttackRange;

	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(BOTRAttack),
		false,
		this
	);

	FHitResult HitResult;

	const bool bHit =
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_Visibility,
			Params
		);

	FVector TraceEnd = End;
	bool bVehicleHit = false;

	if (bHit)
	{
		TraceEnd = HitResult.ImpactPoint;

		ABackontheROCKSPawn* HitVehicle =
			Cast<ABackontheROCKSPawn>(
				HitResult.GetActor()
			);

		if (HitVehicle && HitVehicle != this)
		{
			bVehicleHit = true;

			UGameplayStatics::ApplyDamage(
				HitVehicle,
				1.0f,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}

	MulticastAttackFX(
		Start,
		TraceEnd,
		bVehicleHit,
		TraceEnd
	);
}

void ABackontheROCKSPawn::
MulticastAttackFX_Implementation(
	FVector Start,
	FVector End,
	bool bVehicleHit,
	FVector HitLocation
)
{
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor::Red,
		false,
		1.0f,
		0,
		3.0f
	);

	if (bVehicleHit && HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			HitLocation
		);
	}
}

float ABackontheROCKSPawn::TakeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	if (!HasAuthority() ||
		DamageAmount <= 0.0f ||
		!EventInstigator)
	{
		return 0.0f;
	}

	const ABOTRGameStateBase* GS =
		GetWorld()->GetGameState<ABOTRGameStateBase>();

	if (!GS ||
		GS->CurrentState != EBOTRGameStateBase::Playing)
	{
		return 0.0f;
	}

	AController* VictimController = GetController();

	if (!VictimController ||
		VictimController == EventInstigator)
	{
		return 0.0f;
	}

	ABOTRPlayerState* VictimPS =
		VictimController->GetPlayerState<ABOTRPlayerState>();

	ABOTRPlayerState* AttackerPS =
		EventInstigator->GetPlayerState<ABOTRPlayerState>();

	if (!VictimPS || !AttackerPS)
	{
		return 0.0f;
	}

	// 잔액이 5 미만이면 차감도 지급도 하지 않습니다.
	if (!VictimPS->RemoveCoins(5))
	{
		return 0.0f;
	}

	AttackerPS->AddCoins(5);

	return Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser
	);
}

bReplicates = true;
SetReplicateMovement(true);
SetCanBeDamaged(true);

// 기본값입니다. 실제 범퍼 위치는 차량 BP에서 조정합니다.
AttackTraceOrigin->SetRelativeLocation(
	FVector(250.0f, 0.0f, 60.0f)
);
void ABackontheROCKSPawn::ApplySpeedLimit()
{
	// 다른 차량의 표시용 복제본에서는 속도를 수정하지 않습니다.
	if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	const ABOTRPlayerState* PS =
		GetPlayerState<ABOTRPlayerState>();

	if (!PS ||
		!GetMesh() ||
		!GetMesh()->IsSimulatingPhysics())
	{
		return;
	}

	const float MaxSpeedKmh =
		FMath::Max(
			0.0f,
			BaseMaxSpeedKmh + PS->GetBonusMaxSpeed()
		);

	// km/h → cm/s
	const float MaxSpeedCmPerSecond =
		MaxSpeedKmh / 0.036f;

	FVector Velocity =
		GetMesh()->GetPhysicsLinearVelocity();

	const FVector HorizontalVelocity(
		Velocity.X,
		Velocity.Y,
		0.0f
	);

	const float HorizontalSpeed =
		HorizontalVelocity.Size();

	if (HorizontalSpeed > MaxSpeedCmPerSecond &&
		HorizontalSpeed > KINDA_SMALL_NUMBER)
	{
		const float Scale =
			MaxSpeedCmPerSecond / HorizontalSpeed;

		Velocity.X *= Scale;
		Velocity.Y *= Scale;

		// 점프·낙하 속도 Z는 유지합니다.
		GetMesh()->SetPhysicsLinearVelocity(Velocity);
	}
}
#undef LOCTEXT_NAMESPACE
