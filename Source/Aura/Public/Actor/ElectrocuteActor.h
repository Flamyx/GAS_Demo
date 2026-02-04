// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "ElectrocuteActor.generated.h"

UCLASS()
class AURA_API AElectrocuteActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectrocuteActor();
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"));
	FGameplayEffectSpecHandle InfiniteDamageEffectSpecHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void OnCapsuleOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable)
	void OnCapsuleEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);
	
	// UPROPERTY(EditAnywhere)
	// UCapsuleComponent* CapsuleComponent;
	
private:
	TArray<FActiveGameplayEffectHandle> ActiveEffectHandles;
	
	UAbilitySystemComponent* GetTargetASCFromActiveEffect(FActiveGameplayEffectHandle EffectHandle);
	AActor* GetInstigatorActorFromActiveEffect(FActiveGameplayEffectHandle EffectHandle);
	
};
