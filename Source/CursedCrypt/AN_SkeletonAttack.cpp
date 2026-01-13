/* AN_SkeletonAttack.cpp - Cursed Crypt */
#include "AN_SkeletonAttack.h"
#include "AttributeComponent.h"
#include "CryptCharacter.h"
#include "SkeletonEnemy.h"

void UAN_SkeletonAttack::Notify(
    USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
    const FAnimNotifyEventReference &EventReference) {
  Super::Notify(MeshComp, Animation, EventReference);

  if (MeshComp == nullptr)
    return;

  AActor *Owner = MeshComp->GetOwner();
  if (Owner == nullptr)
    return;

  ASkeletonEnemy *Skeleton = Cast<ASkeletonEnemy>(Owner);
  if (Skeleton == nullptr)
    return;

  FVector Start = Owner->GetActorLocation();
  FVector End = Start + Owner->GetActorForwardVector() * AttackRange;

  TArray<FHitResult> HitResults;
  FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);
  FCollisionQueryParams Params;
  Params.AddIgnoredActor(Owner);

  bool bHit = Owner->GetWorld()->SweepMultiByChannel(
      HitResults, Start, End, FQuat::Identity, ECC_Pawn, Shape, Params);

  if (bHit) {
    TSet<AActor *> HitActors;
    for (const FHitResult &Hit : HitResults) {
      AActor *HitActor = Hit.GetActor();
      ACryptCharacter *Player = Cast<ACryptCharacter>(HitActor);
      if (Player && !HitActors.Contains(HitActor) && !Player->IsDead()) {
        HitActors.Add(HitActor);
        UAttributeComponent *AttribComp =
            HitActor->FindComponentByClass<UAttributeComponent>();
        if (AttribComp) {
          AttribComp->ApplyDamage(Skeleton->AttackDamage, Owner);
        }
      }
    }
  }
}
