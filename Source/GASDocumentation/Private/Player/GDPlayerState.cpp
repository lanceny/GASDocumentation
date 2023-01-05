// Copyright 2020 Dan Kestranek.


#include "Player/GDPlayerState.h"
#include "Characters/Abilities/AttributeSets/GDAttributeSetBase.h"
#include "Characters/Abilities/GDAbilitySystemComponent.h"
#include "Characters/Heroes/GDHeroCharacter.h"
#include "Player/GDPlayerController.h"
#include "UI/GDFloatingStatusBarWidget.h"
#include "UI/GDHUDWidget.h"

AGDPlayerState::AGDPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	// ability system componentを作成し、明示的にレプリケートされるように設定する。
	AbilitySystemComponent = CreateDefaultSubobject<UGDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	// Mixedモードとは、自分自身へのGEのみ複製され、模擬プロキシにはGEが複製されないことを意味する
	// 他のGDPlayerState（Hero）がGEを受け取った場合、サーバーからそのことを知らされることはない。
	// Attributes、GameplayTags、GameplayCuesは、引き続き私たちにレプリケートされる
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	// attribute setを作成する、これはデフォルトで複製される
	// AbilitySystemComponentの所有するアクタのサブオブジェクトとしてそれを追加する
	// 自動的にAbilitySystemComponentにAttributeSetを登録される
	AttributeSetBase = CreateDefaultSubobject<UGDAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	// PlayerStateのNetUpdateFrequencyをCharacterと同じに設定する
	// デフォルトではPlayerStateの更新頻度が非常に低く、アビリティシステムに遅延が発生する
	// 100は出荷時の設定としては高すぎるかもしれませんが、必要に応じて調整する
	NetUpdateFrequency = 100.0f;

	// Cache tags
	// TagName に対応するFGameplayTagを取得
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent * AGDPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGDAttributeSetBase * AGDPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AGDPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AGDPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->ShowAbilityConfirmCancelText(ShowText);
		}
	}
}

float AGDPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AGDPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AGDPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float AGDPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AGDPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AGDPlayerState::GetManaRegenRate() const
{
	return AttributeSetBase->GetManaRegenRate();
}

float AGDPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AGDPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

float AGDPlayerState::GetStaminaRegenRate() const
{
	return AttributeSetBase->GetStaminaRegenRate();
}

float AGDPlayerState::GetArmor() const
{
	return AttributeSetBase->GetArmor();
}

float AGDPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

int32 AGDPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetCharacterLevel();
}

int32 AGDPlayerState::GetXP() const
{
	return AttributeSetBase->GetXP();
}

int32 AGDPlayerState::GetXPBounty() const
{
	return AttributeSetBase->GetXPBounty();
}

int32 AGDPlayerState::GetGold() const
{
	return AttributeSetBase->GetGold();
}

int32 AGDPlayerState::GetGoldBounty() const
{
	return AttributeSetBase->GetGoldBounty();
}

void AGDPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		// Attributeが変わったときのコールバックを定義する
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGDPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AGDPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &AGDPlayerState::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AGDPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &AGDPlayerState::MaxManaChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaRegenRateAttribute()).AddUObject(this, &AGDPlayerState::ManaRegenRateChanged);
		StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaAttribute()).AddUObject(this, &AGDPlayerState::StaminaChanged);
		MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxStaminaAttribute()).AddUObject(this, &AGDPlayerState::MaxStaminaChanged);
		StaminaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaRegenRateAttribute()).AddUObject(this, &AGDPlayerState::StaminaRegenRateChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).AddUObject(this, &AGDPlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).AddUObject(this, &AGDPlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(this, &AGDPlayerState::CharacterLevelChanged);

		// Tag change callbacks
		// GameplayTagsのデリゲート
		// NewOrRemovedを指定した場合,タグが新規作成されたとき、または完全に削除されたときのみイベントが発生します。
		// AnyCountChangeを指定した場合、タグの"count"が変更されるたびにイベントが発生
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGDPlayerState::StunTagChanged);
	}
}

void AGDPlayerState::HealthChanged(const FOnAttributeChangeData & Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	AGDHeroCharacter* Hero = Cast<AGDHeroCharacter>(GetPawn());
	if (Hero)
	{
		UGDFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint

	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (Hero)
		{
			Hero->Die();
		}
	}
}

void AGDPlayerState::MaxHealthChanged(const FOnAttributeChangeData & Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	AGDHeroCharacter* Hero = Cast<AGDHeroCharacter>(GetPawn());
	if (Hero)
	{
		UGDFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxHealth(MaxHealth);
		}
	}
}

void AGDPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData & Data)
{
	float HealthRegenRate = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHealthRegenRate(HealthRegenRate);
		}
	}
}

void AGDPlayerState::ManaChanged(const FOnAttributeChangeData & Data)
{
	float Mana = Data.NewValue;

	// Update floating status bar
	AGDHeroCharacter* Hero = Cast<AGDHeroCharacter>(GetPawn());
	if (Hero)
	{
		UGDFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / GetMaxMana());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AGDPlayerState::MaxManaChanged(const FOnAttributeChangeData & Data)
{
	float MaxMana = Data.NewValue;

	// Update floating status bar
	AGDHeroCharacter* Hero = Cast<AGDHeroCharacter>(GetPawn());
	if (Hero)
	{
		UGDFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(GetMana() / MaxMana);
		}
	}

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxMana(MaxMana);
		}
	}
}

void AGDPlayerState::ManaRegenRateChanged(const FOnAttributeChangeData & Data)
{
	float ManaRegenRate = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetManaRegenRate(ManaRegenRate);
		}
	}
}

void AGDPlayerState::StaminaChanged(const FOnAttributeChangeData & Data)
{
	float Stamina = Data.NewValue;

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AGDPlayerState::MaxStaminaChanged(const FOnAttributeChangeData & Data)
{
	float MaxStamina = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxStamina(MaxStamina);
		}
	}
}

void AGDPlayerState::StaminaRegenRateChanged(const FOnAttributeChangeData & Data)
{
	float StaminaRegenRate = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetStaminaRegenRate(StaminaRegenRate);
		}
	}
}

void AGDPlayerState::XPChanged(const FOnAttributeChangeData & Data)
{
	float XP = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetExperience(XP);
		}
	}
}

void AGDPlayerState::GoldChanged(const FOnAttributeChangeData & Data)
{
	float Gold = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetGold(Gold);
		}
	}
}

void AGDPlayerState::CharacterLevelChanged(const FOnAttributeChangeData & Data)
{
	float CharacterLevel = Data.NewValue;

	// Update the HUD
	AGDPlayerController* PC = Cast<AGDPlayerController>(GetOwner());
	if (PC)
	{
		UGDHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHeroLevel(CharacterLevel);
		}
	}
}

// スタンしたときのコールバック関数
void AGDPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		// スタンされたことによって止めるべきアビリティを示すタグをコンテナに入れる
		// 因みに複数のタグを扱うときは配列よりもコンテナの方が効率が良い
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		// スタンされたことを無視できるアビリティを示すタグをコンテナに入れる
		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		// 指定されたタグを持つすべてのアビリティをキャンセルします。無視インスタンスをキャンセルしません
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
