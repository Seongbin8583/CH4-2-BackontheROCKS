#include "BOTRCoin.h"
#include "BOTRPlayerState.h"

bool ABOTRCoin::GiveReward(ABOTRPlayerState* PS)
{
    if (!HasAuthority() || !IsValid(PS))
    {
        return false;
    }

    PS->AddCoins(10);

    return true;
}