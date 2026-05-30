#pragma once

#include "CoreMinimal.h"

class FZombieCombatHelper
{
public:
	static bool TryUseWeapon(UActorComponent* InventoryComponent);

private:
	static bool DoesInventorySlotContainItemType(UActorComponent* InventoryComponent, int32 SlotIndex, const FString& ItemType);
	static bool TryUseItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex);
	static bool TryRemoveItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex);
	static int32 GetInventoryCapacity(UActorComponent* InventoryComponent);
	static int32 GetInventorySlotItemValue(UActorComponent* InventoryComponent, int32 SlotIndex);
};