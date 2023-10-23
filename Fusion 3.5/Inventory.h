#pragma once
#include <Windows.h>
#include "SDK.hpp"
#include "Helpers.h"
using namespace SDK;

namespace Inventory {
	bool AreGuidsEqual(FGuid guidA, FGuid guidB) {
		if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
			return true;
		else
			return false;
	}
	void Update() {
		Globals::Controller->WorldInventory->HandleInventoryLocalUpdate();
		Globals::Controller->HandleWorldInventoryLocalUpdate();
		Globals::Controller->OnRep_QuickBar();
		Globals::Controller->QuickBars->OnRep_PrimaryQuickBar();
		Globals::Controller->QuickBars->OnRep_SecondaryQuickBar();
	}

	int GetOpenSlot() {
		for (int i = 0; i < 6; i++) {
			if (Globals::Controller->QuickBars->PrimaryQuickBar.Slots[i].Items.Data == nullptr) {
				return i;
			}
		}
		return -1;
	}

	int GetItemSlot(FGuid ItemGuid) {
		for (int i = 0; i < 6; i++) {
			if (Globals::Controller->QuickBars->PrimaryQuickBar.Slots[i].Items.Data != nullptr && AreGuidsEqual(Globals::Controller->QuickBars->PrimaryQuickBar.Slots[i].Items[0], ItemGuid)) {
				return i;
			}
		}
		return -1;
	}

	void EquipItem(UFortWorldItem* Weapon) {
		UFortWeaponItemDefinition* ItemDef = (UFortWeaponItemDefinition*)Weapon->GetItemDefinitionBP();
		AFortWeapon* WeaponData = Globals::Pawn->EquipWeaponDefinition(ItemDef, Weapon->GetItemGuid());
		WeaponData->OnRep_ReplicatedWeaponData();
		WeaponData->ClientGivenTo(Globals::Pawn);
		Globals::Pawn->ClientInternalEquipWeapon(WeaponData);
	}

	void SpawnPickup(UFortWorldItem* Item) {
		FTransform SpawnTransform = Globals::BaseTransform;
		SpawnTransform.Translation = Globals::Pawn->K2_GetActorLocation();
		AFortPickupAthena* Pickup = reinterpret_cast<AFortPickupAthena*>(GameHelpers::GetGameplayStatics()->STATIC_FinishSpawningActor(GameHelpers::GetGameplayStatics()->STATIC_BeginDeferredActorSpawnFromClass(GameHelpers::GetEngine()->GameViewport->World, AFortPickupAthena::StaticClass(), SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Globals::Controller), SpawnTransform));
		Pickup->PrimaryPickupItemEntry = Item->ItemEntry;
		Pickup->OnRep_PrimaryPickupItemEntry();
		Pickup->TossPickup(Globals::Pawn->K2_GetActorLocation(), Globals::Pawn, Pickup->PrimaryPickupItemEntry.Count, true);
	}

	void DropItem(FGuid ItemGuid) {
		int ItemSlot = GetItemSlot(ItemGuid);
		for (int i = 0; i < Globals::Controller->WorldInventory->Inventory.ItemInstances.Num(); i++) {
			if (AreGuidsEqual(Globals::Controller->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid, ItemGuid)) {
				SpawnPickup(Globals::Controller->WorldInventory->Inventory.ItemInstances[i]);
				Globals::Controller->WorldInventory->Inventory.ItemInstances.RemoveAt(i);
				break;
			}
		}
		for (int i = 0; i < Globals::Controller->WorldInventory->Inventory.ReplicatedEntries.Num(); i++) {
			if (AreGuidsEqual(Globals::Controller->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid, ItemGuid)) {
				Globals::Controller->WorldInventory->Inventory.ReplicatedEntries.RemoveAt(i);
				break;
			}
		}
		Globals::Controller->QuickBars->ServerRemoveItemInternal(ItemGuid, false, true);
		Globals::Controller->QuickBars->EmptySlot(EFortQuickBars::Primary, ItemSlot);
		Globals::Controller->QuickBars->PrimaryQuickBar.Slots[ItemSlot].Items.Data = nullptr;
		Globals::Controller->QuickBars->PrimaryQuickBar.Slots[ItemSlot].Items.Reset();
		Update();
	}

