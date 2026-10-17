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

    virtual AActor* ChoosePlayerStart_Implementation(
        AController* Player
    ) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 WaitingDuration = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 PlayingDuration = 60;

    FTimerHandle GameTimerHandle;

    bool bCountdownStarted = false;

    bool IsPlayerStartAvailable(
        const class APlayerStart* PlayerStart
    ) const;

    UPROPERTY(EditDefaultsOnly, Category = "Spawn")
    FVector VehicleSpawnCheckOffset =
        FVector(0.0f, 0.0f, 100.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Spawn")
    FVector VehicleSpawnCheckHalfExtent =
        FVector(250.0f, 120.0f, 100.0f);

    void StartWaiting();
    void StartPlaying();
    void FinishGame();
    void UpdateGameTimer();
};



