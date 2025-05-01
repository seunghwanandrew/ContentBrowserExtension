#pragma once
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

namespace DebugUtilities
{
	static void Print(const FString& MessageToPrint, const FColor& DisplayColor = FColor::MakeRandomColor(), bool bIsLogPrinting = true, int32 InKey = -1, float TimeToDisplay = 8.0f)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, TimeToDisplay, DisplayColor, MessageToPrint);
		}
		if (bIsLogPrinting)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *MessageToPrint);
		}
	}

	static EAppReturnType::Type ShowDialog(EAppMsgType::Type MessageType, const FString& MessageToShow, bool bShowMsgAsWarning = true)
	{
		if (bShowMsgAsWarning)
		{
			const FText MsgTitle = FText::FromString(TEXT("Warning"));
			return FMessageDialog::Open(MessageType, FText::FromString(MessageToShow), MsgTitle);
		}
		else
		{
			return FMessageDialog::Open(MessageType, FText::FromString(MessageToShow));
		}
	}

	static void ShowNotifyInfo(const FString& MessageToNotify)
	{
		FNotificationInfo NotifyInfo(FText::FromString(MessageToNotify));
		NotifyInfo.bUseLargeFont = true;
		NotifyInfo.FadeOutDuration = 7.0f;

		FSlateNotificationManager::Get().AddNotification(NotifyInfo);
	}
}