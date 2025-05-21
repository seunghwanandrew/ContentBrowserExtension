#include "ContentBrowserExtension.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ContentBrowserModule.h"
#include "ObjectTools.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "CustomSlateStyle/ExtensionContentBrowserCustom.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/Texture.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/BlendSpace1D.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidgetBlueprint.h"
#include "Blueprint/BlueprintSupport.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"
#include "UObject/EnumProperty.h"
#include "UObject/Class.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ControlRig.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpace.h"
#include "LevelSequence.h"

#include "DebugHelpers.h"

#define LOCTEXT_NAMESPACE "FContentBrowserExtensionModule"

void FContentBrowserExtensionModule::StartupModule()
{
	InitPrefixMap();
	InitCustomContentBrowserExtension();
    RegisterCustomSlateEditorTab_AdvancedDeletion();
    FExtensionContentBrowserCustom::InitializeCustomIconStyle();
}

void FContentBrowserExtensionModule::ShutdownModule()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvanceDeletion"));
    FExtensionContentBrowserCustom::TerminateCustomIconStyle();
}

void FContentBrowserExtensionModule::SyncContentBrowserDirectToClickedAssetLocation(const FString& AssetPathToSync)
{
    TArray<FString> AssetsPathToSync;
    AssetsPathToSync.Add(AssetPathToSync);

    UEditorAssetLibrary::SyncBrowserToObjects(AssetsPathToSync);
}

bool FContentBrowserExtensionModule::DeleteSingleAsset(const FAssetData& AssetDataToDelete)
{
    TArray<FAssetData> AssetDataArrayToDelete;
    AssetDataArrayToDelete.AddUnique(AssetDataToDelete);
    int32 Counter = ObjectTools::DeleteAssets(AssetDataArrayToDelete);

    return Counter > 0;
}

bool FContentBrowserExtensionModule::DeleteAllSelectedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataArrayToDelete)
{
    TArray<FAssetData> AssetDataArray;

    for (TSharedPtr<FAssetData> AssetDataToDelete : AssetDataArrayToDelete)
    {
        AssetDataArray.Add(*AssetDataToDelete.Get());
    }

    if (AssetDataArray.Num() == 0) return false;

    int32 Counter = ObjectTools::DeleteAssets(AssetDataArray);

    if (Counter > 0)
    {
        DebugUtilities::ShowNotifyInfo(FString::FromInt(Counter) + TEXT(" Assets Are Deleted."));
        return true;
    }

    return false;
}

bool FContentBrowserExtensionModule::DeleteUnusedAssetsAmongSelectedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataArrayToDelete, TArray<TSharedPtr<FAssetData>>& AssetDataListOnSelectedFolder)
{
    TArray<FAssetData> UnusedData;

    for (TSharedPtr<FAssetData> AssetDataPtr : AssetDataArrayToDelete)
    {
        FAssetData AssetData = *AssetDataPtr.Get();
        TArray<FString> AssetReferencer = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData.GetObjectPathString());

        if (AssetReferencer.Num() == 0)
        {
            UnusedData.AddUnique(AssetData);
            AssetDataListOnSelectedFolder.Remove(AssetDataPtr);
        }
    }

    int32 Counter = 0;

    if (UnusedData.Num() > 0)
    {
        Counter = ObjectTools::DeleteAssets(UnusedData);
    }

    if (Counter > 0)
    {
        DebugUtilities::ShowNotifyInfo(FString::FromInt(Counter) + TEXT(" Unused Assets Are Deleted."));
        return true;
    }

    return false;
}

