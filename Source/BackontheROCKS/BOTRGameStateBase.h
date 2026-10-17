#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BOTRGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EBOTRGameStateBase : uint8
{
    Waiting     UMETA(DisplayName = "Waiting"),
    Playing     UMETA(DisplayName = "Playing"),
    Finished    UMETA(DisplayName = "Finished")
};

UCLASS()
class BACKONTHEROCKS_API ABOTRGameStateBase : public AGameStateBase
{
    GENERATED_BODY()

public:
    ABOTRGameStateBase();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    UPROPERTY(
        Replicated,
        BlueprintReadOnly,
        Category = "Game"
    )
    EBOTRGameStateBase CurrentState;

    UPROPERTY(
        Replicated,
        BlueprintReadOnly,
        Category = "Game"
    )
    int32 RemainingTime;

    UPROPERTY(
        Replicated,
        BlueprintReadOnly,
        Category = "Game"
    )
    TArray<APlayerState*> Winners;

    void SetCurrentState(EBOTRGameStateBase NewState);

    void SetRemainingTime(int32 NewTime);

    void SetWinners(
        const TArray<APlayerState*>& NewWinners
    );
};