#include "CustomSlateStyle/ExtensionContentBrowserCustom.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

FName FExtensionContentBrowserCustom::CustomIconStyleSetName = FName("ExtensionContentBrowserCustom");
TSharedPtr<FSlateStyleSet> FExtensionContentBrowserCustom::CreatedCustomStyleSet = nullptr;

void FExtensionContentBrowserCustom::InitializeCustomIconStyle()
{
	if (!CreatedCustomStyleSet.IsValid())
	{
		CreatedCustomStyleSet = CreateCustomStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedCustomStyleSet);
	}
}

void FExtensionContentBrowserCustom::TerminateCustomIconStyle()
{
	if (CreatedCustomStyleSet.IsValid()) 
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedCustomStyleSet);
		CreatedCustomStyleSet.Reset();
	}
}

TSharedRef<FSlateStyleSet> FExtensionContentBrowserCustom::CreateCustomStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomIconStyleSet = MakeShareable(new FSlateStyleSet(CustomIconStyleSetName));

	const FString IconDirectoryPath = IPluginManager::Get().FindPlugin(TEXT("ContentBrowserExtension"))->GetBaseDir() / "Resources";

	CustomIconStyleSet->SetContentRoot(IconDirectoryPath);
	
	const FVector2D IconSize16X16 = FVector2D(16.0f, 16.0f);
	CustomIconStyleSet->Set("ContentBrowser.DeletionAssetIcon", new FSlateImageBrush(IconDirectoryPath / "Delete_40x.png", IconSize16X16));
	CustomIconStyleSet->Set("ContentBrowser.DuplicateAssetIcon", new FSlateImageBrush(IconDirectoryPath / "Copy_40x.png", IconSize16X16));

	return CustomIconStyleSet;
}
