#include "GameHUDWidget.h"

#include "BOTRPlayerState.h"
#include "BOTRGameStateBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

FText UGameHUDWidget::GetCoinsText() const
{
    const APlayerController* PC = GetOwningPlayer();

    const ABOTRPlayerState* PS =
        PC ? PC->GetPlayerState<ABOTRPlayerState>() : nullptr;

    if (!PS)
    {
        return FText::FromString(TEXT("Coins: --"));
    }

    return FText::FromString(
        FString::Printf(TEXT("Coins: %d"), PS->GetCoins())
    );
}

FText UGameHUDWidget::GetBonusSpeedText() const
{
    const APlayerController* PC = GetOwningPlayer();

    const ABOTRPlayerState* PS =
        PC ? PC->GetPlayerState<ABOTRPlayerState>() : nullptr;

    if (!PS)
    {
        return FText::FromString(TEXT("Bonus Max Speed: --"));
    }

    return FText::FromString(
        FString::Printf(
            TEXT("Bonus Max Speed: +%.0f km/h"),
            PS->GetBonusMaxSpeed()
        )
    );
}

FText UGameHUDWidget::GetMatchText() const
{
    const ABOTRGameStateBase* GS =
        GetWorld()
        ? GetWorld()->GetGameState<ABOTRGameStateBase>()
        : nullptr;

    if (!GS)
    {
        return FText::FromString(TEXT("Connecting..."));
    }

    FString StateName;

    switch (GS->CurrentState)
    {
    case EBOTRGameStateBase::Waiting:
        StateName = TEXT("Waiting");
        break;

    case EBOTRGameStateBase::Playing:
        StateName = TEXT("Playing");
        break;

    case EBOTRGameStateBase::Finished:
        StateName = TEXT("Finished");
        break;

    default:
        StateName = TEXT("Unknown");
        break;
    }

    return FText::FromString(
        FString::Printf(
            TEXT("%s | %d sec"),
            *StateName,
            GS->RemainingTime
        )
    );
}

FText UGameHUDWidget::GetResultText() const
{
    const ABOTRGameStateBase* GS =
        GetWorld()
        ? GetWorld()->GetGameState<ABOTRGameStateBase>()
        : nullptr;

    const APlayerController* PC = GetOwningPlayer();

    const ABOTRPlayerState* MyPS =
        PC ? PC->GetPlayerState<ABOTRPlayerState>() : nullptr;

    if (!GS || !MyPS ||
        GS->CurrentState != EBOTRGameStateBase::Finished)
    {
        return FText::GetEmpty();
    }

    if (GS->Winners.IsEmpty())
    {
        return FText::FromString(TEXT("Checking result..."));
    }

    for (const APlayerState* Winner : GS->Winners)
    {
        if (!IsValid(Winner))
        {
            return FText::FromString(TEXT("Checking result..."));
        }
    }

    for (const APlayerState* Winner : GS->Winners)
    {
        if (Winner == MyPS)
        {
            return FText::FromString(
                GS->Winners.Num() > 1
                ? TEXT("JOINT WIN!")
                : TEXT("YOU WIN!")
            );
        }
    }

    return FText::FromString(TEXT("YOU LOSE"));
}