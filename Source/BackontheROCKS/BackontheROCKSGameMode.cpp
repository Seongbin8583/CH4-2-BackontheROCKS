#include "BOTRPlayerState.h"
#include "BOTRGameStateBase.h"
#include "TimerManager.h"
#include "BackontheROCKSGameMode.h"
#include "BackontheROCKSPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ABackontheROCKSGameMode::ABackontheROCKSGameMode()
{
	PlayerControllerClass = ABackontheROCKSPlayerController::StaticClass();

	GameStateClass =
		ABOTRGameStateBase::StaticClass();

	PlayerStateClass =
		ABOTRPlayerState::StaticClass();
}

AActor* ABackontheROCKSGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> AvailablePlayerStarts;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (IsPlayerStartAvailable(PlayerStart))
		{
			AvailablePlayerStarts.Add(PlayerStart);
		}
	}

	checkf(AvailablePlayerStarts.IsEmpty() == false, TEXT("There is no available Vehicle PlayerStart."));

	const int32 RandomIndex = FMath::RandHelper(AvailablePlayerStarts.Num());
	return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

bool ABackontheROCKSGameMode::IsPlayerStartAvailable(const APlayerStart* PlayerStart) const
{
	if (IsValid(PlayerStart) == false || IsValid(GetWorld()) == false)
	{
		return false;
	}

	const FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
	const FVector CheckLocation = PlayerStartTransform.TransformPosition(VehicleSpawnCheckOffset);
	const FQuat CheckRotation = PlayerStartTransform.GetRotation();
	const FCollisionShape CheckShape = FCollisionShape::MakeBox(VehicleSpawnCheckHalfExtent);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehiclePlayerStartCheck), false);

	const bool bVehicleExists =
		GetWorld()->OverlapAnyTestByObjectType(
			CheckLocation,
			CheckRotation,
			ObjectQueryParams,
			CheckShape,
			QueryParams
		);

	return !bVehicleExists;
}

void ABackontheROCKSGameMode::PostLogin(
    APlayerController* NewPlayer
)
{
    Super::PostLogin(NewPlayer);

    if (!bCountdownStarted)
    {
        bCountdownStarted = true;

        StartWaiting();
    }
}

void ABackontheROCKSGameMode::StartWaiting()
{
    ABOTRGameStateBase* GS =
        GetGameState<ABOTRGameStateBase>();

    if (!GS)
    {
        return;
    }

    GS->SetCurrentState(
        EBOTRGameStateBase::Waiting
    );

    GS->SetRemainingTime(
        WaitingDuration
    );

    GetWorldTimerManager().SetTimer(
        GameTimerHandle,
        this,
        &ABackontheROCKSGameMode::UpdateGameTimer,
        1.0f,
        true
    );
}

void ABackontheROCKSGameMode::UpdateGameTimer()
{
    ABOTRGameStateBase* GS =
        GetGameState<ABOTRGameStateBase>();

    if (!GS)
    {
        return;
    }

    GS->SetRemainingTime(
        GS->RemainingTime - 1
    );

    if (GS->RemainingTime > 0)
    {
        return;
    }

    if (GS->CurrentState ==
        EBOTRGameStateBase::Waiting)
    {
        StartPlaying();
    }
    else if (
        GS->CurrentState ==
        EBOTRGameStateBase::Playing)
    {
        FinishGame();
    }
}

void ABackontheROCKSGameMode::StartPlaying()
{
    ABOTRGameStateBase* GS =
        GetGameState<ABOTRGameStateBase>();

    if (!GS)
    {
        return;
    }

    GS->SetCurrentState(
        EBOTRGameStateBase::Playing
    );

    GS->SetRemainingTime(
        PlayingDuration
    );
}

void ABackontheROCKSGameMode::FinishGame()
{
    GetWorldTimerManager().ClearTimer(
        GameTimerHandle
    );

    ABOTRGameStateBase* GS =
        GetGameState<ABOTRGameStateBase>();

    if (!GS)
    {
        return;
    }

    int32 HighestCoins = -1;

    TArray<APlayerState*> WinnerList;

    for (APlayerState* PlayerState : GS->PlayerArray)
    {
        ABOTRPlayerState* BOTRPS =
            Cast<ABOTRPlayerState>(
                PlayerState
            );

        if (!BOTRPS)
        {
            continue;
        }

        const int32 Coins =
            BOTRPS->GetCoins();

        if (Coins > HighestCoins)
        {
            HighestCoins = Coins;

            WinnerList.Reset();
            WinnerList.Add(BOTRPS);
        }
        else if (Coins == HighestCoins)
        {
            WinnerList.Add(BOTRPS);
        }
    }

    GS->SetWinners(WinnerList);

    GS->SetRemainingTime(0);

    GS->SetCurrentState(
        EBOTRGameStateBase::Finished
    );
}