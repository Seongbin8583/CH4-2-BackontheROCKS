#include "BackontheROCKSPlayerController.h"
#include "GameHUDWidget.h"

#include "BackontheROCKSPawn.h"
#include "BackontheROCKSUI.h"
#include "chatUI.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "InputCoreTypes.h"

void ABackontheROCKSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (!IsLocalController())
    {
        return;
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
            GameHUDWidget->AddToViewport(1);
        }
    }

    if (VehicleUIClass)
    {
        VehicleUI =
            CreateWidget<UBackontheROCKSUI>(
                this,
                VehicleUIClass
            );

        if (VehicleUI)
        {
            VehicleUI->AddToViewport(0);
        }
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
            ChatWidget->AddToViewport(2);
        }
    }

    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}

void ABackontheROCKSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!IsLocalController())
    {
        return;
    }

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<
            UEnhancedInputLocalPlayerSubsystem
            >(LP))
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(
                    InputMappingContext,
                    0
                );
            }

            if (bUseSteeringWheelControls &&
                SteeringWheelInputMappingContext)
            {
                Subsystem->AddMappingContext(
                    SteeringWheelInputMappingContext,
                    1
                );
            }
        }
    }

    if (InputComponent)
    {
        InputComponent->BindKey(
            EKeys::Enter,
            IE_Pressed,
            this,
            &ABackontheROCKSPlayerController::OpenChat
        );
    }
}

void ABackontheROCKSPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!IsLocalController() || !VehicleUI)
    {
        return;
    }

    // 서버 OnPossess에서 저장한 포인터에 의존하지 않고,
    // 로컬 컨트롤러의 현재 폰을 조회합니다.
    const ABackontheROCKSPawn* Vehicle =
        Cast<ABackontheROCKSPawn>(GetPawn());

    if (!Vehicle)
    {
        return;
    }

    const UChaosWheeledVehicleMovementComponent* Movement =
        Vehicle->GetChaosVehicleMovement();

    if (Movement)
    {
        VehicleUI->UpdateSpeed(Movement->GetForwardSpeed());
        VehicleUI->UpdateGear(Movement->GetCurrentGear());
    }
}

void ABackontheROCKSPlayerController::OpenChat()
{
    if (ChatWidget)
    {
        ChatWidget->FocusChat();
    }
}

void ABackontheROCKSPlayerController::ServerSendChat_Implementation(
    const FString& Message
)
{
    if (!PlayerState || !GetWorld())
    {
        return;
    }

    FString SafeMessage =
        Message.Left(120).TrimStartAndEnd();

    SafeMessage.ReplaceInline(TEXT("\r"), TEXT(" "));
    SafeMessage.ReplaceInline(TEXT("\n"), TEXT(" "));

    if (SafeMessage.IsEmpty())
    {
        return;
    }

    const double Now = GetWorld()->GetTimeSeconds();

    // 서버에서 전송 간격 제한
    if (Now - LastChatTime < 0.3)
    {
        return;
    }

    LastChatTime = Now;

    const FString FinalMessage = FString::Printf(
        TEXT("%s: %s"),
        *PlayerState->GetPlayerName(),
        *SafeMessage
    );

    // 서버에는 접속한 모든 PlayerController가 있습니다.
    // 각 컨트롤러의 소유 클라이언트로 메시지를 전송합니다.
    for (
        FConstPlayerControllerIterator It =
        GetWorld()->GetPlayerControllerIterator();
        It;
        ++It
        )
    {
        if (ABackontheROCKSPlayerController* PC =
            Cast<ABackontheROCKSPlayerController>(It->Get()))
        {
            PC->ClientReceiveChat(FinalMessage);
        }
    }
}

void ABackontheROCKSPlayerController::ClientReceiveChat_Implementation(
    const FString& Message
)
{
    if (ChatWidget)
    {
        ChatWidget->AddChatMessage(Message);
    }
}