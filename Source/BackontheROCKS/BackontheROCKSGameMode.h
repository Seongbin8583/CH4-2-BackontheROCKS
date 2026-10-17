// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BackontheROCKSGameMode.generated.h"

UCLASS()
class BACKONTHEROCKS_API ABackontheROCKSGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABackontheROCKSGameMode();

    virtual void PostLogin(
        APlayerController* NewPlayer
    ) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 WaitingDuration = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 PlayingDuration = 60;

    FTimerHandle GameTimerHandle;

    bool bCountdownStarted = false;

    void StartWaiting();
    void StartPlaying();
    void FinishGame();
    void UpdateGameTimer();
};



