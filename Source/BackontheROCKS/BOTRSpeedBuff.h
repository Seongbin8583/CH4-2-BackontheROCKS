#pragma once

#include "CoreMinimal.h"
#include "BOTRPickup.h"
#include "BOTRSpeedBuff.generated.h"

UCLASS()
class BACKONTHEROCKS_API ABOTRSpeedBuff : public ABOTRPickup
{
    GENERATED_BODY()

protected:
    virtual bool GiveReward(
        ABOTRPlayerState* PS
    ) override;
};