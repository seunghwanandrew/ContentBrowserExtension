#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "ContentBrowserExtension.h"

#include "DebugHelpers.h"

#define ListAll TEXT("All Assets")

#define BlueprintType TEXT("Blueprint Assets")
#define StaticMeshType TEXT("Static Mesh Assets")
#define SkeletalMeshType TEXT("Skeletal Mesh Assets")
#define WorldLevelType TEXT("World Level Assets")

#define PhysicsType TEXT("Physics Assets")
#define PhysicsMaterialType TEXT("Physics Material Assets")
#define TextureType TEXT("Texture Assets")
#define MaterialType TEXT("Material Assets")
#define MaterialInstanceType TEXT("Material Instance Assets")

#define AnimBlueprintType TEXT("Anim Blueprint Assets")
#define InterfaceType TEXT("Interface Blueprint Assets")
#define UserWidgetType TEXT("User Widget Blueprint Assets")
#define ActorComponentType TEXT("Actor Component Assets")

#define CurveTableType TEXT("Curve Table Assets")
#define DataTableType TEXT("Data Table Assets")
#define EnumType TEXT("Enum Assets")
#define StructType TEXT("Struct Assets")

#define NiagaraEmitterType TEXT("Niagara Emitter Assets")
#define NiagaraSystemType TEXT("Niagara System Assets")
#define NiagaraFunctionLibraryType TEXT("Niagara Function Library Assets")

#define ControlRigType TEXT("Control Rig Assets")
#define SkeletonType TEXT("Skeleton Assets")
#define AnimMontageType TEXT("Anim Montage Assets")
#define AnimSequenceType TEXT("Anim Sequence Assets")
#define BlendSpaceType TEXT("Blend Space Assets")

#define ListUnusedAsset TEXT("Unused Assets")
#define ListSameNameAsset TEXT("Duplicated Assets : Same Name")

void SAdvancedDeletionWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	SelectedAssetsArray.Empty();
	ConstructedCheckBoxArray.Empty();
	ComboBoxTypeSourceItems.Empty();
	ComboBoxStatusSourceItems.Empty();
	FilteredAssetDataList.Empty();

	AssetDataListOnSelectedFolder = InArgs._AssetDataList;
	DisplayAssetDataList = AssetDataListOnSelectedFolder;

	ComboBoxTypeSourceItems.Add(MakeShared<FString>(ListAll));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(BlueprintType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(StaticMeshType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(SkeletalMeshType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(WorldLevelType));	
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(PhysicsType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(PhysicsMaterialType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(TextureType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(MaterialType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(MaterialInstanceType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(AnimBlueprintType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(InterfaceType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(UserWidgetType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(ActorComponentType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(CurveTableType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(DataTableType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(EnumType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(StructType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(NiagaraEmitterType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(NiagaraSystemType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(NiagaraFunctionLibraryType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(ControlRigType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(SkeletonType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(AnimMontageType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(AnimSequenceType));
	ComboBoxTypeSourceItems.Add(MakeShared<FString>(BlendSpaceType));

	ComboBoxStatusSourceItems.Add(MakeShared<FString>(ListAll));
	ComboBoxStatusSourceItems.Add(MakeShared<FString>(ListUnusedAsset));
	ComboBoxStatusSourceItems.Add(MakeShared<FString>(ListSameNameAsset));

	ChildSlot
	[
		SNew(SVerticalBox)
			// First Slot - Title Text
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				ConstructSTextBlock(TEXT("Delete - Advanced"), 18.0f, WidgetTextColor, ETextJustify::InvariantLeft, FMargin(20.0f))
			]

			// Second Slot - Drop Downs And Helper Text
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				// Combo Box 1
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.AutoWidth()
				.Padding(2.0f, 2.0f)
				[
					ConstructSComboStringSelectionAssetTypeBox()
				]
				// Combo Box 2
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.AutoWidth()
				.Padding(2.0f, 2.0f)
				[
					ConstructSComboStringSelectionAssetStatusBox()
				]
			]

			// Third Slot - Asset List Menu
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				ConstructSDisplayMenuBox()		
			]

			// Forth Slot - Asset List
			+ SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					ConstructAssetListView()
				]
			]

			// Fifth Slot - Delete All / Select All / Deselect All
			+ SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.AutoHeight()
			[
				SNew(SHorizontalBox)				
				// Delete All
				+ SHorizontalBox::Slot()
				[
					ConstructSButton(TEXT("Delete : Force All"), [this]() {return OnDeleteAllButtonClicked();})
				]
				// Delete Unused Asset Among Selected
				+ SHorizontalBox::Slot()
				[
					ConstructSButton(TEXT("Delete : Unused Assets"), [this]() {return OnDeleteUnusedAssetButtonClicked();})
				]
				// Select All
				+ SHorizontalBox::Slot()
				[
					ConstructSButton(TEXT("Select All"), [this]() {return OnSelectAllButtonClicked();})
				]
				// DeSelect All
				+ SHorizontalBox::Slot()
				[
					ConstructSButton(TEXT("Deselect All"), [this]() {return OnDeselectAllButtonClicked();})
				]
			]
	];
}

TSharedRef<ITableRow> SAdvancedDeletionWidget::OnGeneratedRowForAssetList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid()) return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);

	const FString& AssetClassName = AssetDataToDisplay->GetAsset()->GetClass()->GetName();
	const FString& AssetName = AssetDataToDisplay->AssetName.ToString();

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget = SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(0.0f, 0.5f))
		[
			SNew(SHorizontalBox)
			// First Slot - Check Box
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.FillWidth(0.12f)
			[
				ConstructSCheckBox(ESlateCheckBoxType::CheckBox, [AssetDataToDisplay, this](ECheckBoxState NewState) { this->OnCheckBoxStateChanged(NewState, AssetDataToDisplay); })
			]
			
			// Second Slot - Asset Class Name
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.FillWidth(0.34f)
			[
				ConstructSTextBlock(AssetClassName, 9.0f, WidgetTextColor, ETextJustify::Left, FMargin(1.0f))
			]

			// Third Slot - Asset Name
			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.FillWidth(0.68f)
			[
				ConstructSTextBlock(AssetName, 9.0f, WidgetTextColor, ETextJustify::Left, FMargin(1.0f))
			]

			// Forth Slot - Delete SButton
			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.FillWidth(0.11f)
			[
				ConstructSButton(TEXT("Force Delete"), [AssetDataToDisplay, this]() {return OnDeleteButtonClicked(AssetDataToDisplay);}, 2.0f)
			]
		];

	return ListViewRowWidget;
}

void SAdvancedDeletionWidget::RefreshAssetListView()
{
	SelectedAssetsArray.Empty();
	ConstructedCheckBoxArray.Empty();

	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
	}
}

void SAdvancedDeletionWidget::ApplyStatusFilterToTypeFilter()
{
	DisplayAssetDataList.Empty();

	GetTypeList(FilteredAssetDataList, DisplayAssetDataList);

	RefreshAssetListView();
}

void SAdvancedDeletionWidget::ApplyTypeFilterToStatusFilter()
{
	DisplayAssetDataList.Empty();

	GetStatusList(FilteredAssetDataList, DisplayAssetDataList);

	RefreshAssetListView();
}

void SAdvancedDeletionWidget::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		SelectedAssetsArray.Remove(AssetData);
		break;
	case ECheckBoxState::Checked:
		SelectedAssetsArray.AddUnique(AssetData);
		break;
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}

FReply SAdvancedDeletionWidget::OnDeleteButtonClicked(TSharedPtr<FAssetData> SelectedAsset)
{
	if (!SelectedAsset.IsValid()) return FReply::Unhandled();

	FContentBrowserExtensionModule& ContentBrowserExtensionModule = FModuleManager::LoadModuleChecked<FContentBrowserExtensionModule>(TEXT("ContentBrowserExtension"));

	if (ContentBrowserExtensionModule.DeleteSingleAsset(*SelectedAsset.Get()))
	{
		if (AssetDataListOnSelectedFolder.Contains(SelectedAsset))
		{
			AssetDataListOnSelectedFolder.Remove(SelectedAsset);
		}

		if (DisplayAssetDataList.Contains(SelectedAsset))
		{
			DisplayAssetDataList.Remove(SelectedAsset);
		}

		RefreshAssetListView();
	}

	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeleteAllButtonClicked()
{
	if (SelectedAssetsArray.IsEmpty())
	{
		DebugUtilities::ShowDialog(EAppMsgType::Ok, TEXT("None Selected Assets"));
		return FReply::Unhandled();
	}

	FContentBrowserExtensionModule& ContentBrowserExtensionModule = FModuleManager::LoadModuleChecked<FContentBrowserExtensionModule>(TEXT("ContentBrowserExtension"));

	if (ContentBrowserExtensionModule.DeleteAllSelectedAssets(SelectedAssetsArray))
	{
		for (const TSharedPtr<FAssetData>& Asset : SelectedAssetsArray)
		{
			if (AssetDataListOnSelectedFolder.Contains(Asset))
			{
				AssetDataListOnSelectedFolder.Remove(Asset);
			}
			if (DisplayAssetDataList.Contains(Asset))
			{
				DisplayAssetDataList.Remove(Asset);
			}
		}

		RefreshAssetListView();
	}

	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeleteUnusedAssetButtonClicked()
{
	if (SelectedAssetsArray.IsEmpty())
	{
		DebugUtilities::ShowDialog(EAppMsgType::Ok, TEXT("None Selected Assets"));
		return FReply::Unhandled();
	}

	FContentBrowserExtensionModule& ContentBrowserExtensionModule = FModuleManager::LoadModuleChecked<FContentBrowserExtensionModule>(TEXT("ContentBrowserExtension"));

	bool bAssetDeleted = ContentBrowserExtensionModule.DeleteUnusedAssetsAmongSelectedAssets(SelectedAssetsArray, DisplayAssetDataList);

	if (bAssetDeleted)
	{
		RefreshAssetListView();
	}

	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnSelectAllButtonClicked()
{
	if (ConstructedCheckBoxArray.IsEmpty()) return FReply::Unhandled();

	for (TSharedRef<SCheckBox> CheckBox : ConstructedCheckBoxArray)
	{
		if (CheckBox->IsChecked()) continue;

		CheckBox->ToggleCheckedState();
	}

	return FReply::Handled();
}

FReply SAdvancedDeletionWidget::OnDeselectAllButtonClicked()
{
	if (ConstructedCheckBoxArray.IsEmpty()) return FReply::Unhandled();

	for (TSharedRef<SCheckBox> CheckBox : ConstructedCheckBoxArray)
	{
		if (!CheckBox->IsChecked()) continue;

		CheckBox->ToggleCheckedState();
	}

	return FReply::Handled();
}

TSharedRef<SWidget> SAdvancedDeletionWidget::OnGenerateComboTypeContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> ConstructedComboContent = SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return ConstructedComboContent;
}

TSharedRef<SWidget> SAdvancedDeletionWidget::OnGenerateComboStatusContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<SWidget> ConstructedComboContent = SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));

	return ConstructedComboContent;
}

void SAdvancedDeletionWidget::OnComboStatusSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	AssetStatusComboBoxDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));

	StatusFilterOption = SelectedOption;	

	GetStatusList(AssetDataListOnSelectedFolder, FilteredAssetDataList);

	ApplyStatusFilterToTypeFilter();
}

void SAdvancedDeletionWidget::OnComboTypeSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	AssetTypeComboBoxDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));

	TypeFilterOption = SelectedOption;

	GetTypeList(AssetDataListOnSelectedFolder, FilteredAssetDataList);

	ApplyTypeFilterToStatusFilter();
}

void SAdvancedDeletionWidget::GetTypeList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
	OutBlueprintAssetList.Empty();

	FContentBrowserExtensionModule& ContentBrowserExtensionModule = FModuleManager::LoadModuleChecked<FContentBrowserExtensionModule>(TEXT("ContentBrowserExtension"));

	if (*TypeFilterOption.Get() == ListAll || !TypeFilterOption.IsValid())
	{
		OutBlueprintAssetList = DataToFilter;
		return;
	}

	if (*TypeFilterOption.Get() == BlueprintType)
	{
		ContentBrowserExtensionModule.GetBlueprintTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == StaticMeshType)
	{
		ContentBrowserExtensionModule.GetStaticMeshTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == SkeletalMeshType)
	{
		ContentBrowserExtensionModule.GetSkeletalMeshTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == WorldLevelType)
	{
		ContentBrowserExtensionModule.GetWorldLevelTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == PhysicsType)
	{
		ContentBrowserExtensionModule.GetPhysicsTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == PhysicsMaterialType)
	{
		ContentBrowserExtensionModule.GetPhysicsMaterialTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == TextureType)
	{
		ContentBrowserExtensionModule.GetTextureTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == MaterialType)
	{
		ContentBrowserExtensionModule.GetMaterialTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == MaterialInstanceType)
	{
		ContentBrowserExtensionModule.GetMaterialInstanceTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == AnimBlueprintType)
	{
		ContentBrowserExtensionModule.GetAnimBlueprintTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == InterfaceType)
	{
		ContentBrowserExtensionModule.GetInterfaceTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == UserWidgetType)
	{
		ContentBrowserExtensionModule.GetUserWidgetTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == ActorComponentType)
	{
		ContentBrowserExtensionModule.GetActorComponentTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == CurveTableType)
	{
		ContentBrowserExtensionModule.GetCurveTableTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == DataTableType)
	{
		ContentBrowserExtensionModule.GetDataTableTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == EnumType)
	{
		ContentBrowserExtensionModule.GetEnumDataTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == StructType)
	{
		ContentBrowserExtensionModule.GetStructDataTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == NiagaraEmitterType)
	{
		ContentBrowserExtensionModule.GetNiagaraEmitterTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == NiagaraSystemType)
	{
		ContentBrowserExtensionModule.GetNiagaraSystemTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == NiagaraFunctionLibraryType)
	{
		ContentBrowserExtensionModule.GetNiagaraFunctionLibraryTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == ControlRigType)
	{
		ContentBrowserExtensionModule.GetControlRigTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == SkeletonType)
	{
		ContentBrowserExtensionModule.GetSkeletonTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == AnimMontageType)
	{
		ContentBrowserExtensionModule.GetAnimMontageTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == AnimSequenceType)
	{
		ContentBrowserExtensionModule.GetAnimSequenceTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*TypeFilterOption.Get() == BlendSpaceType)
	{
		ContentBrowserExtensionModule.GetBlendSpaceTypeAssetList(DataToFilter, OutBlueprintAssetList);
	}	
}

