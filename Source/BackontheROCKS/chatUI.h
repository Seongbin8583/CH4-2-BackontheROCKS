#include "chatUI.generated.h"

UCLASS()
class YOURPROJECT_API UChatWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEditableTextBox> ChatInput;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> MessageBox;

    UFUNCTION()
    void OnChatCommitted(
        const FText& Text,
        ETextCommit::Type CommitMethod);

public:
    void AddChatMessage(
        const FString& Message);
};