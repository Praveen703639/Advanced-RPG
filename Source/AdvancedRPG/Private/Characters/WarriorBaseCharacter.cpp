// pravin's patashala all Rights Reserved 

#include "Characters/WarriorBaseCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "MotionWarpingComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"


#include "WarriorDebugHelper.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	// ✅ Create ASC and AttributeSet in C++
	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));


	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

}

void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Your existing startup data grant
    if (CharacterStartUpData.IsNull())
    {
        return;
    }

    if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
    {
        LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, 1);
    }

    // 🔥 FIX #13: CRITICAL — Re-bind input after possession
    // This MUST happen after GiveToAbilitySystemComponent
    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        if (UInputComponent* InputComp = PC->InputComponent)
        {
            // Clear old bindings
            InputComp->ClearActionBindings();

            // Re-bind abilities to input
            // If your project uses a custom Bind function:
            // WarriorAbilitySystemComponent->BindAbilityActivationToInputComponent(InputComp);

            // If using Enhanced Input, trigger your binding setup:
            SetupPlayerInputComponent(InputComp);
        }
    }


	ensureMsgf(!CharacterStartUpData.IsNull(),
		TEXT("Forgot to assign startup data to %s :"), *GetName());
}

UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	// ✅ This must return VALID ASC
	return WarriorAbilitySystemComponent;
}

UPawnUIComponents* AWarriorBaseCharacter::GetPawnUIComponents() const
{
	return nullptr;
}

UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}