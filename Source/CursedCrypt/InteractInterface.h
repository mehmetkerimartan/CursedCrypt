#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractInterface : public UInterface {
  GENERATED_BODY()
};

class CURSEDCRYPT_API IInteractInterface {
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  void Interact(AActor *Caller);
  virtual void Interact_Implementation(AActor *Caller) {}

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  bool HasKey();
  virtual bool HasKey_Implementation() { return false; }

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  void GetKey();
  virtual void GetKey_Implementation() {}

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
  void UseKey();
  virtual void UseKey_Implementation() {}
};
