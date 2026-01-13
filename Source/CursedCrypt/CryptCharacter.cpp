#include "CryptCharacter.h"
#include "AttributeComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InteractInterface.h"
#include "Kismet/GameplayStatics.h"

ACryptCharacter::ACryptCharacter() {
  PrimaryActorTick.bCanEverTick = true;

  bUseControllerRotationYaw = false;
  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;

  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

  AttributeComp =
      CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComp"));

  SpringArmComp =
      CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
  SpringArmComp->SetupAttachment(RootComponent);
  SpringArmComp->TargetArmLength = 300.0f;
  SpringArmComp->bUsePawnControlRotation = true;

  CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
  CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
  CameraComp->bUsePawnControlRotation = false;

  WeaponHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponHitbox"));
  WeaponHitbox->SetupAttachment(GetMesh(), TEXT("hand_r"));
  WeaponHitbox->SetBoxExtent(FVector(20.0f, 20.0f, 50.0f));
  WeaponHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  WeaponHitbox->SetCollisionResponseToAllChannels(ECR_Overlap);

  GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

void ACryptCharacter::BeginPlay() {
  Super::BeginPlay();

  if (TutorialWidgetClass) {
    TutorialWidget = CreateWidget<UUserWidget>(GetWorld(), TutorialWidgetClass);
    if (TutorialWidget) {
      TutorialWidget->AddToViewport();
    }
  }
  if (APlayerController *PlayerController =
          Cast<APlayerController>(Controller)) {
    if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PlayerController->GetLocalPlayer())) {
      Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
  }

  WeaponHitbox->OnComponentBeginOverlap.AddDynamic(
      this, &ACryptCharacter::OnWeaponOverlap);

  if (UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance()) {
    AnimInstance->OnMontageEnded.AddDynamic(
        this, &ACryptCharacter::OnAttackMontageEnded);
  }

  AttributeComp->OnHealthChanged.AddDynamic(
      this, &ACryptCharacter::HandleHealthChanged);
}

void ACryptCharacter::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (bIsDead)
    return;

  if (bIsSprinting && !bIsAttacking) {
    if (!AttributeComp->UseStamina(SprintStaminaCostPerSecond * DeltaTime)) {
      StopSprint();
    }
  } else if (!bIsAttacking) {
    AttributeComp->RegenStamina(StaminaRegenPerSecond * DeltaTime);
  }
}

void ACryptCharacter::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  if (UEnhancedInputComponent *EnhancedInputComponent =
          CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
                                       this, &ACryptCharacter::Move);
    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
                                       this, &ACryptCharacter::Look);
    EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started,
                                       this, &ACryptCharacter::LightAttack);
    EnhancedInputComponent->BindAction(HeavyAttackAction,
                                       ETriggerEvent::Started, this,
                                       &ACryptCharacter::HeavyAttack);
    EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
                                       this, &ACryptCharacter::StartSprint);
    EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,
                                       this, &ACryptCharacter::StopSprint);
    EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,
                                       this, &ACryptCharacter::Interact);
    EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started,
                                       this, &ACryptCharacter::TogglePauseMenu);
  }
}

