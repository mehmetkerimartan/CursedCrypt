#include "SkeletonAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "SkeletonEnemy.h"

ASkeletonAIController::ASkeletonAIController() {
  UAIPerceptionComponent *PerceptionComp =
      CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
  SetPerceptionComponent(*PerceptionComp);

  UAISenseConfig_Sight *SightConfig =
      CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
  SightConfig->SightRadius = SightRadius;
  SightConfig->LoseSightRadius = LoseSightRadius;
  SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
  SightConfig->SetMaxAge(5.0f);
  SightConfig->DetectionByAffiliation.bDetectEnemies = true;
  SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
  SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

  PerceptionComp->ConfigureSense(*SightConfig);
  PerceptionComp->SetDominantSense(UAISenseConfig_Sight::StaticClass());
  PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
      this, &ASkeletonAIController::OnTargetPerceptionUpdated);
}

void ASkeletonAIController::OnPossess(APawn *InPawn) {
  Super::OnPossess(InPawn);

  if (ASkeletonEnemy *Enemy = Cast<ASkeletonEnemy>(InPawn)) {
    if (Enemy->BehaviorTree != nullptr) {
      RunBehaviorTree(Enemy->BehaviorTree);
    }
  }
}

void ASkeletonAIController::OnTargetPerceptionUpdated(AActor *Actor,
                                                      FAIStimulus Stimulus) {
  if (Stimulus.WasSuccessfullySensed()) {
    /* Diğer skeleton'ları filtrele - sadece oyuncuyu hedef al */
    if (Cast<ASkeletonEnemy>(Actor)) {
      return;
    }
    if (UBlackboardComponent *BBComp = GetBlackboardComponent()) {
      BBComp->SetValueAsObject(TEXT("TargetActor"), Actor);
    }
  }
}
