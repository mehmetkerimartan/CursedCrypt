#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkeletonAIController.h"
#include "SkeletonEnemy.generated.h"

class UAttributeComponent;
class UBehaviorTree;
class UWidgetComponent;

UCLASS()
class CURSEDCRYPT_API ASkeletonEnemy : public ACharacter {
  GENERATED_BODY()

public:
  ASkeletonEnemy();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UAttributeComponent *AttributeComp;

  UPROPERTY(EditDefaultsOnly, Category = "AI")
  UBehaviorTree *BehaviorTree;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
  UWidgetComponent *HealthBarWidget;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  TArray<UAnimMontage *> AttackMontages;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  UAnimMontage *HitReactMontage;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  UAnimMontage *DeathMontage;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float AttackRange = 150.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float AttackDamage = 20.0f;

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void PerformAttack();

  UFUNCTION(BlueprintNativeEvent, Category = "Combat")
  void HandleHealthChanged(UAttributeComponent *OwningComp, float NewHealth,
                           float Delta, AActor *InstigatorActor);
  virtual void
  HandleHealthChanged_Implementation(UAttributeComponent *OwningComp,
                                     float NewHealth, float Delta,
                                     AActor *InstigatorActor);

protected:
  virtual void BeginPlay() override;

private:
  bool bIsDead = false;
};
