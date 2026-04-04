// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

// Sets default values
AAuraProjectile::AAuraProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere); 

	Sphere->SetGenerateOverlapEvents(true);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(MaxLifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this,
	                                      ImpactSound,
	                                      GetActorLocation(),
	                                      FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent) LoopingSoundComponent->Stop();
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority()) OnHit();

	Super::Destroyed();
}

void AAuraProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//For multiplayer "As client" version
	auto SourceActor = DamageEffectParams.SourceASC->GetAvatarActor();
	if ( SourceActor == OtherActor) return;

	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceActor, OtherActor)) return;

	if (!bHit) OnHit();
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetASC = TargetASC;
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			UAuraAbilitySystemLibrary::ApplyDamageEffectParams(DamageEffectParams);
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}


