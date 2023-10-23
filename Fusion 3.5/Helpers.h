#pragma once
#include <Windows.h>
#include <stdio.h>
#include "SDK.hpp"

using namespace SDK;
namespace GeneralHelpers {
	void PrintDebugInfo() {
		printf("\n F u s i o n \n");
		printf("\n DLL Ver: 3.5 \n");
		printf("\n Author: Whisper \n");
	}

	void startConsole() {
		AllocConsole();
		FILE* Handle;
		freopen_s(&Handle, "CONOUT$", "w", stdout);
	}

	bool replace(std::string& str, const std::string& from, const std::string& to) { //Shouldn't be here, should make a utils file.
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}
}

namespace AntiSkid {
	int canPlay() {
		return 1; // TODO
	}
}

namespace Globals {
	AFortPlayerControllerOutpost* Controller;
	APlayerPawn_Generic_C* Pawn;
	AFortGameStateZone* GameState;
	AFortGameMode* Gamemode;
	UWorld* World;
	static auto BaseTransform = FTransform{}; // base transform.

	bool inGame = false;
}

namespace GameHelpers {
	inline UFortEngine* GetEngine()
	{
		static auto engine = UObject::FindObject<UFortEngine>("FortEngine_");
		return engine;
	}

	UGameplayStatics* GetGameplayStatics()
	{
		return reinterpret_cast<UGameplayStatics*>(UGameplayStatics::StaticClass());
	}

	bool IsNull(void* In)
	{
		if (In == nullptr)
			return true;

		return false;
	}

	template<typename T = AActor>
	T* SpawnActor(UClass* Class = nullptr, FVector Location = {}, FRotator Rotation = {})
	{
		if (IsNull(Class))
			Class = T::StaticClass();

		FQuat Quat;
		FTransform Transform;
		Quat.W = 0;
		Quat.X = Rotation.Pitch;
		Quat.Y = Rotation.Roll;
		Quat.Z = Rotation.Yaw;
		Transform.Rotation = Quat;
		Transform.Scale3D = FVector{ 1,1,1 };
		Transform.Translation = Location;

		auto GS = GetGameplayStatics();
		auto Actor = GS->STATIC_BeginDeferredActorSpawnFromClass(Globals::World, Class, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr);
		GS->STATIC_FinishSpawningActor(Actor, Transform);
		return static_cast<T*>(Actor);
	}

	// Start Pawn Play
	APlayerPawn_Generic_C* InitPawn(FTransform SpawnPos) {
		auto Pawn = GetGameplayStatics()->STATIC_BeginDeferredActorSpawnFromClass(Globals::World, APlayerPawn_Generic_C::StaticClass(), SpawnPos, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, nullptr);
		Pawn = GetGameplayStatics()->STATIC_FinishSpawningActor(Pawn, SpawnPos);

		return (APlayerPawn_Generic_C*)Pawn;
	}

	void StartGame(UObject* Object, UFunction* Function, void* Params) {
		
	}

	void ServerLoadingScreenDropped(UObject* Object, UFunction* Function, void* Params) {

	}
}

