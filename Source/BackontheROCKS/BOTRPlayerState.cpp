#include "BOTRPlayerState.h"
#include "Net/UnrealNetwork.h"

ABOTRPlayerState::ABOTRPlayerState()
{
    Coins = 0;
    BonusMaxSpeed = 0.0f;
}

void ABOTRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION
    (AAssignmentPlayerState, Coins, COND_OwnerOnly);

    DOREPLIFETIME_CONDITION
    (AAssignmentPlayerState, BonusMaxSpeed, COND_OwnerOnly);
}

void ABOTRPlayerState::AddCoins(int32 Amount)
{
    if (!HasAuthority())
    {
        return;
    }

    if (Amount <= 0)
    {
        return;
    }

    Coins += Amount;
}

bool ABOTRPlayerState::RemoveCoins(int32 Amount)
{
    if (!HasAuthority())
    {
        return false;
    }

    if (Amount <= 0 || Coins < Amount)
    {
        return false;
    }

    Coins -= Amount;

    return true;
}

void ABOTRPlayerState::AddBonusMaxSpeed(float Amount)
{
    if (!HasAuthority())
    {
        return;
    }

    if (Amount <= 0.0f)
    {
        return;
    }

    BonusMaxSpeed += Amount;
}

void ABOTRPlayerState::OnRep_Coins()
{
}

void ABOTRPlayerState::OnRep_BonusMaxSpeed()
{
}