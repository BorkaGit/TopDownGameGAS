// Copyright Borka


#include "Character/AuraEnemy.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	AbilitySystemComponent	= CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet			= CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
	HealthBar               = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	
	HealthBar->SetupAttachment(GetRootComponent());
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

	if ( UAuraUserWidget* AuraUserWidget =  Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()) )
	{
		AuraUserWidget->SetWidgetController(this);
	}
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnHealthChanged.Broadcast(Data.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnMaxHealthChanged.Broadcast(Data.NewValue);
	});

	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
}

void AAuraEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeDefaultAttributes();
}

void AAuraEnemy::HighlightActor()
{
	ChangeHighlightState(true);
}

void AAuraEnemy::UnHighlightActor()
{
	ChangeHighlightState(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::ChangeHighlightState(bool bNewState) const
{
	GetMesh()->SetRenderCustomDepth(bNewState);
	Weapon->SetRenderCustomDepth(bNewState);
	if (bNewState)
	{
		GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
		Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
}