void FContentBrowserExtensionModule::GetUnusedAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssetList)
{
    OutUnusedAssetList.Empty();

    for (const TSharedPtr<FAssetData>& DataSharedPtr : DataToFilter)
    {
        TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(DataSharedPtr->GetObjectPathString());
        if (AssetReferencers.Num() == 0)
        {
            OutUnusedAssetList.Add(DataSharedPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetHaveSameNameAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssetList)
{
    OutSameNameAssetList.Empty();

    TMultiMap<FString, TSharedPtr<FAssetData>> ConfigureSameNameAssetData;

    for (const TSharedPtr<FAssetData>& DataSharedPtr : DataToFilter)
    {
        FString DataName = DataSharedPtr->AssetName.ToString();
        ConfigureSameNameAssetData.AddUnique(DataName, DataSharedPtr);
    }

    for (const TSharedPtr<FAssetData>& DataSharedPtr : DataToFilter)
    {
        TArray<TSharedPtr<FAssetData>> OutAssetData;
        FString DataName = DataSharedPtr->AssetName.ToString();
        ConfigureSameNameAssetData.MultiFind(DataName, OutAssetData);

        if (OutAssetData.Num() > 1)
        {
            for (const TSharedPtr<FAssetData>& SameNameData : OutAssetData)
            {
                if (!SameNameData.IsValid()) continue;
                OutSameNameAssetList.AddUnique(SameNameData);
            }
        }
    }
}

void FContentBrowserExtensionModule::GetBlueprintTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    OutBlueprintAssetList.Empty();

    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UBlueprint::StaticClass() || SharedDataPtr.Get()->GetClass() == UBlueprintGeneratedClass::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetStaticMeshTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    OutBlueprintAssetList.Empty();

    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UStaticMesh::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetSkeletalMeshTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    OutBlueprintAssetList.Empty();

    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == USkeletalMesh::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetWorldLevelTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    OutBlueprintAssetList.Empty();

    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UWorld::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetPhysicsTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    OutBlueprintAssetList.Empty();

    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UPhysicsAsset::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetPhysicsMaterialTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UPhysicalMaterial::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetTextureTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UTexture::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetMaterialTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UMaterial::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetMaterialInstanceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UMaterialInstanceConstant::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetAnimBlueprintTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UAnimBlueprint::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetInterfaceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UInterface::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetUserWidgetTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UUserWidget::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetActorComponentTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UActorComponent::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetCurveTableTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UCurveTable::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetDataTableTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UDataTable::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetEnumDataTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UEnum::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetStructDataTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UStruct::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetNiagaraEmitterTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UNiagaraEmitter::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetNiagaraSystemTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UNiagaraSystem::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetNiagaraFunctionLibraryTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UNiagaraFunctionLibrary::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetControlRigTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UControlRig::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetSkeletonTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == USkeleton::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetAnimMontageTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UAnimMontage::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetAnimSequenceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UAnimSequence::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

void FContentBrowserExtensionModule::GetBlendSpaceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList)
{
    for (const TSharedPtr<FAssetData>& SharedDataPtr : DataToFilter)
    {
        if (SharedDataPtr.Get()->GetClass() == UBlendSpace::StaticClass() || SharedDataPtr.Get()->GetClass() == UBlendSpace1D::StaticClass())
        {
            OutBlueprintAssetList.AddUnique(SharedDataPtr);
        }
    }
}

#pragma region EngineExtension
void FContentBrowserExtensionModule::InitCustomContentBrowserExtension()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(FName(TEXT("ContentBrowser")));

    TArray<FContentBrowserMenuExtender_SelectedPaths>& PathViewMenuExtender = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
    TArray<FContentBrowserMenuExtender_SelectedAssets>& AssetViewMenuExtender = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

    // First Bind - Create Delegate | BindAction | Add To Array
    /*FContentBrowserMenuExtender_SelectedPaths CustomPathViewContentBrowserMenuDelegate_Delete;
    CustomPathViewContentBrowserMenuDelegate_Delete.BindRaw(this, &FContentBrowserExtensionModule::CustomPathViewMenuExtender);
    PathViewMenuExtender.Add(CustomPathViewContentBrowserMenuDelegate_Delete);*/

    // First Bind - Short Version
    PathViewMenuExtender.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FContentBrowserExtensionModule::CustomPathViewMenuExtender));
    AssetViewMenuExtender.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FContentBrowserExtensionModule::CustomAssetViewMenuExtender));
}

