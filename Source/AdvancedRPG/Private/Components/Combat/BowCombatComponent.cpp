// pravin's patashala all Rights Reserved

#include "Components/Combat/BowCombatComponent.h"
#include "Items/Weapons/WarriorArrow.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Characters/WarriorBowCharacter.h"
#include "Components/SkeletalMeshComponent.h"  
#include "Engine/SkeletalMeshSocket.h" 
#include "WarriorDebugHelper.h"

AWarriorArrow* UBowCombatComponent::GetBowCarriedArrowByTag(FGameplayTag InArrowTagToGet) const
{
	return Cast<AWarriorArrow>(GetCharacterCarriedWeaponByTag(InArrowTagToGet));
}

// ============================================
// 🔥 CRITICAL FIX: Matches base class signature exactly
// Base: void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false)
// ============================================
void UBowCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("=== BOW RegisterSpawnedWeapon ==="));
	UE_LOG(LogTemp, Warning, TEXT("Weapon: %s"), InWeaponToRegister ? *InWeaponToRegister->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Tag: %s"), *InWeaponTagToRegister.ToString());
	UE_LOG(LogTemp, Warning, TEXT("bEquip: %s"), bRegisterAsEquippedWeapon ? TEXT("TRUE") : TEXT("FALSE"));

	if (!InWeaponToRegister)
	{
		UE_LOG(LogTemp, Error, TEXT("BowCombatComponent::RegisterSpawnedWeapon: Weapon is NULL!"));
		return;
	}

	// Store in the base class weapon map
	// Call SUPER first to ensure base class map is populated
	Super::RegisterSpawnedWeapon(InWeaponTagToRegister, InWeaponToRegister, bRegisterAsEquippedWeapon);

	UE_LOG(LogTemp, Warning, TEXT("BowCombatComponent: Registered weapon '%s' with tag '%s'"),
		*InWeaponToRegister->GetName(), *InWeaponTagToRegister.ToString());

	// If this is the equipped weapon, do bow-specific setup
	if (bRegisterAsEquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("BowCombatComponent: Setting up equipped weapon '%s'"), *InWeaponToRegister->GetName());

		// Attach weapon to bow character mesh
		if (AWarriorBowCharacter* BowChar = Cast<AWarriorBowCharacter>(GetOwningPawn()))
		{
			if (USkeletalMeshComponent* Mesh = BowChar->GetMesh())
			{
				// Detach first to ensure clean state
				InWeaponToRegister->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

				// Get socket name from helper
				FName SocketName = GetWeaponAttachSocketName(InWeaponTagToRegister);

				InWeaponToRegister->AttachToComponent(
					Mesh,
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					SocketName);

				UE_LOG(LogTemp, Warning, TEXT("BowCombatComponent: Attached weapon to socket '%s' on mesh '%s'"),
					*SocketName.ToString(), *Mesh->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("BowCombatComponent: BowChar mesh is NULL!"));
			}

			// Set proper ownership for GAS
			InWeaponToRegister->SetOwner(BowChar);
			InWeaponToRegister->SetInstigator(BowChar);

			UE_LOG(LogTemp, Warning, TEXT("BowCombatComponent: Set owner to '%s', instigator to '%s'"),
				*BowChar->GetName(), *BowChar->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BowCombatComponent: GetOwningPawn() is not AWarriorBowCharacter!"));
		}

		// Make visible and enable collision
		InWeaponToRegister->SetActorHiddenInGame(false);
		InWeaponToRegister->SetActorEnableCollision(true);
	}
}

FName UBowCombatComponent::GetWeaponAttachSocketName(FGameplayTag WeaponTag) const
{
	// If the configured socket doesn't exist on the mesh, log a warning
	if (AWarriorBowCharacter* BowChar = Cast<AWarriorBowCharacter>(GetOwningPawn()))
	{
		if (USkeletalMeshComponent* Mesh = BowChar->GetMesh())
		{
			if (!Mesh->DoesSocketExist(WeaponAttachSocketName))
			{
				UE_LOG(LogTemp, Error, TEXT("Socket '%s' does NOT exist on mesh '%s'! Weapon will float in world!"),
					*WeaponAttachSocketName.ToString(), *Mesh->GetName());

				// List available sockets for debugging
				// Get sockets from the SkeletalMesh asset, not the component
				if (USkeletalMesh* SkelMesh = Mesh->GetSkeletalMeshAsset())
				{
					TArray<USkeletalMeshSocket*> Sockets = SkelMesh->GetMeshOnlySocketList();
					for (const USkeletalMeshSocket* Socket : Sockets)
					{
						if (Socket)
						{
							FString SocketStr = Socket->SocketName.ToString();
							Debug::Print(FString::Printf(TEXT("  Available socket: %s"), *SocketStr));
						}
					}
				}
			}
		}
	}
	return WeaponAttachSocketName;
}

AWarriorArrow* UBowCombatComponent::GetBowCurrentEquippedArrow() const
{
	return Cast<AWarriorArrow>(GetCharacterCurrentEquippedWeapon());
}

void UBowCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// Safety check
	if (!HitActor || HitActor == GetOwningPawn())
	{
		return;
	}

	// Prevent multi-hit (same as hero)
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	const FVector HitLocation = HitActor->GetActorLocation();

	// Play arrow hit effects
	if (AWarriorArrow* CurrentArrow = GetBowCurrentEquippedArrow())
	{
		if (UNiagaraSystem* HitEffect = ArrowHitSparkEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				HitEffect,
				HitLocation,
				GetOwningPawn()->GetActorRotation()
			);
		}

		if (USoundBase* HitSound = ArrowHitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				HitLocation
			);
		}
	}

	// Send hit event to owning pawn (bow character)
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Shared_Event_MeeleHit,
		Data
	);
}

void UBowCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("BowCombatComponent: Arrow pulled from target %s"), *InteractedActor->GetName());
}