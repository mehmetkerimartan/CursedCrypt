#pragma once
#include "AIController.h"
#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "SkeletonAIController.generated.h"


class UAISenseConfig_Sight;

UCLASS()
class CURSEDCRYPT_API ASkeletonAIController : public AAIController {
  GENERATED_BODY()

public:
  ASkeletonAIController();

  virtual void OnPossess(APawn *InPawn) override;

protected:
  UPROPERTY(EditDefaultsOnly, Category = "AI")
  float SightRadius = 1000.0f;

  UPROPERTY(EditDefaultsOnly, Category = "AI")
  float LoseSightRadius = 1200.0f;

  UPROPERTY(EditDefaultsOnly, Category = "AI")
  float PeripheralVisionAngle = 60.0f;

  UFUNCTION()
  void OnTargetPerceptionUpdated(AActor *Actor, FAIStimulus Stimulus);
};
