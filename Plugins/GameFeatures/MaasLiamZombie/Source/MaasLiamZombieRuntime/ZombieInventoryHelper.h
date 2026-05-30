#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieInventoryHelper
{
public:
	static AActor* GetBestItem(UStudentPerceptor* Perceptor, AActor* Owner);
	static int32 GetItemPriority(AActor* ItemActor);

	static bool TryPickupItem(AActor* Owner, UActorComponent* InventoryComponent, UStudentPerceptor* Perceptor, AActor* ItemActor);
	static bool TryGrabItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex, AActor* ItemActor);
	static bool TryUseItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex);
	static bool TryRemoveItemInSlot(UActorComponent* InventoryComponent, int32 SlotIndex);

	static bool DoesInventorySlotContainItemType(UActorComponent* InventoryComponent, int32 SlotIndex, const FString& ItemType);
	static bool HasInventoryItemType(UActorComponent* InventoryComponent, const FString& ItemType);
	static bool IsInventoryFull(UActorComponent* InventoryComponent);

	static float GetPickupRange(UActorComponent* InventoryComponent);
	static int32 GetInventoryCapacity(UActorComponent* InventoryComponent);
	static int32 GetInventorySlotItemValue(UActorComponent* InventoryComponent, int32 SlotIndex);

	static int32 GetLowestInventoryPrioritySlot(UActorComponent* InventoryComponent);
	static bool TryReplaceInventoryItem(UActorComponent* InventoryComponent, UStudentPerceptor* Perceptor, AActor* NewItem);
	
	static bool CanReplaceInventoryItem(UActorComponent* InventoryComponent, AActor* NewItem);
};