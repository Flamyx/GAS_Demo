// Copyright Sparrow Inc.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "MeteoriteProjectile.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class AURA_API AMeteoriteProjectile : public AAuraProjectile
{
	GENERATED_BODY()

public:
	AMeteoriteProjectile();

	void SetSphereRadius(float ChargeRatio);

protected:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere)
	float BlastRadius;
	UPROPERTY(EditAnywhere)
	float MaxBlastRadius;
};
