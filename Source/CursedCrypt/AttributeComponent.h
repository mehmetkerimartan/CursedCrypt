#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged,
                                              UAttributeComponent *, OwningComp,
                                              float, NewHealth, float, Delta,
                                              AActor *, InstigatorActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged,
                                               UAttributeComponent *,
                                               OwningComp, float, NewStamina,
                                               float, Delta);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CURSEDCRYPT_API UAttributeComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UAttributeComponent();

  UPROPERTY(BlueprintAssignable, Category = "Attributes")
  FOnHealthChanged OnHealthChanged;

  UPROPERTY(BlueprintAssignable, Category = "Attributes")
  FOnStaminaChanged OnStaminaChanged;

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  bool ApplyDamage(float DamageAmount, AActor *InstigatorActor);

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  bool ApplyHealing(float HealAmount);

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  bool UseStamina(float Amount);

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  void RegenStamina(float Amount);

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  float GetHealthPercent() const;

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  float GetStaminaPercent() const;

  UFUNCTION(BlueprintCallable, Category = "Attributes")
  bool IsAlive() const;

protected:
  UPROPERTY(EditDefaultsOnly, Category = "Attributes")
  float MaxHealth = 100.0f;

  UPROPERTY(VisibleAnywhere, Category = "Attributes")
  float Health;

  UPROPERTY(EditDefaultsOnly, Category = "Attributes")
  float MaxStamina = 100.0f;

  UPROPERTY(VisibleAnywhere, Category = "Attributes")
  float Stamina;

  virtual void BeginPlay() override;
};