#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FContentBrowserExtensionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// OnClick Function
	void SyncContentBrowserDirectToClickedAssetLocation(const FString& AssetPathToSync);

	// Delete Function
	bool DeleteSingleAsset(const FAssetData& AssetDataToDelete);
	bool DeleteAllSelectedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataArrayToDelete);
	bool DeleteUnusedAssetsAmongSelectedAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataArrayToDelete, TArray<TSharedPtr<FAssetData>>& AssetDataListOnSelectedFolder);

	// List Function
	void GetUnusedAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssetList);
	void GetHaveSameNameAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssetList);

	void GetBlueprintTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetStaticMeshTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetSkeletalMeshTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetWorldLevelTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	
	void GetPhysicsTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetPhysicsMaterialTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetTextureTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetMaterialTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetMaterialInstanceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);

	void GetAnimBlueprintTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetInterfaceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetUserWidgetTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetActorComponentTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);

	void GetCurveTableTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetDataTableTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetEnumDataTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetStructDataTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);

	void GetNiagaraEmitterTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetNiagaraSystemTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetNiagaraFunctionLibraryTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);

	void GetControlRigTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetSkeletonTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetAnimMontageTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetAnimSequenceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	void GetBlendSpaceTypeAssetList(const TArray<TSharedPtr<FAssetData>>& DataToFilter, TArray<TSharedPtr<FAssetData>>& OutBlueprintAssetList);
	
#pragma region EngineExtension
protected:
	void InitCustomContentBrowserExtension();
	// Slate Widget Construct
	TSharedRef<SWindow> ConstructSEditableTextBoxForGetInputNumOfDuplicate();

	// Delegate Bind Function
	TSharedRef<FExtender> CustomPathViewMenuExtender(const TArray<FString>& SelectedPaths);
	void AddAfterDeleteOnPathViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	void OnPathViewMenu_DeleteClearingFolderButtonClicked();
	void OnPathViewMenu_AdvancedDeletionButtonClicked();

	TSharedRef<FExtender> CustomAssetViewMenuExtender(const TArray<FAssetData>& SelectedAssets);
	void AddAfterLoadOnAssetViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	void AddAfterDeleteOnAssetViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	void AddAfterDuplicateOnAssetViewContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	void OnAssetViewMenu_AddPrefixButtonClicked();
	void OnAssetViewMenu_DeleteUnusedAssetButtonClicked();

	void InitPrefixMap();
	void DuplicateBatchAssets(int32 NumberOfDuplicates);
	void AddPrefix();
	void DeleteUnusedAssetsOnSelectedFiles();
	void DeleteFolder_ClearingUnusedAssetsOnSelectedFolder();

private:
	TMap<UClass*, FString> PrefixMap;
	TArray<FString> FolderSelectedPaths;
	TArray<FAssetData> BatchSelectedAssets;

	bool IsProtectedFolder(const FString& AssetPath);
#pragma endregion EnginExtension


#pragma region CustomSlateEditor
protected:
	void RegisterCustomSlateEditorTab_AdvancedDeletion();

	TSharedRef<SDockTab> OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAssetDataListOnSelectedFolder();
#pragma endregion CustomSlateEditor
};