TSharedRef<SWindow> FContentBrowserExtensionModule::ConstructSEditableTextBoxForGetInputNumOfDuplicate()
{
    TSharedPtr<SEditableTextBox> InputTextBox = SNew(SEditableTextBox);
    TWeakPtr<SWindow> WeakInputWindow;

    TSharedRef<SWindow> InputWindow = SNew(SWindow)
        .Title(FText::FromString(TEXT("Multiple Duplicate")))
        .ClientSize(FVector2D(450.0f, 60.0f))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    InputWindow->SetContent(
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(3.0f)
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(EVerticalAlignment::VAlign_Center)
            .Padding(0.5f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Enter Duplicates Numbers : ")))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(0.5f)            
            [                
                SAssignNew(InputTextBox, SEditableTextBox)
                .HintText(FText::FromString(TEXT("e.g. 3")))             
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(EHorizontalAlignment::HAlign_Center)
        .Padding(3.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(5.0f)
            .AutoWidth()
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("OK")))
                .OnClicked_Lambda([InputTextBox, WeakInputWindow = TWeakPtr<SWindow>(InputWindow), this]()
                    {
                        FString InputString = InputTextBox->GetText().ToString();
                        int32 NumberOfDuplicates = FCString::Atoi(*InputString);

                        if (NumberOfDuplicates > 0)
                        {
                            DuplicateBatchAssets(NumberOfDuplicates);
                        }
                        else
                        {
                            DebugUtilities::ShowDialog(EAppMsgType::Ok, TEXT("Not Valid Input Parameters"));
                        }

                        WeakInputWindow.Pin()->RequestDestroyWindow();
                        return FReply::Handled();
                    })
            ]
            + SHorizontalBox::Slot()
            .Padding(5.0f)
            .AutoWidth()
            [
                SNew(SButton)
                .Text(FText::FromString("Cancel"))
                .OnClicked_Lambda([WeakInputWindow = TWeakPtr<SWindow>(InputWindow)]()
                    {
                        WeakInputWindow.Pin()->RequestDestroyWindow();
                        return FReply::Handled();
                    })
            ]
        ]
    );

    FSlateApplication::Get().AddWindow(InputWindow);

    return InputWindow;
}

TSharedRef<FExtender> FContentBrowserExtensionModule::CustomPathViewMenuExtender(const TArray<FString>& SelectedPaths)
{
    FolderSelectedPaths = SelectedPaths;

    TSharedRef<FExtender> PathViewMenuExtender(new FExtender());

    if (SelectedPaths.Num() > 0)
    {
        PathViewMenuExtender->AddMenuExtension(
            FName(TEXT("Delete")),
            EExtensionHook::After,
            TSharedPtr<FUICommandList>(),
            FMenuExtensionDelegate::CreateRaw(this, &FContentBrowserExtensionModule::AddAfterDeleteOnPathViewContentBrowserMenuEntry)
        );
    }

    return PathViewMenuExtender;
}

void FContentBrowserExtensionModule::AddAfterDeleteOnPathViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Delete : Clearing Folder")),
        FText::FromString(TEXT("Clearing And Delete Empty Folder After Checking.\nIf Folder Is Empty Then Delete Folder.\nIf Not Empty,Then Clearing Unused Assets On Selected Folder.")),
        FSlateIcon(FExtensionContentBrowserCustom::GetCustomIconStyleSetName(), "ContentBrowser.DeletionAssetIcon"),
        FExecuteAction::CreateRaw(this, &FContentBrowserExtensionModule::OnPathViewMenu_DeleteClearingFolderButtonClicked)
    );

    MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Advanced Deletion")),
        FText::FromString(TEXT("Displayed Assets List On Panel.\nSelect Assets to Delete")),
        FSlateIcon(FExtensionContentBrowserCustom::GetCustomIconStyleSetName(), "ContentBrowser.DeletionAssetIcon"),
        FExecuteAction::CreateRaw(this, &FContentBrowserExtensionModule::OnPathViewMenu_AdvancedDeletionButtonClicked)
    );
}

