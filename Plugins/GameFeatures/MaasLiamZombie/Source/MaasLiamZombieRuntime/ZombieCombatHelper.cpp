#include "ZombieCombatHelper.h"

bool FZombieCombatHelper::TryUseWeapon(UActorComponent* InventoryComponent)
{
	if (!InventoryComponent)
	{
		return false;
	}

	const int32 Capacity = GetInventoryCapacity(InventoryComponent);

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (DoesInventorySlotContainItemType( InventoryComponent, SlotIndex, TEXT("Shotgun")) || DoesInventorySlotContainItemType(InventoryComponent, SlotIndex, TEXT("Pistol")))
		{
			const bool bUsedWeapon = TryUseItemInSlot(InventoryComponent, SlotIndex);

			if (GetInventorySlotItemValue(InventoryComponent, SlotIndex ) <= 0)
			{
				TryRemoveItemInSlot(InventoryComponent, SlotIndex);
			}

			return bUsedWeapon;
		}
	}

	return false;
}

bool FZombieCombatHelper::DoesInventorySlotContainItemType(UActorComponent* InventoryComponent, int32 SlotIndex, const FString& ItemType)
{
	if (!InventoryComponent)
	{
		return false;
	}

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));

	if (!GetInventoryFunction)
	{
		return false;
	}

	struct FInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FInventoryParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	if (!Params.ReturnValue.IsValidIndex(SlotIndex))
	{
		return false;
	}

	AActor* Item = Params.ReturnValue[SlotIndex];

	if (!Item)
	{
		return false;
	}

	return Item->GetName().Contains(ItemType) || Item->GetClass()->GetName().Contains(ItemType);
}

bool FZombieCombatHelper::TryUseItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return false;
	}

	UFunction* UseFunction = InventoryComponent->FindFunction(TEXT("UseItem"));

	if (!UseFunction)
	{
		return false;
	}

	struct FUseParams
	{
		int32 SlotIdx;
		bool ReturnValue;
	};

	FUseParams Params;
	Params.SlotIdx = SlotIndex;
	Params.ReturnValue = false;
	InventoryComponent->ProcessEvent(UseFunction, &Params);
	return Params.ReturnValue;
}

bool FZombieCombatHelper::TryRemoveItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return false;
	}

	UFunction* RemoveFunction = InventoryComponent->FindFunction(TEXT("RemoveItem"));

	if (!RemoveFunction)
	{
		return false;
	}

	struct FRemoveParams
	{
		int32 SlotIdx;
		bool ReturnValue;
	};

	FRemoveParams Params;
	Params.SlotIdx = SlotIndex;
	Params.ReturnValue = false;
	InventoryComponent->ProcessEvent(RemoveFunction,&Params);
	return Params.ReturnValue;
}

int32 FZombieCombatHelper::GetInventoryCapacity(UActorComponent* InventoryComponent)
{
	if (!InventoryComponent)
	{
		return 5;
	}

	UFunction* CapacityFunction = InventoryComponent->FindFunction(TEXT("GetInventoryCapacity"));

	if (!CapacityFunction)
	{
		return 5;
	}

	struct FCapacityParams
	{
		int32 ReturnValue;
	};

	FCapacityParams Params;
	Params.ReturnValue = 5;
	InventoryComponent->ProcessEvent(CapacityFunction, &Params);
	return Params.ReturnValue;
}

int32 FZombieCombatHelper::GetInventorySlotItemValue(UActorComponent* InventoryComponent, int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return 0;
	}

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));

	if (!GetInventoryFunction)
	{
		return 0;
	}

	struct FInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FInventoryParams InventoryParams;

	InventoryComponent->ProcessEvent(GetInventoryFunction, &InventoryParams);

	if (!InventoryParams.ReturnValue.IsValidIndex(SlotIndex))
	{
		return 0;
	}

	AActor* Item = InventoryParams.ReturnValue[SlotIndex];

	if (!Item)
	{
		return 0;
	}

	UFunction* GetValueFunction = Item->FindFunction(TEXT("GetValue"));

	if (!GetValueFunction)
	{
		return 0;
	}

	struct FValueParams
	{
		int32 ReturnValue;
	};

	FValueParams ValueParams;
	ValueParams.ReturnValue = 0;
	Item->ProcessEvent(GetValueFunction, &ValueParams);
	return ValueParams.ReturnValue;
}