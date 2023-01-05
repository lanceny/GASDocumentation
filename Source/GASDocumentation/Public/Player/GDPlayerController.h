// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Characters/GDCharacterBase.h"
#include "GDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API AGDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void CreateHUD();

	UPROPERTY(EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<class UGDDamageTextWidgetComponent> DamageNumberClass;

	class UGDHUDWidget* GetHUD();

	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AGDCharacterBase* TargetCharacter);
	void ShowDamageNumber_Implementation(float DamageAmount, AGDCharacterBase* TargetCharacter);
	bool ShowDamageNumber_Validate(float DamageAmount, AGDCharacterBase* TargetCharacter);

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	// GameModeからRespawnまでのカウントダウンをクライアントにRPCする簡単な方法。サーバーと同期していないため、レイテンシーが大きくなる。
	// Clientをつけてるから、サーバー側で実行されたら、サーバー側では実行せず、client側で実行する
	UFUNCTION(Client, Reliable, WithValidation)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	// UFUNCTIONで WithValidationを指定した場合、_Validateのサフィックスがついたこの関数がtrueを返せば、_Implementationの実行が許可され
	// falseを返すと実行が阻止されるようなバリデーションを定義できる
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<class UGDHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASDocumentation|UI")
	class UGDHUDWidget* UIHUDWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;
};