void FContentBrowserExtensionModule::OnPathViewMenu_DeleteClearingFolderButtonClicked()
{
    DeleteFolder_ClearingUnusedAssetsOnSelectedFolder();
}

void FContentBrowserExtensionModule::OnPathViewMenu_AdvancedDeletionButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(FName(TEXT("AdvanceDeletion")));
}

TSharedRef<FExtender> FContentBrowserExtensionModule::CustomAssetViewMenuExtender(const TArray<FAssetData>& SelectedAssets)
{
    BatchSelectedAssets = SelectedAssets;

    TSharedRef<FExtender> AssetViewMenuExtender(new FExtender());

    if (SelectedAssets.Num() > 0)
    {
        AssetViewMenuExtender->AddMenuExtension(
            FName(TEXT("Load")),
            EExtensionHook::After,
            TSharedPtr<FUICommandList>(),
            FMenuExtensionDelegate::CreateRaw(this, &FContentBrowserExtensionModule::AddAfterLoadOnAssetViewContentBrowserMenuEntry)
        );

        AssetViewMenuExtender->AddMenuExtension(
            FName(TEXT("Delete")),
            EExtensionHook::After,
            TSharedPtr<FUICommandList>(),
            FMenuExtensionDelegate::CreateRaw(this, &FContentBrowserExtensionModule::AddAfterDeleteOnAssetViewContentBrowserMenuEntry)
        );

        AssetViewMenuExtender->AddMenuExtension(
            FName(TEXT("Duplicate")),
            EExtensionHook::After,
            TSharedPtr<FUICommandList>(),
            FMenuExtensionDelegate::CreateRaw(this, &FContentBrowserExtensionModule::AddAfterDuplicateOnAssetViewContentBrowserMenuEntry)
        );
    }   

    return AssetViewMenuExtender;
}

void FContentBrowserExtensionModule::AddAfterLoadOnAssetViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Add Prefix")),
        FText::FromString(TEXT("Add Unreal Recommanded Prefix To Selected Asset")),
        FSlateIcon(),
        FExecuteAction::CreateRaw(this, &FContentBrowserExtensionModule::OnAssetViewMenu_AddPrefixButtonClicked)
    );
}

void FContentBrowserExtensionModule::AddAfterDeleteOnAssetViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Delete : Unused Asset")),
        FText::FromString(TEXT("Delete Asset By Checking Which Is Currently Unusing")),
        FSlateIcon(FExtensionContentBrowserCustom::GetCustomIconStyleSetName(), "ContentBrowser.DeletionAssetIcon"),
        FExecuteAction::CreateRaw(this, &FContentBrowserExtensionModule::OnAssetViewMenu_DeleteUnusedAssetButtonClicked)
    );
}

void FContentBrowserExtensionModule::AddAfterDuplicateOnAssetViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Duplicates : Multiple Copy")),
        FText::FromString(TEXT("Duplicates Selected Assets As Input Number Of Value")),
        FSlateIcon(FExtensionContentBrowserCustom::GetCustomIconStyleSetName(), "ContentBrowser.DuplicateAssetIcon"),
        FExecuteAction::CreateLambda([this]() 
        {
            TSharedRef<SWindow> InputWindow = ConstructSEditableTextBoxForGetInputNumOfDuplicate();
        })
    );
}

void FContentBrowserExtensionModule::OnAssetViewMenu_AddPrefixButtonClicked()
{
    AddPrefix();
}

void FContentBrowserExtensionModule::OnAssetViewMenu_DeleteUnusedAssetButtonClicked()
{
    DeleteUnusedAssetsOnSelectedFiles();
}

