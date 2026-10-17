#include "BOTRSpeedBuff.h"
#include "BOTRPlayerState.h"

bool ABOTRSpeedBuff::GiveReward(ABOTRPlayerState* PS)
{
    if (!HasAuthority() || !IsValid(PS))
    {
        return false;
    }

    PS->AddBonusMaxSpeed(10.0f);

    return true;
}