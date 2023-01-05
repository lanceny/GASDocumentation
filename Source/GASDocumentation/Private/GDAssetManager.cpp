// Copyright 2020 Dan Kestranek.


#include "GDAssetManager.h"
#include "AbilitySystemGlobals.h"

// AssetManagerを返す
// Asset Managerは、アセット管理システムで、この仕組みを使うことで「アセットの非同期ロード」と「DLCのID管理」を効率よく行うことが可能
// PrimaryAssetsのロードとアンロード、およびゲーム固有のアセット参照の維持を担当するシングルトン
UGDAssetManager& UGDAssetManager::Get() 
{
	UGDAssetManager* Singleton = Cast<UGDAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GDAssetManager!"));
		return *NewObject<UGDAssetManager>();	 // never calls this
	}
}


void UGDAssetManager::StartInitialLoading() 
{
	Super::StartInitialLoading();

	// これは一度は呼んどかないとあかん感じのやつ
	// GAS使うなら脳死コピペで良さそう
	UAbilitySystemGlobals::Get().InitGlobalData();
}
