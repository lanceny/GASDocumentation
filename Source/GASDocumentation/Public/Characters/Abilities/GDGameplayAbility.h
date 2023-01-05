// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASDocumentation/GASDocumentation.h"
#include "GDGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGDGameplayAbility();

	// Abilities with this set will automatically activate when the input is pressed
	// このセットを持つアビリティは、入力が押されたときに自動的に起動します
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGDAbilityInputID AbilityInputID = EGDAbilityInputID::None;

	// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	// 自動的に起動する入力に結びつけずに、能力をスロットに関連付けるための値です。
	// パッシブアビリティは入力に結びつかないので、アビリティとスロットを一般的に関連付ける方法が必要です。
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGDAbilityInputID AbilityID = EGDAbilityInputID::None;

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	// 能力が付与されたら即座に発動するように指示します。パッシブアビリティや他人に強制されるアビリティに使用します。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	// 能力が「ActivateAbilityOnGranted」と記されている場合、ここで与えられたら即座にそれらを発動させる。
	// エピックからのコメントです。プロジェクトでは、ここでパッシブを開始したり、他の「BeginPlay」タイプのロジックを実行したい場合があります。
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