void ACryptCharacter::EnableWeaponCollision() {
  WeaponHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACryptCharacter::DisableWeaponCollision() {
  WeaponHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  HitActors.Empty();
}

void ACryptCharacter::OnAttackMontageEnded(UAnimMontage *Montage,
                                           bool bInterrupted) {
  if (Montage == DeathMontage) {
    GetMesh()->bPauseAnims = true;
    return;
  }
  bIsAttacking = false;
  HitActors.Empty();
  GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void ACryptCharacter::OnWeaponOverlap(UPrimitiveComponent *OverlappedComponent,
                                      AActor *OtherActor,
                                      UPrimitiveComponent *OtherComp,
                                      int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult &SweepResult) {
  if (OtherActor == nullptr || OtherActor == this) {
    return;
  }

  if (HitActors.Contains(OtherActor)) {
    return;
  }

  UAttributeComponent *TargetAttributes =
      OtherActor->FindComponentByClass<UAttributeComponent>();
  if (TargetAttributes != nullptr) {
    HitActors.Add(OtherActor);
    TargetAttributes->ApplyDamage(AttackDamage, this);
  }
}

void ACryptCharacter::Move(const FInputActionValue &Value) {
  FVector2D MovementVector = Value.Get<FVector2D>();

  if (Controller != nullptr) {
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
  }
}

void ACryptCharacter::Look(const FInputActionValue &Value) {
  FVector2D LookAxisVector = Value.Get<FVector2D>();

  if (Controller != nullptr) {
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
  }
}

void ACryptCharacter::LightAttack() { PerformAttack(false); }

void ACryptCharacter::HeavyAttack() { PerformAttack(true); }

void ACryptCharacter::PerformAttack(bool bIsHeavyAttack) {
  if (bIsAttacking)
    return;

  float StaminaCost =
      bIsHeavyAttack ? HeavyAttackStaminaCost : LightAttackStaminaCost;
  if (!AttributeComp->UseStamina(StaminaCost))
    return;

  UAnimMontage *MontageToPlay =
      bIsHeavyAttack ? AttackMontage2 : AttackMontage1;
  if (!MontageToPlay)
    return;

  if (Controller) {
    FRotator ControlRotation = Controller->GetControlRotation();
    FRotator NewRotation(0.0f, ControlRotation.Yaw, 0.0f);
    SetActorRotation(NewRotation);
  }

  bIsAttacking = true;
  bCurrentAttackIsHeavy = bIsHeavyAttack;
  HitActors.Empty();
  GetCharacterMovement()->MaxWalkSpeed = 150.0f;
  PlayAnimMontage(MontageToPlay);
}

void ACryptCharacter::Interact() {
  FVector Center = GetActorLocation();
  float Radius = 200.0f;

  TArray<FOverlapResult> Overlaps;
  FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
  FCollisionQueryParams Params;
  Params.AddIgnoredActor(this);

  if (GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity,
                                        ECC_Visibility, Sphere, Params)) {
    for (const FOverlapResult &Overlap : Overlaps) {
      AActor *HitActor = Overlap.GetActor();
      if (HitActor && HitActor->GetClass()->ImplementsInterface(
                          UInteractInterface::StaticClass())) {
        IInteractInterface::Execute_Interact(HitActor, this);
        break;
      }
    }
  }
}

void ACryptCharacter::HandleHealthChanged(UAttributeComponent *OwningComp,
                                          float NewHealth, float Delta,
                                          AActor *InstigatorActor) {
  if (NewHealth <= 0.0f && !bIsDead) {
    HandleDeath();
  } else if (Delta < 0.0f && HitReactMontage && !bIsDead) {
    PlayAnimMontage(HitReactMontage);
  }
}

void ACryptCharacter::HandleDeath() {
  bIsDead = true;

  GetCharacterMovement()->DisableMovement();
  GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  float DeathDuration = 0.0f;
  if (DeathMontage) {
    DeathDuration = PlayAnimMontage(DeathMontage);
    float BlendOutTime = DeathMontage->BlendOut.GetBlendTime();

    FTimerHandle DeathTimerHandle;
    GetWorldTimerManager().SetTimer(
        DeathTimerHandle,
        [this]() {
          GetMesh()->bPauseAnims = true;
          GetMesh()->bNoSkeletonUpdate = true;
        },
        FMath::Max(0.1f, DeathDuration - BlendOutTime - 0.1f), false);
  }

  APlayerController *PC = Cast<APlayerController>(GetController());
  if (PC) {
    DisableInput(PC);
    PC->bShowMouseCursor = true;

    if (DeathWidgetClass) {
      UUserWidget *DeathWidget =
          CreateWidget<UUserWidget>(GetWorld(), DeathWidgetClass);
      if (DeathWidget) {
        DeathWidget->AddToViewport();
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(DeathWidget->TakeWidget());
        PC->SetInputMode(InputMode);
      }
    }
  }
}

void ACryptCharacter::StartSprint() {
  if (bIsAttacking || bIsDead)
    return;
  bIsSprinting = true;
  GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ACryptCharacter::StopSprint() {
  bIsSprinting = false;
  if (!bIsAttacking) {
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
  }
}

void ACryptCharacter::TogglePauseMenu() {
  if (bIsDead)
    return;

  APlayerController *PC = Cast<APlayerController>(Controller);
  if (!PC)
    return;

  if (PauseMenuWidget) {
    PauseMenuWidget->RemoveFromParent();
    PauseMenuWidget = nullptr;

    PC->SetPause(false);
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = false;
  } else {
    if (PauseMenuClass) {
      PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);
      if (PauseMenuWidget) {
        PauseMenuWidget->AddToViewport();

        PC->SetPause(true);
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
      }
    }
  }
}
void ACryptCharacter::HideTutorial() {
  if (TutorialWidget) {
    TutorialWidget->RemoveFromParent();
    TutorialWidget = nullptr;
  }
}

void ACryptCharacter::ShowNotification(FText Message) {
  if (NotificationWidgetClass) {
    UUserWidget *NotifyWidget =
        CreateWidget<UUserWidget>(GetWorld(), NotificationWidgetClass);
    if (NotifyWidget) {
      NotifyWidget->AddToViewport();

      FTimerHandle TimerHandle;
      GetWorldTimerManager().SetTimer(
          TimerHandle,
          [NotifyWidget]() {
            if (NotifyWidget) {
              NotifyWidget->RemoveFromParent();
            }
          },
          2.0f, false);
    }
  }
}

bool ACryptCharacter::HasKey_Implementation() { return KeyCount > 0; }
void ACryptCharacter::GetKey_Implementation() { KeyCount++; }
void ACryptCharacter::UseKey_Implementation() {
  KeyCount = FMath::Max(0, KeyCount - 1);
}
