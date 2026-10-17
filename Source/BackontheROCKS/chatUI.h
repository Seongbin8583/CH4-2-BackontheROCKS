#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "chatUI.generated.h"

class UScrollBox;

UCLASS()
class BACKONTHEROCKS_API UChatWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void AddChatMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "Chat")
    void FocusChat();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEditableTextBox> ChatInput;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UScrollBox> MessageBox;

    UFUNCTION()
    void OnChatCommitted(
        const FText& Text,
        ETextCommit::Type CommitMethod
    );

private:
    void ReturnToGame();
};