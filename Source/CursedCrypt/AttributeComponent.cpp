#include "AttributeComponent.h"

UAttributeComponent::UAttributeComponent() {
  PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay() {
  Super::BeginPlay();
  Health = MaxHealth;
  Stamina = MaxStamina;
}

bool UAttributeComponent::ApplyDamage(float DamageAmount,
                                      AActor *InstigatorActor) {
  if (DamageAmount <= 0.0f || Health <= 0.0f) {
    return false;
  }

  float OldHealth = Health;
  Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
  float Delta = Health - OldHealth;

  OnHealthChanged.Broadcast(this, Health, Delta, InstigatorActor);
  return true;
}

bool UAttributeComponent::ApplyHealing(float HealAmount) {
  if (HealAmount <= 0.0f || Health <= 0.0f || Health >= MaxHealth) {
    return false;
  }

  float OldHealth = Health;
  Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
  float Delta = Health - OldHealth;

  OnHealthChanged.Broadcast(this, Health, Delta, nullptr);
  return true;
}

bool UAttributeComponent::UseStamina(float Amount) {
  if (Amount <= 0.0f || Stamina < Amount) {
    return false;
  }

  float OldStamina = Stamina;
  Stamina = FMath::Clamp(Stamina - Amount, 0.0f, MaxStamina);
  float Delta = Stamina - OldStamina;

  OnStaminaChanged.Broadcast(this, Stamina, Delta);
  return true;
}

void UAttributeComponent::RegenStamina(float Amount) {
  if (Amount <= 0.0f || Stamina >= MaxStamina) {
    return;
  }

  float OldStamina = Stamina;
  Stamina = FMath::Clamp(Stamina + Amount, 0.0f, MaxStamina);
  float Delta = Stamina - OldStamina;

  OnStaminaChanged.Broadcast(this, Stamina, Delta);
}

float UAttributeComponent::GetHealthPercent() const {
  return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent() const {
  return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive() const { return Health > 0.0f; }