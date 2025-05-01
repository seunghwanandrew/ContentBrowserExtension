#pragma once

#include "Widgets/SCompoundWidget.h"

class CONTENTBROWSEREXTENSION_API SAdvancedDeletionWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeletionWidget) {}

	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDataList)

	SLATE_END_ARGS()

public:
	FORCEINLINE FSlateFontInfo GetTitleFontInfo() const { return FCoreStyle::Get().GetFontStyle(FName(TEXT("TitleFont"))); }
	FORCEINLINE FSlateFontInfo GetMenuFontInfo() const { return FCoreStyle::Get().GetFontStyle(FName(TEXT("MonospacedText"))); }
	void Construct(const FArguments& InArgs);

protected:
	TSharedRef<ITableRow> OnGeneratedRowForAssetList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);

	void RefreshAssetListView();
	void ApplyStatusFilterToTypeFilter();
	void ApplyTypeFilterToStatusFilter();
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> SelectedAsset);
	FReply OnDeleteAllButtonClicked();
	FReply OnDeleteUnusedAssetButtonClicked();
	FReply OnSelectAllButtonClicked();
	FReply OnDeselectAllButtonClicked();

	TSharedRef<SWidget> OnGenerateComboTypeContent(TSharedPtr<FString> SourceItem);
	TSharedRef<SWidget> OnGenerateComboStatusContent(TSharedPtr<FString> SourceItem);
	void OnComboStatusSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	void OnComboTypeSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	void GetTypeList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetStatusList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);

	void OnRowWidgetMouseRightButtonClicked(TSharedPtr<FAssetData> ClickedData);

#pragma region ConstructoHelperFunc
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedRef<SHeaderRow> ConstructSDisplayMenuBox();
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructSComboStringSelectionAssetTypeBox();
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructSComboStringSelectionAssetStatusBox();
	TSharedRef<STextBlock> ConstructSTextBlock(const FString& TextBlockText, float FontSize = 10.0f, const FColor& TextColor = FColor::White,
													ETextJustify::Type JustifyType = ETextJustify::Center, const FMargin& Padding = FMargin(20.0f));
	TSharedRef<SCheckBox> ConstructSCheckBox(ESlateCheckBoxType::Type CheckBoxType, TFunction<void(ECheckBoxState)> OnCheckBoxStateChanged, EVisibility VisibilityType = EVisibility::Visible);
	TSharedRef<SButton> ConstructSButton(const FString& ButtonName, TFunction<FReply()> OnClickedFunc, const FMargin& Padding = FMargin(5.0f));
#pragma endregion ConstructoHelperFunc

private:
	FColor WidgetTextColor = FColor(176.f, 176.f, 176.f, 204.0f);
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetListView;
	TSharedPtr<STextBlock> AssetStatusComboBoxDisplayTextBlock;
	TSharedPtr<STextBlock> AssetTypeComboBoxDisplayTextBlock;
	TArray<TSharedPtr<FAssetData>> AssetDataListOnSelectedFolder;
	TArray<TSharedPtr<FAssetData>> FilteredAssetDataList;
	TArray<TSharedPtr<FAssetData>> DisplayAssetDataList;
	TArray<TSharedPtr<FAssetData>> SelectedAssetsArray;
	TArray<TSharedRef<SCheckBox>> ConstructedCheckBoxArray;
	TArray<TSharedPtr<FString>> ComboBoxStatusSourceItems;
	TArray<TSharedPtr<FString>> ComboBoxTypeSourceItems;
	TSharedPtr<FString> StatusFilterOption;
	TSharedPtr<FString> TypeFilterOption;
};