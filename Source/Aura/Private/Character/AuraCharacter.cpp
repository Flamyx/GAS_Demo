// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"

AAuraCharacter::AAuraCharacter()
{
	LevelUpComponent = CreateDefaultSubobject<UNiagaraComponent>("Level Up Component");
	LevelUpComponent->SetupAttachment(GetRootComponent());
	LevelUpComponent->bAutoActivate = false;

	GetCharacterMovement()->RotationRate - FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll =  false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
		
}
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	//Init ability actor info for the client
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(float IncomingXP)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->GrantXP(IncomingXP);
}

void AAuraCharacter::AddToLevel_Implementation(int32 NumLevelUps)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->AddToLevel(NumLevelUps);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 inSpellPoints)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->AddToSpellPoints(inSpellPoints);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 inAttributePoints)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->AddToAttributePoints(inAttributePoints);
}

int32 AAuraCharacter::GetXP_Implementation()
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->GetXP();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(float inXP)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);

	return PS->FindLevelForXP(inXP);
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 inLevel)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	auto LevelUpInfo = PS->GetLevelUpInfo(inLevel);
	return LevelUpInfo.SpellPointReward;
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 inLevel)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	auto LevelUpInfo = PS->GetLevelUpInfo(inLevel);
	return LevelUpInfo.AttributePointReward;
}

void AAuraCharacter::LevelUp_Implementation()
{
	MultiCastLevelUpParticles();
	/*AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->SetLevel(inLevel);*/
	//TODO add niagara effect
}

void AAuraCharacter::RecalculateSecondaryAttributes_Implementation()
{
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
}

void AAuraCharacter::MultiCastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpComponent))
	{
		LevelUpComponent->Activate(true);
	}
}


int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	if (AuraPlayerController)
	{
		AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD());
		if (AuraHUD)
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitializeDefaultAttributes();
}

void AAuraCharacter::BindToLevelUp()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
}