void SAdvancedDeletionWidget::GetStatusList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
	OutBlueprintAssetList.Empty();

	FContentBrowserExtensionModule& ContentBrowserExtensionModule = FModuleManager::LoadModuleChecked<FContentBrowserExtensionModule>(TEXT("ContentBrowserExtension"));

	if (!StatusFilterOption.IsValid() || *StatusFilterOption.Get() == ListAll)
	{
		OutBlueprintAssetList = DataToFilter;
		return;
	}
	
	if (*StatusFilterOption.Get() == ListUnusedAsset)
	{
		ContentBrowserExtensionModule.GetUnusedAssetList(DataToFilter, OutBlueprintAssetList);
	}
	else if (*StatusFilterOption.Get() == ListSameNameAsset)
	{
		ContentBrowserExtensionModule.GetHaveSameNameAssetList(DataToFilter, OutBlueprintAssetList);
	}
}

void SAdvancedDeletionWidget::OnRowWidgetMouseRightButtonClicked(TSharedPtr<FAssetData> ClickedData)
{
	FContentBrowserExtensionModule& ContentBrowserExtensionModule = FModuleManager::LoadModuleChecked<FContentBrowserExtensionModule>(TEXT("ContentBrowserExtension"));

	ContentBrowserExtensionModule.SyncContentBrowserDirectToClickedAssetLocation(ClickedData->GetObjectPathString());
}

