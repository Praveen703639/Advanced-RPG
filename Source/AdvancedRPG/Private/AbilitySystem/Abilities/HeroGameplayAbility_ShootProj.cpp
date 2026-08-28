// pravin's patashala all Rights Reserved

#include "AbilitySystem/Abilities/HeroGameplayAbility_ShootProj.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "Widgets/InGameWidgets/WarriorWidgetBase.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "EnhancedInputSubsystems.h"
#include "WarriorFunctionLibrary.h"
#include "warriorGameplayTags.h"

// =============================================================================
// ACTIVATE / END
// =============================================================================

void UHeroGameplayAbility_ShootProj::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AWarriorHeroController* HeroPC = GetWarriorHeroControllerFromActorInfo();
	if (!HeroPC || !HeroPC->IsLocalController())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Initialize aim location via camera forward trace so it starts on a surface
	// instead of floating in mid-air. This gives an "arrow-worthy" initial target.
	FVector CameraLocation;
	FRotator CameraRotation;
	HeroPC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * AimMaxDistance;
	FCollisionQueryParams InitParams;
	InitParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	FHitResult InitHit;
	if (GetWorld()->LineTraceSingleByChannel(InitHit, CameraLocation, TraceEnd, ECC_Visibility, InitParams))
	{
		CurrentAimWorldLocation = InitHit.ImpactPoint;
	}
	else
	{
		CurrentAimWorldLocation = TraceEnd;
	}

	// Clamp to valid distance from player
	if (AWarriorHeroCharacter* HeroChar = GetWarriorHeroCharacterFromActorInfo())
	{
		const FVector PlayerLocation = HeroChar->GetActorLocation();
		FVector ToAim = CurrentAimWorldLocation - PlayerLocation;
		const float Dist = ToAim.Size();
		if (Dist < AimMinDistance)
		{
			CurrentAimWorldLocation = PlayerLocation + ToAim.GetSafeNormal() * AimMinDistance;
		}
		else if (Dist > AimMaxDistance)
		{
			CurrentAimWorldLocation = PlayerLocation + ToAim.GetSafeNormal() * AimMaxDistance;
		}
	}

	InitAimMappingContext();
	DrawAimWidget();
	SetAimWidgetPosition();

	// NOTE: We do NOT use WaitInputRelease here. The ability stays active
	// until you call FinishAiming() from Blueprint (your toggle/shoot logic).
}

void UHeroGameplayAbility_ShootProj::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ResetAimMappingContext();
	CleanUpAimPhase();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// =============================================================================
// AIM TICK & MOVE
// =============================================================================

void UHeroGameplayAbility_ShootProj::OnAimTick(float DeltaTime)
{
	if (!GetWarriorHeroCharacterFromActorInfo() ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetWarriorHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Death))
	{
		CancelAiming();
		return;
	}

	UpdateAimLocation(DeltaTime);
	SetAimWidgetPosition();

	CurrentAimInput = FVector2D::ZeroVector;
}

void UHeroGameplayAbility_ShootProj::MoveAim(const FVector2D& InInputDirection)
{
	CurrentAimInput = InInputDirection;
}

// =============================================================================
// BLUEPRINT HELPERS
// =============================================================================

FVector UHeroGameplayAbility_ShootProj::GetAimTargetLocation() const
{
	return CurrentAimWorldLocation;
}

void UHeroGameplayAbility_ShootProj::FinishAiming()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHeroGameplayAbility_ShootProj::CancelAiming()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

// =============================================================================
// AIM HELPERS
// =============================================================================

void UHeroGameplayAbility_ShootProj::InitAimMappingContext()
{
	if (!AimInputMappingContext) return;

	AWarriorHeroController* HeroPC = GetWarriorHeroControllerFromActorInfo();
	if (!HeroPC) return;

	const ULocalPlayer* LocalPlayer = HeroPC->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem) return;

	Subsystem->AddMappingContext(AimInputMappingContext, 4);
}

