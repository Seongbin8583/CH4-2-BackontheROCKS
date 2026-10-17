#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BackontheROCKSPlayerController.generated.h"

class UInputMappingContext;
class UBackontheROCKSUI;
class UGameHUDWidget;
class UChatWidget;

UCLASS(abstract)
class BACKONTHEROCKS_API ABackontheROCKSPlayerController
    : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(Server, Reliable)
    void ServerSendChat(const FString& Message);

    UFUNCTION(Client, Reliable)
    void ClientReceiveChat(const FString& Message);

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    bool bUseSteeringWheelControls = false;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (EditCondition = "bUseSteeringWheelControls")
    )
    TObjectPtr<UInputMappingContext> SteeringWheelInputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UBackontheROCKSUI> VehicleUIClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UChatWidget> ChatWidgetClass;

    UPROPERTY()
    TObjectPtr<UBackontheROCKSUI> VehicleUI;

    UPROPERTY()
    TObjectPtr<UChatWidget> ChatWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGameHUDWidget> GameHUDWidgetClass;

    UPROPERTY()
    TObjectPtr<UGameHUDWidget> GameHUDWidget;

private:
    void OpenChat();

    double LastChatTime = -1000.0;
};