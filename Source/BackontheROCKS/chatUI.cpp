#include "chatUI.h"

#include "BackontheROCKSPlayerController.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ChatInput->OnTextCommitted.AddUniqueDynamic(
        this,
        &UChatWidget::OnChatCommitted
    );

    ChatInput->SetClearKeyboardFocusOnCommit(true);
}

void UChatWidget::NativeDestruct()
{
    if (ChatInput)
    {
        ChatInput->OnTextCommitted.RemoveDynamic(
            this,
            &UChatWidget::OnChatCommitted
        );
    }

    Super::NativeDestruct();
}

void UChatWidget::FocusChat()
{
    APlayerController* PC = GetOwningPlayer();

    if (!PC || !ChatInput)
    {
        return;
    }

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(ChatInput->TakeWidget());

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;

    ChatInput->SetKeyboardFocus();
}

void UChatWidget::ReturnToGame()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

void UChatWidget::OnChatCommitted(
    const FText& Text,
    ETextCommit::Type CommitMethod
)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        const FString Message =
            Text.ToString().TrimStartAndEnd();

        if (!Message.IsEmpty())
        {
            if (ABackontheROCKSPlayerController* PC =
                Cast<ABackontheROCKSPlayerController>(
                    GetOwningPlayer()
                ))
            {
                PC->ServerSendChat(Message);
            }
        }

        ChatInput->SetText(FText::GetEmpty());
    }

    ReturnToGame();
}

void UChatWidget::AddChatMessage(const FString& Message)
{
    if (!MessageBox)
    {
        return;
    }

    UTextBlock* MessageText =
        NewObject<UTextBlock>(this);

    MessageText->SetText(FText::FromString(Message));
    MessageText->SetAutoWrapText(true);

    MessageBox->AddChild(MessageText);

    // 메시지 무한 누적 방지
    while (MessageBox->GetChildrenCount() > 100)
    {
        MessageBox->RemoveChildAt(0);
    }

    MessageBox->ScrollToEnd();
}