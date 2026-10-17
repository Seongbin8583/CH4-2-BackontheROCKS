#include "ChatWidget.h"
#include "GameHUDWidget.h"
#include "BackontheROCKSPlayerController.h"
#include "BackontheROCKSPawn.h"
#include "BackontheROCKSUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"


void ABackontheROCKSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}

	if (ChatWidgetClass)
	{
		ChatWidget =
			CreateWidget<UChatWidget>(
				this,
				ChatWidgetClass
			);

		if (ChatWidget)
		{
			ChatWidget->AddToViewport();
		}
	}

	if (GameHUDWidgetClass)
	{
		GameHUDWidget =
			CreateWidget<UGameHUDWidget>(
				this,
				GameHUDWidgetClass
			);

		if (GameHUDWidget)
		{
			GameHUDWidget->AddToViewport();
		}
	}
}

void ABackontheROCKSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void ABackontheROCKSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, VehiclePawn);
}

void ABackontheROCKSPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void ABackontheROCKSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<ABackontheROCKSPawn>(InPawn);
}

void ABackontheROCKSPlayerController::ServerSendChat_Implementation(
	const FString& Message
)
{
	FString SafeMessage =
		Message.TrimStartAndEnd();

	if (SafeMessage.IsEmpty())
	{
		return;
	}

	SafeMessage = SafeMessage.Left(120);

	FString PlayerName = TEXT("Player");

	if (PlayerState)
	{
		PlayerName =
			PlayerState->GetPlayerName();
	}

	const FString FinalMessage =
		FString::Printf(
			TEXT("%s: %s"),
			*PlayerName,
			*SafeMessage
		);

	for (
		FConstPlayerControllerIterator It =
		GetWorld()->GetPlayerControllerIterator();
		It;
		++It
		)
	{
		ABackontheROCKSPlayerController* PC =
			Cast<ABackontheROCKSPlayerController>(
				It->Get()
			);

		if (PC)
		{
			PC->ClientReceiveChat(
				FinalMessage
			);
		}
	}
}