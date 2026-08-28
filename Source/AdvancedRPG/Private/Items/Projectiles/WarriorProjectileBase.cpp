// pravin's patashala all Rights Reserved 

#include "Items/Projectiles/WarriorProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "warriorGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraSystem.h"

AWarriorProjectileBase::AWarriorProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>("ProjectileCollisionBox");
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);


	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("ProjectileNiagaraComponent");
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());


	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
}

void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileDamagePolicy == EWarriorProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}
	OverlappedActors.AddUnique(OtherActor);

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = this;
		Data.Target = HitPawn;

		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{

			HandleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	if (!ProjectileDamageEffectSpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile %s: Invalid DamageEffectSpecHandle! Skipping damage."), *GetActorNameOrLabel());
		return;
	}

	const bool bWasApplied = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WarriorGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}





void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

		APawn* HitPawn = Cast<APawn>(OtherActor);

		if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{


			Destroy();
			return;
		}

		bool bIsValidBlock = false;

		const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn, WarriorGameplayTags::Player_Status_Blocking);

		if (bIsPlayerBlocking)
		{
			bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this, HitPawn);
		}

		FGameplayEventData Data;
		Data.Instigator = this;
		Data.Target = HitPawn;

		if (bIsValidBlock)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				HitPawn,
				WarriorGameplayTags::Player_Event_SuccessfulBlock,
				Data
			);
		}
		else
		{
			HandleApplyProjectileDamage(HitPawn, Data);
		}

		Destroy();
	}
}

// ===== NEW SETTER METHODS =====

void AWarriorProjectileBase::SetProjectileVelocity(const FVector& InVelocity)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = InVelocity;
	}
}

void AWarriorProjectileBase::SetProjectileSpeed(float InInitialSpeed, float InMaxSpeed)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->InitialSpeed = InInitialSpeed;
		ProjectileMovementComp->MaxSpeed = InMaxSpeed;
	}
}

void AWarriorProjectileBase::SetProjectileGravityScale(float InGravityScale)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->ProjectileGravityScale = InGravityScale;
	}
}

void AWarriorProjectileBase::SetDamageEffectSpecHandle(const FGameplayEffectSpecHandle& InSpecHandle)
{
	ProjectileDamageEffectSpecHandle = InSpecHandle;
}

void AWarriorProjectileBase::SetNiagaraEffect(UNiagaraSystem* InNiagaraSystem)
{
	if (ProjectileNiagaraComponent && InNiagaraSystem)
	{
		ProjectileNiagaraComponent->SetAsset(InNiagaraSystem);
	}
}

void AWarriorProjectileBase::ActivateNiagaraComponent(bool bReset)
{
	if (ProjectileNiagaraComponent)
	{
		ProjectileNiagaraComponent->Activate(bReset);
	}
}

void AWarriorProjectileBase::SetHomingTarget(USceneComponent* InTarget, float InAccelerationMagnitude)
{
	if (ProjectileMovementComp && InTarget)
	{
		ProjectileMovementComp->bIsHomingProjectile = true;
		ProjectileMovementComp->HomingTargetComponent = InTarget;
		ProjectileMovementComp->HomingAccelerationMagnitude = InAccelerationMagnitude;
	}
}

void AWarriorProjectileBase::ClearHomingTarget()
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->bIsHomingProjectile = false;
		ProjectileMovementComp->HomingTargetComponent = nullptr;
	}
}
