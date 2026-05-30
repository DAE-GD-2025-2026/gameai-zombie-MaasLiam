#include "ZombieInventoryHelper.h"
#include "StudentPerceptor.h"

AActor* FZombieInventoryHelper::GetBestItem(UStudentPerceptor* Perceptor, AActor* Owner)
{
	if (!Perceptor || !Owner) return nullptr;

	AActor* BestItem = nullptr;
	int32 BestPriority = 0;
	float BestDistance = FLT_MAX;

	const FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* Item : Perceptor->SeenItems)
	{
		if (!IsValid(Item)) continue;

		const int32 Priority = GetItemPriority(Item);
		if (Priority <= 0) continue;

		const float Distance = FVector::Dist(OwnerLocation, Item->GetActorLocation());

		if (Priority > BestPriority || (Priority == BestPriority && Distance < BestDistance))
		{
			BestPriority = Priority;
			BestDistance = Distance;
			BestItem = Item;
		}
	}

	return BestItem;
}

int32 FZombieInventoryHelper::GetItemPriority(AActor* ItemActor)
{
	if (!ItemActor) return 0;

	const FString ItemName = ItemActor->GetName();
	const FString ClassName = ItemActor->GetClass()->GetName();

	if (ItemName.Contains(TEXT("Medkit")) || ClassName.Contains(TEXT("Medkit"))) return 4;
	if (ItemName.Contains(TEXT("Food")) || ClassName.Contains(TEXT("Food"))) return 3;

	if (
		ItemName.Contains(TEXT("Shotgun")) || ClassName.Contains(TEXT("Shotgun")) ||
		ItemName.Contains(TEXT("Pistol")) || ClassName.Contains(TEXT("Pistol"))
	)
	{
		return 2;
	}

	if (ItemName.Contains(TEXT("Garbage")) || ClassName.Contains(TEXT("Garbage"))) return 0;

	return 1;
}

bool FZombieInventoryHelper::TryPickupItem(AActor* Owner, UActorComponent* InventoryComponent, UStudentPerceptor* Perceptor, AActor* ItemActor)
{
	if (!Owner || !InventoryComponent || !ItemActor) return false;

	const float DistanceToItem = FVector::Dist(Owner->GetActorLocation(), ItemActor->GetActorLocation());
	const float AllowedPickupDistance = GetPickupRange(InventoryComponent) + 75.f;

	if (DistanceToItem > AllowedPickupDistance) return false;

	const int32 Capacity = GetInventoryCapacity(InventoryComponent);

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (TryGrabItemInSlot(InventoryComponent, SlotIndex, ItemActor))
		{
			if (Perceptor)
			{
				Perceptor->SeenItems.Remove(ItemActor);
			}

			return true;
		}
	}

	return false;
}

bool FZombieInventoryHelper::TryGrabItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex, AActor* ItemActor)
{
	if (!InventoryComponent) return false;

	UFunction* GrabFunction = InventoryComponent->FindFunction(TEXT("GrabItem"));
	if (!GrabFunction) return false;

	struct FParams
	{
		int32 SlotIdx;
		AActor* Item;
		bool ReturnValue;
	};

	FParams Params;
	Params.SlotIdx = SlotIndex;
	Params.Item = ItemActor;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(GrabFunction, &Params);

	return Params.ReturnValue;
}

bool FZombieInventoryHelper::TryUseItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex)
{
	if (!InventoryComponent) return false;

	UFunction* UseFunction = InventoryComponent->FindFunction(TEXT("UseItem"));
	if (!UseFunction) return false;

	struct FParams
	{
		int32 SlotIdx;
		bool ReturnValue;
	};

	FParams Params;
	Params.SlotIdx = SlotIndex;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(UseFunction, &Params);

	return Params.ReturnValue;
}

bool FZombieInventoryHelper::TryRemoveItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex)
{
	if (!InventoryComponent) return false;

	UFunction* RemoveFunction = InventoryComponent->FindFunction(TEXT("RemoveItem"));
	if (!RemoveFunction) return false;

	struct FParams
	{
		int32 SlotIdx;
		bool ReturnValue;
	};

	FParams Params;
	Params.SlotIdx = SlotIndex;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(RemoveFunction, &Params);

	return Params.ReturnValue;
}

bool FZombieInventoryHelper::DoesInventorySlotContainItemType(UActorComponent* InventoryComponent, int32 SlotIndex, const FString& ItemType)
{
	if (!InventoryComponent) return false;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return false;

	struct FParams
	{
		TArray<AActor*> ReturnValue;
	};

	FParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	if (!Params.ReturnValue.IsValidIndex(SlotIndex)) return false;

	AActor* Item = Params.ReturnValue[SlotIndex];
	if (!Item) return false;

	return Item->GetName().Contains(ItemType) || Item->GetClass()->GetName().Contains(ItemType);
}

