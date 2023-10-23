#pragma once
#include "SDK.hpp"
#include "Helpers.h"
#include "Abilities.h"
#include "Inventory.h"
using namespace SDK;

//static void* (*ProcessEvent)(UObject*, UFunction*, void*);

bool isHooked = false;
bool isLoading = false;
bool hasSwitched = false;
void ProcessEventHook(UObject* Object, UFunction* Function, void* Paramaters) {

	if (!Object || !Function)
		return ProcessEvent(Object, Function, Paramaters);

	if (!isHooked) {
		isHooked = true;
		printf("\nprocessEvent hooked! can enter now\n");
	}

	auto FuncName = Function->GetName();
	auto FuncNameFull = Function->GetFullName();

	auto ObjName = Object->GetName();
	auto ObjNameFull = Object->GetFullName();

	if (FuncNameFull.contains("StartButton") && isLoading == false) {
		isLoading = true;
		printf("\nTime to load!\n");
		GameHelpers::GetEngine()->GameInstance->LocalPlayers[0]->PlayerController->SwitchLevel(L"Zone_Onboarding_FarmsteadFort");
	}

	if (FuncNameFull.contains("ReadyToStartMatch")) {
		if (hasSwitched == false) {
			hasSwitched = true;
			GameHelpers::SpawnActor<AFortGameStateZone>(nullptr); // spawn game state
			Globals::World = GameHelpers::GetEngine()->GameViewport->World;
			auto GameMode = reinterpret_cast<AFortGameMode*>(Object);
			auto GameState = (AFortGameStateZone*)GameMode->GameState;
			auto Controller = reinterpret_cast<AFortPlayerControllerOutpost*>(GameHelpers::GetEngine()->GameInstance->LocalPlayers[0]->PlayerController);
			auto PlayerState = (AFortPlayerStateZone*)Controller->PlayerState;

			Globals::Controller = Controller;

			Globals::GameState = GameState;

			FTransform SpawnPos = Globals::BaseTransform;

			auto PlayerStarts = GameMode->K2_FindPlayerStart(Globals::Controller, L"");

			FVector SpawnLoc = PlayerStarts->K2_GetActorLocation();

			if (PlayerStarts) {
				SpawnPos.Translation = SpawnLoc;
			}
			else {
				SpawnPos.Translation = { 0, 0, 5500 };
			}


			auto Pawn = GameHelpers::SpawnActor<APlayerPawn_Generic_C>(nullptr, SpawnPos.Translation);

			Globals::Pawn = Pawn;

			Globals::Controller->Possess(Globals::Pawn);

			Globals::Controller->OverriddenBackpackSize = 10000;
			Globals::Controller->bInfiniteAmmo = true;
			Globals::Controller->bCraftFree = true;

			Globals::Controller->bCraftFree = true;

			Inventory::SetupInventory();

			Globals::Controller->CheatManager->God();
			Globals::Controller->ServerSetClientHasFinishedLoading(true);
			Globals::Controller->bHasServerFinishedLoading = true;
			Globals::Controller->OnRep_bHasServerFinishedLoading();

			Globals::Controller->ServerReadyToStartMatch(); //magical Loading screen drop

			PlayerState->bIsGameSessionOwner = true;
			PlayerState->OnRep_SessionOwner();
			PlayerState->OnRep_CharacterParts();
			Globals::Pawn->OnCharacterPartsReinitialized();

			GameMode->StartMatch();
			GameMode->StartPlay();
		}
	}

	if (FuncNameFull.contains("ServerLoadingScreenDropped")) {
		Abilities::GiveAllAbilities();
	}

	if (FuncNameFull.contains("ServerCraftSchematic")) {
		auto Params = (AFortPlayerController_ServerCraftSchematic_Params*)Paramaters;
		if (Params)
		{
			auto ItemId = Params->ItemId.ToString();
			std::cout << "[HomebaseX] : Crafted: " << ItemId << std::endl;
			GeneralHelpers::replace(ItemId, "Schematic:", ""); //should just be the ID
			GeneralHelpers::replace(ItemId, "SID_", "WID_"); // mimic changing from SID to WID
			std::cout << "[HomebaseX] : WID Item: " << ItemId << std::endl;
			auto name = std::format("FortWeaponRangedItemDefinition {}.{}", ItemId.c_str(), ItemId.c_str());
			UFortItemDefinition* ItemDef = UObject::FindObject<UFortItemDefinition>(name);
			if (ItemDef)
			{
				Inventory::HandleNewItem(ItemDef, Inventory::GetOpenSlot());
			}
		}
	}


	return ProcessEvent(Object, Function, Paramaters);
}