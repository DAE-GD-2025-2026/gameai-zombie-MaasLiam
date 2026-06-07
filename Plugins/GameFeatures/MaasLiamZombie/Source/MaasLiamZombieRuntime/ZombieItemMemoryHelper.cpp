#include "ZombieItemMemoryHelper.h"

#include "StudentPerceptor.h"
#include "ZombieInventoryHelper.h"
#include "ZombieSurvivorStatusHelper.h"

void FZombieItemMemoryHelper::UpdateItemMemory(TArray<FRememberedItem>& RememberedItems, UStudentPerceptor* Perceptor, float DeltaTime, float MaxMemoryAge)
{
	for (int32 Index = RememberedItems.Num() - 1; Index >= 0; --Index)
	{
		FRememberedItem& Memory = RememberedItems[Index];
		Memory.TimeSinceLastSeen += DeltaTime;

		if (Memory.TimeSinceLastSeen > MaxMemoryAge)
		{
			RememberedItems.RemoveAt(Index);
			continue;
		}

		if (Memory.ItemActor && !IsValid(Memory.ItemActor))
		{
			RememberedItems.RemoveAt(Index);
			continue;
		}
	}

	if (!Perceptor)
	{
		return;
	}

	for (AActor* SeenItem : Perceptor->SeenItems)
	{
		if (!IsValid(SeenItem))
		{
			continue;
		}

		RememberVisibleItem(RememberedItems, SeenItem);
	}
}

FRememberedItem* FZombieItemMemoryHelper::GetBestRememberedItem(TArray<FRememberedItem>& RememberedItems, AActor* Owner,
	UActorComponent* InventoryComponent, UActorComponent* HealthComponent, UActorComponent* StaminaComponent, int LowHealthThreshold, float LowStaminaThreshold)
{
	if (!Owner)
	{
		return nullptr;
	}

	FRememberedItem* BestMemory = nullptr;
	float BestDistance = FLT_MAX;
	const FVector OwnerLocation = Owner->GetActorLocation();

	for (FRememberedItem& Memory : RememberedItems)
	{
		if (!DoesMemoryMatchNeed(Memory, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold))
		{
			continue;
		}

		const float Distance = FVector::Dist(OwnerLocation, Memory.LastKnownLocation);

		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestMemory = &Memory;
		}
	}

	return BestMemory;
}

const FRememberedItem* FZombieItemMemoryHelper::GetBestRememberedItem(const TArray<FRememberedItem>& RememberedItems, AActor* Owner, UActorComponent* InventoryComponent, 
	UActorComponent* HealthComponent, UActorComponent* StaminaComponent, int LowHealthThreshold, float LowStaminaThreshold)
{
	if (!Owner)
	{
		return nullptr;
	}

	const FRememberedItem* BestMemory = nullptr;
	float BestDistance = FLT_MAX;
	const FVector OwnerLocation = Owner->GetActorLocation();

	for (const FRememberedItem& Memory : RememberedItems)
	{
		if (!DoesMemoryMatchNeed(Memory, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold))
		{
			continue;
		}

		const float Distance = FVector::Dist(OwnerLocation, Memory.LastKnownLocation);

		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestMemory = &Memory;
		}
	}

	return BestMemory;
}

void FZombieItemMemoryHelper::ForgetItem(TArray<FRememberedItem>& RememberedItems, AActor* ItemActor)
{
	if (!ItemActor)
	{
		return;
	}

	for (int32 Index = RememberedItems.Num() - 1; Index >= 0; --Index)
	{
		if (RememberedItems[Index].ItemActor == ItemActor)
		{
			RememberedItems.RemoveAt(Index);
		}
	}
}

void FZombieItemMemoryHelper::ForgetItemNearLocation(TArray<FRememberedItem>& RememberedItems, const FVector& Location, float Radius)
{
	for (int32 Index = RememberedItems.Num() - 1; Index >= 0; --Index)
	{
		const float Distance = FVector::Dist(Location, RememberedItems[Index].LastKnownLocation);

		if (Distance <= Radius)
		{
			RememberedItems.RemoveAt(Index);
		}
	}
}

FString FZombieItemMemoryHelper::GetItemType(AActor* ItemActor)
{
	if (!ItemActor)
	{
		return "";
	}

	const FString ItemName = ItemActor->GetName();
	const FString ClassName = ItemActor->GetClass()->GetName();

	if (ItemName.Contains(TEXT("Food")) || ClassName.Contains(TEXT("Food")))
	{
		return "Food";
	}

	if (ItemName.Contains(TEXT("Medkit")) || ClassName.Contains(TEXT("Medkit")))
	{
		return "Medkit";
	}

	if (ItemName.Contains(TEXT("Pistol")) || ClassName.Contains(TEXT("Pistol")))
	{
		return "Pistol";
	}

	if (ItemName.Contains(TEXT("Shotgun")) || ClassName.Contains(TEXT("Shotgun")))
	{
		return "Shotgun";
	}

	return "";
}

bool FZombieItemMemoryHelper::IsUsefulItemType(const FString& ItemType)
{
	return ItemType == "Food" || ItemType == "Medkit" || ItemType == "Pistol" || ItemType == "Shotgun";
}

bool FZombieItemMemoryHelper::DoesMemoryMatchNeed(
	const FRememberedItem& Memory, UActorComponent* InventoryComponent, UActorComponent* HealthComponent,
	UActorComponent* StaminaComponent, int LowHealthThreshold, float LowStaminaThreshold)
{
	if (!IsUsefulItemType(Memory.ItemType))
	{
		return false;
	}

	const int CurrentHealth = FZombieSurvivorStatusHelper::GetCurrentHealth(HealthComponent);

	const float CurrentStamina = FZombieSurvivorStatusHelper::GetCurrentStamina(StaminaComponent);

	if (Memory.ItemType == "Food" && CurrentStamina <= LowStaminaThreshold && !FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Food")))
	{
		return true;
	}

	if (Memory.ItemType == "Medkit" && CurrentHealth <= LowHealthThreshold && !FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Medkit")))
	{
		return true;
	}

	const bool bHasWeapon = FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Pistol")) || FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Shotgun"));

	if (!bHasWeapon && (Memory.ItemType == "Pistol" || Memory.ItemType == "Shotgun"))
	{
		return true;
	}

	return false;
}

void FZombieItemMemoryHelper::RememberVisibleItem(TArray<FRememberedItem>& RememberedItems, AActor* ItemActor)
{
	if (!ItemActor)
	{
		return;
	}

	const FString ItemType = GetItemType(ItemActor);

	if (!IsUsefulItemType(ItemType))
	{
		return;
	}

	for (FRememberedItem& Memory : RememberedItems)
	{
		if (Memory.ItemActor == ItemActor)
		{
			Memory.LastKnownLocation = ItemActor->GetActorLocation();
			Memory.ItemType = ItemType;
			Memory.TimeSinceLastSeen = 0.f;
			return;
		}
	}

	FRememberedItem NewMemory;
	NewMemory.ItemActor = ItemActor;
	NewMemory.LastKnownLocation = ItemActor->GetActorLocation();
	NewMemory.ItemType = ItemType;
	NewMemory.TimeSinceLastSeen = 0.f;

	RememberedItems.Add(NewMemory);
}