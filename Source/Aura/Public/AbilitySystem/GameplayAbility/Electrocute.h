// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraDamageGameplayAbility.h"
#include "Electrocute.generated.h"

class AElectrocuteActor;
/**
 * 
 */
UCLASS()
class AURA_API UElectrocute : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
							const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	UPROPERTY(BlueprintReadWrite)
	bool bInputReleased = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AElectrocuteActor> ElectrocuteActorClass;
private:
	TObjectPtr<AElectrocuteActor> ElectricBeam;
};
