#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractInterface.h"
#include "CryptCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class UAttributeComponent;
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UUserWidget;
struct FInputActionValue;

UCLASS()
class CURSEDCRYPT_API ACryptCharacter : public ACharacter,
                                        public IInteractInterface {
  GENERATED_BODY()

public:
  ACryptCharacter();

  virtual void SetupPlayerInputComponent(
      class UInputComponent *PlayerInputComponent) override;

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void EnableWeaponCollision();

  UFUNCTION(BlueprintCallable, Category = "Combat")
  void DisableWeaponCollision();

  UFUNCTION(BlueprintCallable, Category = "Combat")
  bool IsDead() const { return bIsDead; }

protected:
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UAttributeComponent *AttributeComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  USpringArmComponent *SpringArmComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UCameraComponent *CameraComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
  UBoxComponent *WeaponHitbox;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputMappingContext *DefaultMappingContext;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *MoveAction;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *LookAction;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *AttackAction;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *HeavyAttackAction;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *SprintAction;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *InteractAction;

  UPROPERTY(EditDefaultsOnly, Category = "Input")
  UInputAction *PauseAction;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  UAnimMontage *AttackMontage1;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  UAnimMontage *AttackMontage2;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  UAnimMontage *HitReactMontage;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float AttackDamage = 25.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float LightAttackStaminaCost = 10.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float HeavyAttackStaminaCost = 25.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Movement")
  float SprintSpeed = 800.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Movement")
  float WalkSpeed = 500.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Movement")
  float SprintStaminaCostPerSecond = 20.0f;

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  float StaminaRegenPerSecond = 15.0f;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> PauseMenuClass;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> TutorialWidgetClass;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> NotificationWidgetClass;

  UPROPERTY(EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> DeathWidgetClass;

  UPROPERTY()
  UUserWidget *PauseMenuWidget;

  UPROPERTY()
  UUserWidget *TutorialWidget;

  UPROPERTY(BlueprintReadOnly, Category = "Combat")
  bool bIsAttacking = false;

  UPROPERTY(BlueprintReadOnly, Category = "Movement")
  bool bIsSprinting = false;

  UPROPERTY(BlueprintReadWrite, Category = "Inventory")
  int32 KeyCount = 0;

  bool bCurrentAttackIsHeavy = false;

  TArray<AActor *> HitActors;

  void Move(const FInputActionValue &Value);
  void Look(const FInputActionValue &Value);
  void LightAttack();
  void HeavyAttack();
  void PerformAttack(bool bIsHeavyAttack);
  void Interact();
  void StartSprint();
  void StopSprint();

  UFUNCTION(BlueprintCallable, Category = "UI")
  void TogglePauseMenu();

  UFUNCTION(BlueprintCallable, Category = "UI")
  void HideTutorial();

  UFUNCTION(BlueprintCallable, Category = "UI")
  void ShowNotification(FText Message);

  UFUNCTION()
  void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

  UFUNCTION()
  void OnWeaponOverlap(UPrimitiveComponent *OverlappedComponent,
                       AActor *OtherActor, UPrimitiveComponent *OtherComp,
                       int32 OtherBodyIndex, bool bFromSweep,
                       const FHitResult &SweepResult);

  virtual bool HasKey_Implementation() override;
  virtual void GetKey_Implementation() override;
  virtual void UseKey_Implementation() override;

  UFUNCTION()
  void HandleHealthChanged(UAttributeComponent *OwningComp, float NewHealth,
                           float Delta, AActor *InstigatorActor);

  void HandleDeath();

  UPROPERTY(EditDefaultsOnly, Category = "Combat")
  UAnimMontage *DeathMontage;

  bool bIsDead = false;
};
