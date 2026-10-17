#pragma once

#include "CoreMinimal.h"
#include "BOTRPickup.h"
#include "BOTRCoin.generated.h"

UCLASS()
class BACKONTHEROCKS_API ABOTRCoin : public ABOTRPickup
{
    GENERATED_BODY()

protected:
    virtual bool GiveReward(
        ABOTRPlayerState* PS
    ) override;
};