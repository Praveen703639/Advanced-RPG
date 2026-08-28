// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AWarriorIceClone.generated.h"

UCLASS()
class ADVANCEDRPG_API AAWarriorIceClone : public ACharacter
{
    GENERATED_BODY()

public:
    AAWarriorIceClone();

    void InitializeClone(ACharacter* InSourceHero, float InDuration);

    UFUNCTION(BlueprintCallable, Category = "IceClone")
    void DestroyClone();

    UFUNCTION(BlueprintPure, Category = "IceClone")
    float GetRemainingDuration() const;

    UFUNCTION(BlueprintPure, Category = "IceClone")
    ACharacter* GetSourceHero() const;

    UFUNCTION(BlueprintPure, Category = "IceClone")
    bool IsDestroying() const { return bIsDestroying; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;

    // --- Configurable Defaults ---
    UPROPERTY(EditDefaultsOnly, Category = "IceClone|Visuals")
    float FadeOutDuration = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "IceClone|Visuals")
    FName IceMaterialOpacityParam = FName("Opacity");

    UPROPERTY(EditDefaultsOnly, Category = "IceClone|Stats")
    float CloneHealth = 100.f;

    // --- Runtime State ---
    UPROPERTY()
    TWeakObjectPtr<ACharacter> SourceHero;

    UPROPERTY()
    float MaxDuration = 7.f;

    UPROPERTY()
    float ElapsedTime = 0.f;

    UPROPERTY()
    bool bIsDestroying = false;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> DynamicMaterials;

    // --- Internal Methods ---
    void SetupForAggro();
    void SetupVisualsFromHero();
    void UpdateVisuals(float DeltaTime);
    void FadeOutAndDestroy();

    UFUNCTION()
    void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};