#pragma region ConstructoHelperFunc
TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvancedDeletionWidget::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(24.0f)
		.ListItemsSource(&DisplayAssetDataList)
		.OnMouseButtonClick(this, &SAdvancedDeletionWidget::OnRowWidgetMouseRightButtonClicked)
		.OnGenerateRow(this, &SAdvancedDeletionWidget::OnGeneratedRowForAssetList);

	return ConstructedAssetListView.ToSharedRef();
}
TSharedRef<SHeaderRow> SAdvancedDeletionWidget::ConstructSDisplayMenuBox()
{
	TSharedRef<SHeaderRow> ConstructedDisplayMenuBox = SNew(SHeaderRow)
		+ SHeaderRow::Column(FName(TEXT("CheckBox")))
		.FillWidth(0.1f)
		[
			ConstructSTextBlock(TEXT("Check"), 10.0f)
		]
		+ SHeaderRow::Column(FName(TEXT("AssetClass")))
		.FillWidth(0.32f)
		[
			ConstructSTextBlock(TEXT("Asset Class"), 10.0f)
		]
		+ SHeaderRow::Column(FName(TEXT("AssetName")))
		.FillWidth(0.62f)
		[
			ConstructSTextBlock(TEXT("Asset Name"), 10.0f)
		]
		+SHeaderRow::Column(FName(TEXT("DeleteButton")))
		.FillWidth(0.13f)
		[
			ConstructSTextBlock(TEXT("Delete Button"), 10.0f)
		];

	return ConstructedDisplayMenuBox;
}
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvancedDeletionWidget::ConstructSComboStringSelectionAssetTypeBox()
{
	FSlateFontInfo ListTextFont = GetMenuFontInfo();
	ListTextFont.Size = 11.5f;

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboStringSelectionAssetTypeBox = SNew(SComboBox<TSharedPtr<FString>>)
		[
			SAssignNew(AssetTypeComboBoxDisplayTextBlock, STextBlock)
				.Text(FText::FromString("Select : Asset Type List Option"))
				.Font(ListTextFont)
				.Margin(FMargin(2.0f, 2.0f))
		]
		.HasDownArrow(true)
		.OptionsSource(&ComboBoxTypeSourceItems)
		.OnGenerateWidget(this, &SAdvancedDeletionWidget::OnGenerateComboTypeContent)
		.OnSelectionChanged(this, &SAdvancedDeletionWidget::OnComboTypeSelectionChanged);

	return ConstructedComboStringSelectionAssetTypeBox;
}
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvancedDeletionWidget::ConstructSComboStringSelectionAssetStatusBox()
{
	FSlateFontInfo ListTextFont = GetMenuFontInfo();
	ListTextFont.Size = 11.5f;

	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboStringSelectionAssetStatusBox = SNew(SComboBox<TSharedPtr<FString>>)
		[
			SAssignNew(AssetStatusComboBoxDisplayTextBlock, STextBlock)
				.Text(FText::FromString("Select : Asset State List Option"))
				.Font(ListTextFont)
				.Margin(FMargin(2.0f, 2.0f))
		]
		.HasDownArrow(true)
		.OptionsSource(&ComboBoxStatusSourceItems)
		.OnGenerateWidget(this, &SAdvancedDeletionWidget::OnGenerateComboStatusContent)
		.OnSelectionChanged(this, &SAdvancedDeletionWidget::OnComboStatusSelectionChanged);

	return ConstructedComboStringSelectionAssetStatusBox;
}
TSharedRef<STextBlock> SAdvancedDeletionWidget::ConstructSTextBlock(const FString& TextBlockText, float FontSize, const FColor& TextColor, ETextJustify::Type JustifyType, const FMargin& Padding)
{
	FSlateFontInfo TitleTextFont = GetTitleFontInfo();
	TitleTextFont.Size = FontSize;

	TSharedRef<STextBlock> ContructedSTextBlock = SNew(STextBlock)
		.Text(FText::FromString(TextBlockText))
		.Font(TitleTextFont)
		.ColorAndOpacity(TextColor)
		.Justification(JustifyType)
		.Margin(Padding);
		
	return ContructedSTextBlock;
}

TSharedRef<SCheckBox> SAdvancedDeletionWidget::ConstructSCheckBox(ESlateCheckBoxType::Type CheckBoxType, TFunction<void(ECheckBoxState)> OnCheckBoxStateChanged, EVisibility VisibilityType)
{
	TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
		.Type(CheckBoxType)
		.Visibility(VisibilityType)
		.OnCheckStateChanged_Lambda([OnCheckBoxStateChanged](ECheckBoxState NewState) {OnCheckBoxStateChanged(NewState);});

	ConstructedCheckBoxArray.AddUnique(ConstructedCheckBox);

	return ConstructedCheckBox;
}

TSharedRef<SButton> SAdvancedDeletionWidget::ConstructSButton(const FString& ButtonName, TFunction<FReply()> OnClickedFunc, const FMargin& Padding)
{
	TSharedRef<SButton> ConstructedButton = SNew(SButton)
		.ContentPadding(Padding)
		.OnClicked_Lambda([OnClickedFunc]() {return OnClickedFunc();});

	ConstructedButton->SetContent(ConstructSTextBlock(ButtonName, 9.0f, WidgetTextColor, ETextJustify::Center, Padding));
	return ConstructedButton;
}

#pragma endregion ConstructoHelperFunc


