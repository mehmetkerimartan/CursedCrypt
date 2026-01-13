#include "SkeletonEnemy.h"
#include "AIController.h"
#include "AttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "CryptCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASkeletonEnemy::ASkeletonEnemy() {
  PrimaryActorTick.bCanEverTick = false;

  AttributeComp =
      CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComp"));

  HealthBarWidget =
      CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
  HealthBarWidget->SetupAttachment(RootComponent);
  HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
  HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
  HealthBarWidget->SetDrawSize(FVector2D(50.0f, 5.0f));
  HealthBarWidget->SetDrawAtDesiredSize(false);

  GetCharacterMovement()->MaxWalkSpeed = 300.0f;

  GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
  GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

  AIControllerClass = ASkeletonAIController::StaticClass();
  AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASkeletonEnemy::BeginPlay() {
  Super::BeginPlay();

  if (AttributeComp) {
    AttributeComp->OnHealthChanged.AddDynamic(
        this, &ASkeletonEnemy::HandleHealthChanged);
  }
}

void ASkeletonEnemy::PerformAttack() {
  if (AttackMontages.Num() > 0 && !bIsDead) {
    AAIController *AIC = Cast<AAIController>(GetController());
    if (AIC) {
      UBlackboardComponent *BBComp = AIC->GetBlackboardComponent();
      if (BBComp) {
        AActor *Target =
            Cast<AActor>(BBComp->GetValueAsObject(TEXT("TargetActor")));
        if (Target) {
          ACryptCharacter *Player = Cast<ACryptCharacter>(Target);
          if (Player && Player->IsDead()) {
            BBComp->ClearValue(TEXT("TargetActor"));
            return;
          }
          FVector Direction = Target->GetActorLocation() - GetActorLocation();
          Direction.Z = 0.0f;
          SetActorRotation(Direction.Rotation());
        }
      }
    }
    int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
    PlayAnimMontage(AttackMontages[RandomIndex]);
  }
}

void ASkeletonEnemy::HandleHealthChanged_Implementation(
    UAttributeComponent *OwningComp, float NewHealth, float Delta,
    AActor *InstigatorActor) {
  if (bIsDead)
    return;

  /* Can barını güncelle */
  if (HealthBarWidget && AttributeComp) {
    if (UUserWidget *Widget = HealthBarWidget->GetUserWidgetObject()) {
      if (UProgressBar *ProgressBar =
              Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HP_Bar")))) {
        ProgressBar->SetPercent(AttributeComp->GetHealthPercent());
      }
    }
  }

  if (Delta < 0.0f) {
    if (InstigatorActor) {
      FVector Direction =
          GetActorLocation() - InstigatorActor->GetActorLocation();
      Direction.Z = 0.0f;
      Direction.Normalize();

      SetActorRotation((-Direction).Rotation());

      LaunchCharacter(Direction * 300.0f, true, false);
    }

    if (NewHealth <= 0.0f) {
      bIsDead = true;

      // Ölünce can barını gizle
      if (HealthBarWidget) {
        HealthBarWidget->SetVisibility(false);
      }

      if (DeathMontage) {
        float DeathDuration = PlayAnimMontage(DeathMontage);

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(
            TimerHandle, [this]() { GetMesh()->bPauseAnims = true; },
            DeathDuration - 0.1f, false);
      }

      AAIController *AIC = Cast<AAIController>(GetController());
      if (AIC) {
        AIC->GetBrainComponent()->StopLogic(TEXT("Dead"));
      }

      GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn,
                                                           ECR_Ignore);

      SetLifeSpan(3.0f);
    } else {
      if (HitReactMontage) {
        PlayAnimMontage(HitReactMontage);
      }
    }
  }
}