bool FZombieInventoryHelper::HasInventoryItemType(UActorComponent* InventoryComponent, const FString& ItemType)
{
	const int32 Capacity = GetInventoryCapacity(InventoryComponent);

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (DoesInventorySlotContainItemType(InventoryComponent, SlotIndex, ItemType))
		{
			return true;
		}
	}

	return false;
}

bool FZombieInventoryHelper::IsInventoryFull(UActorComponent* InventoryComponent)
{
	if (!InventoryComponent) return true;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return true;

	struct FParams
	{
		TArray<AActor*> ReturnValue;
	};

	FParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	for (AActor* Item : Params.ReturnValue)
	{
		if (!Item) return false;
	}

	return true;
}

float FZombieInventoryHelper::GetPickupRange(UActorComponent* InventoryComponent)
{
	if (!InventoryComponent) return 100.f;

	UFunction* RangeFunction = InventoryComponent->FindFunction(TEXT("GetPickupRange"));
	if (!RangeFunction) return 100.f;

	struct FParams
	{
		float ReturnValue;
	};

	FParams Params;
	Params.ReturnValue = 100.f;

	InventoryComponent->ProcessEvent(RangeFunction, &Params);

	return Params.ReturnValue;
}

int32 FZombieInventoryHelper::GetInventoryCapacity(UActorComponent* InventoryComponent)
{
	if (!InventoryComponent) return 5;

	UFunction* CapacityFunction = InventoryComponent->FindFunction(TEXT("GetInventoryCapacity"));
	if (!CapacityFunction) return 5;

	struct FParams
	{
		int32 ReturnValue;
	};

	FParams Params;
	Params.ReturnValue = 5;

	InventoryComponent->ProcessEvent(CapacityFunction, &Params);

	return Params.ReturnValue;
}

int32 FZombieInventoryHelper::GetInventorySlotItemValue(UActorComponent* InventoryComponent, int32 SlotIndex)
{
	if (!InventoryComponent) return 0;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return 0;

	struct FInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FInventoryParams InventoryParams;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &InventoryParams);

	if (!InventoryParams.ReturnValue.IsValidIndex(SlotIndex)) return 0;

	AActor* Item = InventoryParams.ReturnValue[SlotIndex];
	if (!Item) return 0;

	UFunction* GetValueFunction = Item->FindFunction(TEXT("GetValue"));
	if (!GetValueFunction) return 0;

	struct FValueParams
	{
		int32 ReturnValue;
	};

	FValueParams ValueParams;
	ValueParams.ReturnValue = 0;

	Item->ProcessEvent(GetValueFunction, &ValueParams);

	return ValueParams.ReturnValue;
}

int32 FZombieInventoryHelper::GetLowestInventoryPrioritySlot(UActorComponent* InventoryComponent)
{
	if (!InventoryComponent) return INDEX_NONE;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return INDEX_NONE;

	struct FParams
	{
		TArray<AActor*> ReturnValue;
	};

	FParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	int32 LowestSlot = INDEX_NONE;
	int32 LowestPriority = 999;

	for (int32 SlotIndex = 0; SlotIndex < Params.ReturnValue.Num(); ++SlotIndex)
	{
		AActor* Item = Params.ReturnValue[SlotIndex];
		if (!Item) continue;

		const int32 Priority = GetItemPriority(Item);

		if (Priority < LowestPriority)
		{
			LowestPriority = Priority;
			LowestSlot = SlotIndex;
		}
	}

	return LowestSlot;
}

bool FZombieInventoryHelper::TryReplaceInventoryItem(UActorComponent* InventoryComponent, UStudentPerceptor* Perceptor, AActor* NewItem)
{
	if (!InventoryComponent || !NewItem) return false;

	const int32 NewPriority = GetItemPriority(NewItem);
	const int32 LowestSlot = GetLowestInventoryPrioritySlot(InventoryComponent);

	if (LowestSlot == INDEX_NONE) return false;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return false;

	struct FParams
	{
		TArray<AActor*> ReturnValue;
	};

	FParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);
	if (!Params.ReturnValue.IsValidIndex(LowestSlot)) return false;
	
	AActor* ExistingItem = Params.ReturnValue[LowestSlot];
	if (!ExistingItem) return false;
	
	const int32 ExistingPriority = GetItemPriority(ExistingItem);
	if (NewPriority <= ExistingPriority) return false;
	
	TryRemoveItemInSlot(InventoryComponent, LowestSlot);
	if (TryGrabItemInSlot(InventoryComponent, LowestSlot, NewItem))
	{
		if (Perceptor)
		{
			Perceptor->SeenItems.Remove(NewItem);
		}

		return true;
	}
	
	return false;
}