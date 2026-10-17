#include "BOTRGameStateBase.h"
#include "Net/UnrealNetwork.h"

ABOTRGameStateBase::ABOTRGameStateBase()
{
    CurrentState = EBOTRGameStateBase::Waiting;
    RemainingTime = 0;
}

void ABOTRGameStateBase::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME
    (ABOTRGameStateBase, CurrentState);

    DOREPLIFETIME
    (ABOTRGameStateBase, RemainingTime);

    DOREPLIFETIME
    (ABOTRGameStateBase, Winners);
}

void ABOTRGameStateBase::SetCurrentState(
    EBOTRGameStateBase NewState
)
{
    if (!HasAuthority())
    {
        return;
    }

    CurrentState = NewState;
}

void ABOTRGameStateBase::SetRemainingTime(
    int32 NewTime
)
{
    if (!HasAuthority())
    {
        return;
    }

    RemainingTime = FMath::Max(NewTime, 0);
}

void ABOTRGameStateBase::SetWinners(
    const TArray<APlayerState*>& NewWinners
)
{
    if (!HasAuthority())
    {
        return;
    }

    Winners = NewWinners;
}
