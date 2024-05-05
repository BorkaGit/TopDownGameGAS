// Copyright Borka


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& EffectType)
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetAbilitySystemComponent) return;
	
	FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystemComponent->MakeOutgoingSpec(EffectType.GameplayEffectClass,ActorLevel, EffectContextHandle);
	const TSharedPtr<FActiveGameplayEffectHandle> ActiveEffectHandle = MakeShared<FActiveGameplayEffectHandle>(TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data));

	if ( EffectSpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite && EffectType.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap )
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystemComponent);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	for ( auto& Effect : Effects )
	{
		if ( Effect.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap )
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	for ( auto& Effect : Effects )
	{
		if ( Effect.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap )
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
		else if ( Effect.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap )
		{
			UAbilitySystemComponent* TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetAbilitySystemComponent)) return;

			TArray<TSharedPtr<FActiveGameplayEffectHandle>> HandlesToRemove;
		
			for ( auto HandlePair : ActiveEffectHandles )
			{
				if ( TargetAbilitySystemComponent == HandlePair.Value )
				{
					TargetAbilitySystemComponent->RemoveActiveGameplayEffect(*HandlePair.Key);
					HandlesToRemove.Add(HandlePair.Key);
				}
			}
			for ( auto Handle : HandlesToRemove )
			{
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}
		}
	}
}