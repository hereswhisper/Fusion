#pragma once
#include <Windows.h>
#include "SDK.hpp"
#include "Memory.h"
#include "Helpers.h"
using namespace SDK;

namespace Abilities {
	static auto GiveAbilityFunc = reinterpret_cast<FGameplayAbilitySpecHandle(*)(UAbilitySystemComponent * ThisPtr, FGameplayAbilitySpecHandle * OutHandle, FGameplayAbilitySpec InSpec)>(Memory::FindPattern("48 89 5C 24 ? 56 57 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B F0 4C 8B F2 48 8B D9 7E 61"));

    void GrantGameplayAbility(APlayerPawn_Generic_C* TargetPawn, UClass* GameplayAbilityClass)
    {
        auto AbilitySystemComponent = TargetPawn->AbilitySystemComponent;

        if (!AbilitySystemComponent)
            return;

        auto GenerateNewSpec = [&]() -> FGameplayAbilitySpec
            {
                FGameplayAbilitySpecHandle Handle{ rand() };

                FGameplayAbilitySpec Spec{ -1, -1, -1, Handle, (UGameplayAbility*)GameplayAbilityClass->CreateDefaultObject(), 1, -1, nullptr, 0, false, false, false };

                return Spec;
            };

        auto Spec = GenerateNewSpec();

        for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++)
        {
            auto& CurrentSpec = AbilitySystemComponent->ActivatableAbilities.Items[i];

            if (CurrentSpec.Ability == Spec.Ability)
                return;
        }

        GiveAbilityFunc(AbilitySystemComponent, &Spec.Handle, Spec);
        return;
    }

	void GiveAllAbilities() {
		printf("\nTest Abilities\n");
        static auto SprintAbility = UObject::FindClass("Class FortniteGame.FortGameplayAbility_Sprint");
        static auto ReloadAbility = UObject::FindClass("Class FortniteGame.FortGameplayAbility_Reload");
        static auto RangedWeaponAbility = UObject::FindClass("Class FortniteGame.FortGameplayAbility_RangedWeapon");
        static auto JumpAbility = UObject::FindClass("Class FortniteGame.FortGameplayAbility_Jump");
        static auto DeathAbility = UObject::FindClass("BlueprintGeneratedClass GA_DefaultPlayer_Death.GA_DefaultPlayer_Death_C");
        static auto InteractUseAbility = UObject::FindClass("BlueprintGeneratedClass GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C");
        static auto InteractSearchAbility = UObject::FindClass("BlueprintGeneratedClass GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C");
        static auto EmoteAbility = UObject::FindClass("BlueprintGeneratedClass GAB_Emote_Generic.GAB_Emote_Generic_C");
        static auto TrapAbility = UObject::FindClass("BlueprintGeneratedClass GA_TrapBuildGeneric.GA_TrapBuildGeneric_C");
        static auto DanceGrenadeAbility = UObject::FindClass("BlueprintGeneratedClass GA_DanceGrenade_Stun.GA_DanceGrenade_Stun_C");

        GrantGameplayAbility(Globals::Pawn, SprintAbility);
        GrantGameplayAbility(Globals::Pawn, ReloadAbility);
        GrantGameplayAbility(Globals::Pawn, RangedWeaponAbility);
        GrantGameplayAbility(Globals::Pawn, JumpAbility);
        GrantGameplayAbility(Globals::Pawn, DeathAbility);
        GrantGameplayAbility(Globals::Pawn, InteractUseAbility);
        GrantGameplayAbility(Globals::Pawn, InteractSearchAbility);
        GrantGameplayAbility(Globals::Pawn, EmoteAbility);
        GrantGameplayAbility(Globals::Pawn, TrapAbility);
        GrantGameplayAbility(Globals::Pawn, DanceGrenadeAbility);

	}
}