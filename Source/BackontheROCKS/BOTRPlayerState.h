#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BOTRPlayerState.generated.h"

UCLASS()
class BACKONTHEROCKS_API ABOTRPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ABOTRPlayerState();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    UFUNCTION(BlueprintCallable, Category = "PlayerState")
    void AddCoins(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "PlayerState")
    bool RemoveCoins(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "PlayerState")
    int32 GetCoins() const
    {
        return Coins;
    }

    UFUNCTION(BlueprintCallable, Category = "PlayerState")
    void AddBonusMaxSpeed(float Amount);

    UFUNCTION(BlueprintPure, Category = "PlayerState")
    float GetBonusMaxSpeed() const
    {
        return BonusMaxSpeed;
    }

protected:

    UPROPERTY(
        ReplicatedUsing = OnRep_Coins,
        BlueprintReadOnly,
        Category = "PlayerState"
    )
    int32 Coins = 0;

    UPROPERTY(
        ReplicatedUsing = OnRep_BonusMaxSpeed,
        BlueprintReadOnly,
        Category = "PlayerState"
    )
    float BonusMaxSpeed = 0.0f;

    UFUNCTION()
    void OnRep_Coins();

    UFUNCTION()
    void OnRep_BonusMaxSpeed();
};