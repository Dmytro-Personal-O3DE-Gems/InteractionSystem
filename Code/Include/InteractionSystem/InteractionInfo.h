
#pragma once

#include <AzCore/base.h>
#include <AzCore/Math/Crc.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/string/string.h>

namespace AZ
{
    class ReflectContext;
}

namespace InteractionSystem
{
    //! What an interactable SUPPORTS. Configured per entity in the inspector.
    //! Serialized as a number: never reorder these values, only append new ones at the end.
    enum class InteractionMode : AZ::u8
    {
        Press,          //!< Fires on a single press of the interaction button.
        Hold,           //!< Fires once the button has been held for m_holdDuration.
        PressOrHold     //!< Release before m_holdDuration counts as a press, reaching it counts as a hold.
    };

    //! What the player actually DID. Passed to CanInteract() and to reaction components.
    //! Never PressOrHold: an actual interaction is always one or the other.
    //! Never reorder these values, only append new ones at the end.
    enum class InteractionType : AZ::u8
    {
        Press,
        Hold
    };

    //! Player-visible text that can be localized: a localization key plus the text to show without a translation.
    //! The interaction system only stores and passes it. Translating is the job of whoever displays it (e.g. the UI prompt),
    //! so this gem does not depend on any localization gem.
    //! How a displayer is expected to read it:
    //! - key and default text both empty -> nothing to show;
    //! - key empty                       -> show m_defaultText as is (a game without localization);
    //! - key set                         -> show the translation of m_key; if there is none, m_defaultText;
    //!                                      if that is empty too, m_key itself (a loud hint that a string is missing).
    struct LocalizedText
    {
        AZ_TYPE_INFO(LocalizedText, "{3612B7EA-7515-43BD-8EA1-6F19BA93FFF1}");
        AZ_CLASS_ALLOCATOR(LocalizedText, AZ::SystemAllocator);

        //! Registers this struct in the Serialize, Edit and Behavior contexts.
        //! Called from InteractionInfo::Reflect(), because InteractionInfo has fields of this type.
        static void Reflect(AZ::ReflectContext* context);

        //! True when there is nothing to show: no key and no default text.
        //! Also means "no opinion" in an answer to InteractableNotifications::ReturnInteractableLabel.
        bool IsEmpty() const
        {
            return m_key.empty() && m_defaultText.empty();
        }

        //! Localization key, e.g. "door.open". Empty means "not localized": m_defaultText is shown as is.
        AZStd::string m_key;

        //! Text shown when there is no translation for m_key (or no localization at all), e.g. "Open".
        AZStd::string m_defaultText;
    };

    //! Static description of an interactable entity.
    //! Used both as the Interactable component's configuration (edited in the inspector)
    //! and as the answer to "what is this thing?" for the Interactor and UI prompts.
    //! Runtime state such as "is it locked right now" does NOT belong here.
    struct InteractionInfo
    {
        AZ_TYPE_INFO(InteractionInfo, "{1FFAE517-5ECF-4AF9-BDAB-56151950B5B7}");
        AZ_CLASS_ALLOCATOR(InteractionInfo, AZ::SystemAllocator);

        //! Registers this struct and both enums in the Serialize, Edit and Behavior contexts.
        //! Must be called exactly once, from the Reflect() of a component whose descriptor is registered in the module.
        static void Reflect(AZ::ReflectContext* context);

        //! Editor-only: hides Hold Duration in the inspector when it has no effect (Press mode).
        //! Returns AZ::Edit::PropertyVisibility::Show or ::Hide.
        AZ::Crc32 GetHoldDurationVisibility() const;

        //! Editor-only: shows the press label only in modes that have a press (Press, PressOrHold).
        AZ::Crc32 GetOnPressLabelVisibility() const;

        //! Editor-only: shows the hold label only in modes that have a hold (Hold, PressOrHold).
        AZ::Crc32 GetOnHoldLabelVisibility() const;

        //! Name shown to the player, e.g. "Backpack". Empty means no name is shown.
        AZStd::string m_displayName;

        //! Which interactions this entity supports.
        InteractionMode m_mode = InteractionMode::Press;

        //! Default label of the press action, e.g. "Open". Used when no listener answers the label query
        //! (see InteractableRequests::GetInteractableLabel). Later this becomes a localization key.
        AZStd::string m_OnPressLabel;

        //! Default label of the hold action, e.g. "Lockpick". Same rules as m_OnPressLabel.
        AZStd::string m_OnHoldLabel;

        //! Seconds the button must be held for a Hold interaction.
        //! In PressOrHold mode it is also the threshold that separates a press from a hold.
        //! Ignored in Press mode.
        float m_holdDuration = 1.0f;
    };

    // An enum cannot hold AZ_TYPE_INFO inside itself, so its type info is declared next to it.
    // The macro generates free functions found by argument-dependent lookup,
    // so it must live in the enum's own namespace (or in namespace AZ), not in the global one.
    AZ_TYPE_INFO_SPECIALIZE(InteractionMode, "{FFB141C3-5E74-47B4-B5A0-88813A3A9F3D}");
    AZ_TYPE_INFO_SPECIALIZE(InteractionType, "{72BE4090-3ECE-40BE-A20E-7F379FA80324}");
} // namespace InteractionSystem
