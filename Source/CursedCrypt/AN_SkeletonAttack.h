#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SkeletonAttack.generated.h"

UCLASS()
class CURSEDCRYPT_API UAN_SkeletonAttack : public UAnimNotify {
  GENERATED_BODY()

public:
  virtual void Notify(USkeletalMeshComponent *MeshComp,
                      UAnimSequenceBase *Animation,
                      const FAnimNotifyEventReference &EventReference) override;

  UPROPERTY(EditAnywhere, Category = "Attack")
  float AttackRadius = 100.0f;

  UPROPERTY(EditAnywhere, Category = "Attack")
  float AttackRange = 150.0f;
};
