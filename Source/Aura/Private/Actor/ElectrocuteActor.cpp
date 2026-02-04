// Copyright Sparrow Inc.


#include "Actor/ElectrocuteActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


// Sets default values
AElectrocuteActor::AElectrocuteActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	// SetRootComponent(CapsuleComponent);
	//
	// CapsuleComponent->SetCapsuleSize(25.f, 75.f); 
	// CapsuleComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	// FRotator Rotation = FRotator(90.f, 0.f, 0.f);
	// CapsuleComponent->SetRelativeRotation(Rotation);
	//
	// CapsuleComponent->SetGenerateOverlapEvents(true);
	// CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AElectrocuteActor::OnCapsuleOverlap);
	// CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AElectrocuteActor::OnCapsuleEndOverlap);
	
}

// Called when the game starts or when spawned
void AElectrocuteActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AElectrocuteActor::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!InfiniteDamageEffectSpecHandle.IsValid())
		return;
	
	FGameplayEffectSpec* EffectSpec = InfiniteDamageEffectSpecHandle.Data.Get();
	AActor* Avatar = EffectSpec->GetEffectContext().GetEffectCauser();
	
	if (!Avatar || !UAuraAbilitySystemLibrary::IsNotFriend(Avatar, OtherActor) || IsPendingKillPending())
		return;
	
	if (HasAuthority())
	{
		
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			FActiveGameplayEffectHandle EffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec);
			ActiveEffectHandles.Add(EffectHandle);
		}
	}
}

void AElectrocuteActor::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	for (auto ActiveEffectHandle : ActiveEffectHandles)
	{
		UAbilitySystemComponent* TargetASC = GetTargetASCFromActiveEffect(ActiveEffectHandle);
		if (TargetASC && TargetASC->GetOwnerActor() == OtherActor)
		{	
			TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
			return;
		}
	}
}

UAbilitySystemComponent* AElectrocuteActor::GetTargetASCFromActiveEffect(FActiveGameplayEffectHandle ActiveEffectHandle)
{
	if (ActiveEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* TargetASC = ActiveEffectHandle.GetOwningAbilitySystemComponent())
		{
			return TargetASC;
		}
	}
	
	return nullptr;
}

AActor* AElectrocuteActor::GetInstigatorActorFromActiveEffect(FActiveGameplayEffectHandle ActiveEffectHandle)
{
	if (ActiveEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* TargetASC = ActiveEffectHandle.GetOwningAbilitySystemComponent())
		{
			// Get the active gameplay effect
			if (const FActiveGameplayEffect* ActiveGE = TargetASC->GetActiveGameplayEffect(ActiveEffectHandle))
			{
				// Now, get the spec
				const FGameplayEffectSpec& Spec = ActiveGE->Spec;
				// The instigator is stored in the spec.
				AActor* InstigatorActor = Spec.GetEffectContext().GetInstigator();
				
				return InstigatorActor;
			}
		}
	}
	
	return nullptr;
}
