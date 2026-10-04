
#pragma once

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/string/string.h>

namespace AZ
{
    class ReflectContext;
}

namespace InteractionSystem
{
    //! Static description of an interactable entity.
    //! Used both as the Interactable component's configuration (edited in the inspector)
    //! and as the answer to "what is this thing?" for the Interactor and UI prompts.
    //! Runtime state such as "is it locked right now" does NOT belong here.
    struct InteractionInfo
    {
        AZ_TYPE_INFO(InteractionInfo, "{1FFAE517-5ECF-4AF9-BDAB-56151950B5B7}");
        AZ_CLASS_ALLOCATOR(InteractionInfo, AZ::SystemAllocator);

        //! Registers this struct in the Serialize, Edit and Behavior contexts.
        //! Must be called exactly once, from the Reflect() of a component whose descriptor is registered in the module.
        static void Reflect(AZ::ReflectContext* context);

        //! Name shown to the player, e.g. "Backpack". Empty means no name is shown.
        AZStd::string m_displayName;

        //! How long the interaction button must be held, in seconds. 0 means a single press.
        float m_holdDuration = 0.0f;
    };
} // namespace InteractionSystem