void FContentBrowserExtensionModule::InitPrefixMap()
{
    PrefixMap =
    {
        // General
        { UMaterial::StaticClass(), TEXT("M_") },
        { UMaterialInstanceConstant::StaticClass(), TEXT("MI_") },
        { UPhysicsAsset::StaticClass(), TEXT("PHYS_") },
        { UPhysicalMaterial::StaticClass(), TEXT("PM_") },
        { UTexture::StaticClass(), TEXT("T_") },
        { UStaticMesh::StaticClass(), TEXT("SM_") },
        { USkeletalMesh::StaticClass(), TEXT("SK_") },
        { UWorld::StaticClass(), TEXT("L_")},

        // Blueprints
        { UBlueprint::StaticClass(), TEXT("BP_") },
        { UAnimBlueprint::StaticClass(), TEXT("ABP_") },
        { UBlueprintGeneratedClass::StaticClass(), TEXT("BP_") },
        { UInterface::StaticClass(), TEXT("BI_") },
        { UUserWidget::StaticClass(), TEXT("WBP_")},
        { UUserWidgetBlueprint::StaticClass(), TEXT("WBP_") },
        { UActorComponent::StaticClass(), TEXT("AC_") },

        // Tables & Data
        { UCurveTable::StaticClass(), TEXT("CT_") },
        { UDataTable::StaticClass(), TEXT("DT_") },
        { UEnum::StaticClass(), TEXT("E_") },
        { UStruct::StaticClass(), TEXT("F_") },

        // Particle Effects
        { UNiagaraEmitter::StaticClass(), TEXT("NE_") },
        { UNiagaraSystem::StaticClass(), TEXT("NS_") },
        { UNiagaraFunctionLibrary::StaticClass(), TEXT("NFL_") },

        // Skeletal Animations
        { UControlRig::StaticClass(), TEXT("CR_") },
        { USkeleton::StaticClass(), TEXT("SKEL_") },
        { UAnimMontage::StaticClass(), TEXT("AM_") },
        { UAnimSequence::StaticClass(), TEXT("AS_") },
        { UBlendSpace::StaticClass(), TEXT("BS_") },
        { UBlendSpace1D::StaticClass(), TEXT("BS_") },

        // Animation
        { ULevelSequence::StaticClass(), TEXT("LS_") },
        { UObject::StaticClass(), TEXT("EDIT_") }, // Placeholder
    };
}

void FContentBrowserExtensionModule::DuplicateBatchAssets(int32 NumberOfDuplicates)
{
	if (NumberOfDuplicates < 0)
	{
		DebugUtilities::ShowDialog(EAppMsgType::Ok, TEXT("Please Enter Valid Number"));
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int i = 0; i < NumberOfDuplicates; i++)
		{
			const FString SelectedAssetPath = SelectedAssetData.GetObjectPathString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
			const FString NewPath = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SelectedAssetPath, NewPath))
			{
				UEditorAssetLibrary::SaveAsset(NewPath, false);
				++Counter;
			}
		}
	}
	
    if (Counter > 0) DebugUtilities::ShowNotifyInfo("Successfully Duplicated " + FString::FromInt(Counter) + " Files");
}

