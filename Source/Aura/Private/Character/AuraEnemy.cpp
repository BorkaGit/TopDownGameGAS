// Copyright Borka


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	AbilitySystemComponent	= CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet			= CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
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
