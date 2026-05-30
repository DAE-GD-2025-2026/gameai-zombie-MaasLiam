#include "ZombieSurvivorStatusHelper.h"
#include "ZombieInventoryHelper.h"

UActorComponent* FZombieSurvivorStatusHelper::FindComponentByNamePart(AActor* Owner, const FString& NamePart)
{
	if (!Owner)
	{
		return nullptr;
	}

	TArray<UActorComponent*> Components;
	Owner->GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		if (!Component)
		{
			continue;
		}

		if (
			Component->GetName().Contains(NamePart) ||
			Component->GetClass()->GetName().Contains(NamePart)
		)
		{
			return Component;
		}
	}

	return nullptr;
}

int FZombieSurvivorStatusHelper::GetCurrentHealth(UActorComponent* HealthComponent)
{
	if (!HealthComponent)
	{
		return 10;
	}

	UFunction* HealthFunction = HealthComponent->FindFunction(TEXT("GetHealth"));
	if (!HealthFunction)
	{
		return 10;
	}

	struct FHealthParams
	{
		int ReturnValue;
	};

	FHealthParams Params;
	Params.ReturnValue = 10;

	HealthComponent->ProcessEvent(HealthFunction, &Params);

	return Params.ReturnValue;
}

float FZombieSurvivorStatusHelper::GetCurrentStamina(UActorComponent* StaminaComponent)
{
	if (!StaminaComponent)
	{
		return 10.f;
	}

	UFunction* StaminaFunction = StaminaComponent->FindFunction(TEXT("GetCurrentStamina"));
	if (!StaminaFunction)
	{
		return 10.f;
	}

	struct FStaminaParams
	{
		float ReturnValue;
	};

	FStaminaParams Params;
	Params.ReturnValue = 10.f;

	StaminaComponent->ProcessEvent(StaminaFunction, &Params);

	return Params.ReturnValue;
}

bool FZombieSurvivorStatusHelper::ShouldUseItem(
	UActorComponent* InventoryComponent,
	UActorComponent* HealthComponent,
	UActorComponent* StaminaComponent,
	int LowHealthThreshold,
	float LowStaminaThreshold)
{
	const int CurrentHealth = GetCurrentHealth(HealthComponent);
	const float CurrentStamina = GetCurrentStamina(StaminaComponent);

	const bool bCanHeal =
		CurrentHealth <= LowHealthThreshold &&
		FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Medkit"));

	const bool bCanRestoreStamina =
		CurrentStamina <= LowStaminaThreshold &&
		FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Food"));

	return bCanHeal || bCanRestoreStamina;
}

bool FZombieSurvivorStatusHelper::TryUseInventoryItem(
	UActorComponent* InventoryComponent,
	UActorComponent* HealthComponent,
	UActorComponent* StaminaComponent,
	int LowHealthThreshold,
	float LowStaminaThreshold)
{
	const int32 Capacity = FZombieInventoryHelper::GetInventoryCapacity(InventoryComponent);

	const bool bNeedsHealth = GetCurrentHealth(HealthComponent) <= LowHealthThreshold;
	const bool bNeedsStamina = GetCurrentStamina(StaminaComponent) <= LowStaminaThreshold;

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (
			bNeedsHealth &&
			FZombieInventoryHelper::DoesInventorySlotContainItemType(InventoryComponent, SlotIndex, TEXT("Medkit"))
		)
		{
			if (FZombieInventoryHelper::TryUseItemInSlot(InventoryComponent, SlotIndex))
			{
				FZombieInventoryHelper::TryRemoveItemInSlot(InventoryComponent, SlotIndex);
				return true;
			}
		}

		if (
			bNeedsStamina &&
			FZombieInventoryHelper::DoesInventorySlotContainItemType(InventoryComponent, SlotIndex, TEXT("Food"))
		)
		{
			if (FZombieInventoryHelper::TryUseItemInSlot(InventoryComponent, SlotIndex))
			{
				FZombieInventoryHelper::TryRemoveItemInSlot(InventoryComponent, SlotIndex);
				return true;
			}
		}
	}

	return false;
}