void FContentBrowserExtensionModule::AddPrefix()
{
    TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
    uint32 Counter = 0;

    for (UObject* SelectedAsset : SelectedAssets)
    {
        if (!SelectedAsset) continue;

        FString* PrefixToApply = PrefixMap.Find(SelectedAsset->GetClass());

        if (PrefixMap.IsEmpty() || !PrefixToApply)
        {
            DebugUtilities::ShowNotifyInfo(FString::Printf(TEXT("Failed To Find Prefix For %s"), *SelectedAsset->GetClass()->GetName()));
            continue;
        }

        FString OldName = SelectedAsset->GetName();

        if (OldName.StartsWith(*PrefixToApply))
        {
            DebugUtilities::ShowNotifyInfo(FString::Printf(TEXT("%s Has Already Applied Class Prefix"), *OldName));
            continue;
        }

        if (SelectedAsset->IsA<UMaterialInstanceConstant>())
        {
            OldName.RemoveFromStart(TEXT("M_"));
            OldName.RemoveFromEnd(TEXT("_Inst"));
        }

        const FString AssetNameWithPrefix = *PrefixToApply + OldName;

        UEditorUtilityLibrary::RenameAsset(SelectedAsset, AssetNameWithPrefix);

        ++Counter;
    }

    if (Counter > 0)
    {
        DebugUtilities::ShowNotifyInfo(TEXT("Successfully Renamed " + FString::FromInt(Counter) + " Assets"));
    }
}

void FContentBrowserExtensionModule::DeleteUnusedAssetsOnSelectedFiles()
{
    TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
    TArray<FAssetData> UnusedAssetsData;

    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        TArray<FString> AssetReferencer = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString());

        if (AssetReferencer.Num() == 0)
        {
            UnusedAssetsData.AddUnique(SelectedAssetData);
        }
    }

    if (UnusedAssetsData.Num() <= 0)
    {
        DebugUtilities::ShowNotifyInfo(FString::Printf(TEXT("No Unused Data Asset Found Among The Selected Files")));
    }

    // Module "UnrealEd" Need to Add On build.cs
    const int32 NumberOfDeletedAsset = ObjectTools::DeleteAssets(UnusedAssetsData, /*bShowConfirmation*/true);

    if (NumberOfDeletedAsset > 0)
    {
        DebugUtilities::ShowNotifyInfo(FString::FromInt(NumberOfDeletedAsset) + TEXT(" Unused Data Asset Deleted"));
    }
}

void FContentBrowserExtensionModule::DeleteFolder_ClearingUnusedAssetsOnSelectedFolder()
{
    if (FolderSelectedPaths.Num() > 1)
    {
        DebugUtilities::ShowDialog(EAppMsgType::Ok, TEXT("Can Not Be Proceeded By Selecting Multiple Folders"));
        return;
    }

    const FString DirectoryPath = FolderSelectedPaths[0];

    TArray<FString> AssetsPathNameArray = UEditorAssetLibrary::ListAssets(DirectoryPath, /*bRecursive*/ true, /*bIncludeFolder*/ true);

    if (AssetsPathNameArray.Num() == 0)
    {
        DebugUtilities::ShowNotifyInfo(TEXT("No Asset Found Under The Selected Folder"));
        return;
    }

    // Confirmation Dialog
    const FString ConfirmMsg = FString::FromInt(AssetsPathNameArray.Num()) + TEXT(" Assets Detected On The Selected Folder\nWould Like To Proceed?");
    EAppReturnType::Type ConfirmResult = DebugUtilities::ShowDialog(EAppMsgType::YesNo, ConfirmMsg, /*bShowMsgAsWarning*/ false);

    if (ConfirmResult == EAppReturnType::No) return;

    TArray<FAssetData> UnusedAssetsDataArray;

    for (const FString& AssetPathName : AssetsPathNameArray)
    {
        // Pass Root Folder Asset
        if (IsProtectedFolder(AssetPathName)) continue;

        // Pass If Path Folder Does Not Have Asset 
        if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName)) continue;

        TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName, /*bLoadAssetsToConfirm*/ false);

        if (AssetReferencers.Num() == 0)
        {
            const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
            UnusedAssetsDataArray.AddUnique(UnusedAssetData);
        }
    }

    int32 NumberOfDeleteAssets = 0;

    if (UnusedAssetsDataArray.Num() > 0)
    {
        NumberOfDeleteAssets = ObjectTools::DeleteAssets(UnusedAssetsDataArray);
    }

    TArray<FString> AllFolderPath;
    
    // Sort By Longest Length Of Path
    for (const FString& AssetPathName : AssetsPathNameArray)
    {
        FString FolderPathName = FPackageName::GetLongPackagePath(AssetPathName);
        AllFolderPath.AddUnique(FolderPathName);
    }
    
    AllFolderPath.Sort([](const FString& CurrentPath, const FString& NextPath) {return CurrentPath.Len() > NextPath.Len();});
    
    int32 NumberOfDeleteFolderCounter = 0;

    for (const FString& FolderPath : AllFolderPath)
    {
        if (UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
        {
            TArray<FString> AssetsInFolder = UEditorAssetLibrary::ListAssets(FolderPath,  /*bRecursive*/ true, /*bIncludeFolder*/ false);
            if (AssetsInFolder.Num() == 0)
            {
                if (UEditorAssetLibrary::DeleteDirectory(FolderPath))
                {
                    ++NumberOfDeleteFolderCounter;
                }
            }
        }
    }

    if (NumberOfDeleteAssets > 0 || NumberOfDeleteFolderCounter > 0)
    {
        FString ResultMsg = FString::Printf(TEXT("%d Unused Assets Are Deleted\n%d Empty Folders Are Deleted"), NumberOfDeleteAssets, NumberOfDeleteFolderCounter);
        DebugUtilities::ShowNotifyInfo(ResultMsg);
    }
    else
    {
        DebugUtilities::ShowNotifyInfo(TEXT("No Unused Assets Or Empty Folder Under The Selected Folders"));
    }
}

