// Copyright Borka


#include "Character/AuraCharacter.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	AddStartupAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	return GetPlayerState<AAuraPlayerState>()->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	if ( const auto AuraPlayerState = GetPlayerState<AAuraPlayerState>())
	{
		AbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
		
		AttributeSet = AuraPlayerState->GetAttributeSet();
		
		AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
		AbilitySystemComponent->AbilityActorInfoSet();

		InitializeDefaultAttributes();
		
		if ( AAuraPlayerController* AuraPlayerController =  Cast<AAuraPlayerController>(GetController()) )
		{
			if ( AAuraHUD* AuraHud =  Cast<AAuraHUD>(AuraPlayerController->GetHUD()) )
			{
				AuraHud->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}
}	

