// Copyright 2020 Dan Kestranek.

#include "GASDocumentation/GASDocumentationGameMode.h"
#include "Engine/World.h"
#include "Characters/Heroes/GDHeroCharacter.h"
#include "Player/GDPlayerController.h"
#include "Player/GDPlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AGASDocumentationGameMode::AGASDocumentationGameMode()
{
	RespawnDelay = 5.0f;

	HeroClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASDocumentation/Characters/Hero/BP_HeroCharacter.BP_HeroCharacter_C"));
	if (!HeroClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find HeroClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}
}

// ヒーローが死んだら、死んだときのポーンを取り外して、新しく観戦のためのポーンをスポーンさせて、できたポーンをコントローラーにつける
// で、一定時間後にRespawnHeroを実行する
// もしプレイヤーのヒーローだったら、カウントダウンをセットする関数を実行する
void AGASDocumentationGameMode::HeroDied(AController* Controller)
{
	FActorSpawnParameters SpawnParameters;
	// アクターは (シェイプ コンポーネントに基づいて) 近くの衝突しない場所を見つけようとするが、見つからない場合でも常にスポーンする
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// SpawnActor()はアクターをスポーンして、スポーンしたアクターのポインターを返す
	// 観戦のためのポーンクラスを生成する
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	// コントローラーに対して、ポーンの所有を取り消す
	Controller->UnPossess();
	// Possess()はauthorityの場合のみ実行されるらしい
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	// 1つ目は実行する場所？、2つ目はバインドする関数ポインタ、3つ目は関数ポインタに渡す引数
	RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGASDocumentationGameMode::RespawnHero, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	// プレイヤーのコントローラーにキャストすることで、プレイヤーかどうかを判定、AIだったらカウントダウンいらないから
	AGDPlayerController* PC = Cast<AGDPlayerController>(Controller);
	if (PC)
	{
		PC->SetRespawnCountdown(RespawnDelay);
	}
}


// 敵ヒーローのスポーン地点を取得する
void AGASDocumentationGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get the enemy hero spawn point
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor->GetName() == FString("EnemyHeroSpawn"))
		{
			EnemySpawnPoint = Actor;
			break;
		}
	}
}

void AGASDocumentationGameMode::RespawnHero(AController * Controller)
{
	if (Controller->IsPlayerController())
	{
		// Respawn player hero
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGDHeroCharacter* Hero = GetWorld()->SpawnActor<AGDHeroCharacter>(HeroClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		// HeroDiedでつけた、観戦のポーンを取得して外して消す
		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
	else
	{
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGDHeroCharacter* Hero = GetWorld()->SpawnActor<AGDHeroCharacter>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);
		
		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
}