bool FContentBrowserExtensionModule::IsProtectedFolder(const FString& AssetPath)
{
    return AssetPath.Contains(TEXT("Developers")) ||
        AssetPath.Contains(TEXT("Collections")) ||
        AssetPath.Contains(TEXT("__ExternalActors__")) ||
        AssetPath.Contains(TEXT("__ExternalObjects__"));
}
#pragma endregion EnginExtension


#pragma region CustomSlateEditor
void FContentBrowserExtensionModule::RegisterCustomSlateEditorTab_AdvancedDeletion()
{
    static bool bIsTabRegistered = false;
    if (bIsTabRegistered) return;

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        FName(TEXT("AdvanceDeletion")),
        FOnSpawnTab::CreateRaw(this, &FContentBrowserExtensionModule::OnSpawnAdvancedDeletionTab))
        .SetDisplayName(FText::FromString("Advance Deletion"))
        .SetIcon(FSlateIcon(FExtensionContentBrowserCustom::GetCustomIconStyleSetName(), "ContentBrowser.DeletionAssetIcon"));

    bIsTabRegistered = true;
}
TSharedRef<SDockTab> FContentBrowserExtensionModule::OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return
        SNew(SDockTab).TabRole(ETabRole::NomadTab)
        [
            SNew(SAdvancedDeletionWidget)
                .AssetDataList(GetAssetDataListOnSelectedFolder())
        ];
}
TArray<TSharedPtr<FAssetData>> FContentBrowserExtensionModule::GetAssetDataListOnSelectedFolder()
{
    if (FolderSelectedPaths.IsEmpty()) return TArray<TSharedPtr<FAssetData>>();
    
    const FString& SelectedFolderPath = FolderSelectedPaths[0];
    TArray<TSharedPtr<FAssetData>> AvailableAssetsDataList;

    TArray<FString> AssetsPath = UEditorAssetLibrary::ListAssets(SelectedFolderPath);

    for (const FString& AssetPath : AssetsPath)
    {
        if (IsProtectedFolder(AssetPath)) continue;

        if (!UEditorAssetLibrary::DoesAssetExist(AssetPath)) continue;

        const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPath);

        AvailableAssetsDataList.AddUnique(MakeShared<FAssetData>(Data));
    }

    return AvailableAssetsDataList;
}
#pragma endregion CustomSlateEditor

#undef LOCTEXT_NAMESPACE	
IMPLEMENT_MODULE(FContentBrowserExtensionModule, ContentBrowserExtension)
