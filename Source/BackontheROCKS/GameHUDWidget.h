#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"

UCLASS()
class BACKONTHEROCKS_API UGameHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "HUD")
    FText GetCoinsText() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    FText GetBonusSpeedText() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    FText GetMatchText() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    FText GetResultText() const;
};