	UFortWorldItem* AddItemToInventory(UFortItemDefinition* ItemDef, int Slot, int Count = 1, EFortQuickBars QBs = EFortQuickBars::Primary) {
		UFortWorldItem* Weapon = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count, 1);
		Globals::Controller->WorldInventory->Inventory.ReplicatedEntries.Add(Weapon->ItemEntry);
		Globals::Controller->WorldInventory->Inventory.ItemInstances.Add(Weapon);
		Globals::Controller->QuickBars->ServerAddItemInternal(Weapon->GetItemGuid(), QBs, Slot);
		Update();
		return Weapon;
	}

	void HandleNewItem(UFortItemDefinition* ItemDef, int Count = 1) {
		int Slot = GetOpenSlot();
		if (Slot != -1) {
			AddItemToInventory(ItemDef, Slot, Count);
		}
		else {
			FGuid CurrentSlot = {};
			if (Globals::Controller->QuickBars->PrimaryQuickBar.CurrentFocusedSlot != 0) {
				CurrentSlot = Globals::Controller->QuickBars->PrimaryQuickBar.Slots[Globals::Controller->QuickBars->PrimaryQuickBar.CurrentFocusedSlot].Items[0];
			}
			else {
				CurrentSlot = Globals::Controller->QuickBars->PrimaryQuickBar.Slots[1].Items[0];
			}

			DropItem(CurrentSlot);
			AddItemToInventory(ItemDef, Globals::Controller->QuickBars->PrimaryQuickBar.CurrentFocusedSlot, Count);
		}
	}

	

	void EquipInventoryItem(FGuid ItemGuid) {
		for (int i = 0; i < Globals::Controller->WorldInventory->Inventory.ItemInstances.Num(); i++) {
			// Globals::Controller->WorldInventory->Inventory.ItemInstances[i]->GetItemGuid() == ItemGuid
			if (AreGuidsEqual(Globals::Controller->WorldInventory->Inventory.ItemInstances[i]->GetItemGuid(), ItemGuid)) {
				EquipItem(Globals::Controller->WorldInventory->Inventory.ItemInstances[i]);
				break;
			}
		}
	}

	void GiveMaterials() {
		// FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData
		// FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData
		// FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData

		auto WoodItemDef = UObject::FindObject<UFortResourceItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");
		auto StoneItemDef = UObject::FindObject<UFortResourceItemDefinition>("FortResourceItemDefinition StoneItemData.StoneItemData");
		auto MetalItemDef = UObject::FindObject<UFortResourceItemDefinition>("FortResourceItemDefinition MetalItemData.MetalItemData");

		AddItemToInventory(WoodItemDef, 0, 999, EFortQuickBars::Secondary);
		AddItemToInventory(StoneItemDef, 1, 999, EFortQuickBars::Secondary);
		AddItemToInventory(MetalItemDef, 2, 999, EFortQuickBars::Secondary);
	}

	void SetupInventory() {
		Globals::Controller->QuickBars = reinterpret_cast<AFortQuickBars*>(GameHelpers::GetGameplayStatics()->STATIC_FinishSpawningActor(GameHelpers::GetGameplayStatics()->STATIC_BeginDeferredActorSpawnFromClass(GameHelpers::GetEngine()->GameViewport->World, AFortQuickBars::StaticClass(), Globals::BaseTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Globals::Controller), Globals::BaseTransform));
		auto WorldInventory = Globals::Controller->WorldInventory;
		auto Inventory = WorldInventory->Inventory;
		auto Pickaxe = UObject::FindObject<UAthenaPickaxeItemDefinition>("AthenaPickaxeItemDefinition DefaultPickaxe.DefaultPickaxe");
		auto PickaxeWeaponDef = UObject::FindObject<UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");

		auto WallItemDef = UObject::FindObject<UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall");
		auto StairItemDef = UObject::FindObject<UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W");
		auto ConeItemDef = UObject::FindObject<UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS");
		auto FloorItemDef = UObject::FindObject<UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor");

		EquipItem(AddItemToInventory(PickaxeWeaponDef, 0));

		AddItemToInventory(WallItemDef, 0, 1, EFortQuickBars::Secondary);
		AddItemToInventory(FloorItemDef, 1, 1, EFortQuickBars::Secondary);
		AddItemToInventory(StairItemDef, 2, 1, EFortQuickBars::Secondary);
		AddItemToInventory(ConeItemDef, 3, 1, EFortQuickBars::Secondary);
	}
}