void UHeroGameplayAbility_ShootProj::ResetAimMappingContext()
{
	if (!AimInputMappingContext) return;

	AWarriorHeroController* HeroPC = GetWarriorHeroControllerFromActorInfo();
	if (!HeroPC) return;

	const ULocalPlayer* LocalPlayer = HeroPC->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem) return;

	Subsystem->RemoveMappingContext(AimInputMappingContext);
}

void UHeroGameplayAbility_ShootProj::DrawAimWidget()
{
	if (!AimWidgetClass) return;

	AWarriorHeroController* HeroPC = GetWarriorHeroControllerFromActorInfo();
	if (!HeroPC) return;

	if (!DrawnAimWidget)
	{
		DrawnAimWidget = CreateWidget<UWarriorWidgetBase>(HeroPC, AimWidgetClass);
		if (DrawnAimWidget)
		{
			DrawnAimWidget->AddToViewport();
		}
	}
}

void UHeroGameplayAbility_ShootProj::SetAimWidgetPosition()
{
	if (!DrawnAimWidget) return;

	AWarriorHeroController* HeroPC = GetWarriorHeroControllerFromActorInfo();
	if (!HeroPC) return;

	FVector2D WidgetScreenPosition;
	if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(HeroPC, CurrentAimWorldLocation, WidgetScreenPosition, true))
	{
		return;
	}

	if (AimWidgetSize == FVector2D::ZeroVector)
	{
		DrawnAimWidget->WidgetTree->ForEachWidget([&](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					AimWidgetSize.X = FoundSizeBox->GetWidthOverride();
					AimWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			});
	}

	WidgetScreenPosition -= (AimWidgetSize / 2.f);
	DrawnAimWidget->SetPositionInViewport(WidgetScreenPosition, false);
}

void UHeroGameplayAbility_ShootProj::UpdateAimLocation(float DeltaTime)
{
	AWarriorHeroController* HeroPC = GetWarriorHeroControllerFromActorInfo();
	AWarriorHeroCharacter* HeroChar = GetWarriorHeroCharacterFromActorInfo();
	if (!HeroPC || !HeroChar) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	HeroPC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector CameraRight = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);
	const FVector CameraUp = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Z);

	FVector MoveDelta = (CameraRight * CurrentAimInput.X + CameraUp * CurrentAimInput.Y) * AimMoveSpeed * DeltaTime;
	CurrentAimWorldLocation += MoveDelta;

	const FVector PlayerLocation = HeroChar->GetActorLocation();
	FVector ToAim = CurrentAimWorldLocation - PlayerLocation;
	const float Dist = ToAim.Size();

	if (Dist > AimMaxDistance)
	{
		CurrentAimWorldLocation = PlayerLocation + ToAim.GetSafeNormal() * AimMaxDistance;
	}
	else if (Dist < AimMinDistance)
	{
		CurrentAimWorldLocation = PlayerLocation + ToAim.GetSafeNormal() * AimMinDistance;
	}

	FHitResult GroundHit;
	const FVector TraceStart = CurrentAimWorldLocation + FVector(0.f, 0.f, AimGroundTraceDepth);
	const FVector TraceEnd = CurrentAimWorldLocation - FVector(0.f, 0.f, AimGroundTraceDepth);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	const bool bHit = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	if (bShowDebugTrace)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, -1.f, 0, 2.f);
		if (bHit) DrawDebugPoint(GetWorld(), GroundHit.ImpactPoint, 10.f, FColor::Green, false, -1.f);
	}

	if (bHit)
	{
		CurrentAimWorldLocation.Z = GroundHit.ImpactPoint.Z;
	}
}

void UHeroGameplayAbility_ShootProj::CleanUpAimPhase()
{
	CurrentAimInput = FVector2D::ZeroVector;
	CurrentAimWorldLocation = FVector::ZeroVector;

	if (DrawnAimWidget)
	{
		DrawnAimWidget->RemoveFromParent();
	}
	DrawnAimWidget = nullptr;
	AimWidgetSize = FVector2D::ZeroVector;
}