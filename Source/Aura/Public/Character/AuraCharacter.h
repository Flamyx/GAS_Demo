// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "UI/Overlay/AuraHUD.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"


class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:

	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Player interface */
	virtual void AddToXP_Implementation(float IncomingXP) override;
	virtual void AddToLevel_Implementation(int32 NumLevelUps) override;
	virtual void AddToSpellPoints_Implementation(int32 inSpellPoints) override;
	virtual void AddToAttributePoints_Implementation(int32 inAttributePoints) override;
	virtual int32 GetXP_Implementation() override;
	virtual int32 FindLevelForXP_Implementation(float inXP) override;
	virtual int32 GetSpellPointsReward_Implementation(int32 inLevel) override;
	virtual int32 GetAttributePointsReward_Implementation(int32 inLevel) override;
	virtual void LevelUp_Implementation();
	virtual void RecalculateSecondaryAttributes_Implementation() override;
	virtual int32 GetAttributePoints_Implementation() override;
	virtual int32 GetSpellPoints_Implementation() override;
	/* End player interface */

	/* Combat interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	/* End combat interface */

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> LevelUpComponent;

	virtual void InitAbilityActorInfo() override;

	void BindToLevelUp();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastLevelUpParticles() const;
};
