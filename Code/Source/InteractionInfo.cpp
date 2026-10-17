
#include <InteractionSystem/InteractionInfo.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    void LocalizedText::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<LocalizedText>()
                ->Version(1)
                ->Field("Key", &LocalizedText::m_key)
                ->Field("DefaultText", &LocalizedText::m_defaultText)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                // No ShowChildrenOnly here (unlike InteractionInfo): each LocalizedText field of a component
                // is shown as its own collapsible group, titled by that field's DataElement name ("On Press Label", ...).
                editContext->Class<LocalizedText>("Localized Text",
                    "Player-visible text: a localization key plus the text shown without a translation.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &LocalizedText::m_key,
                        "Key",
                        "Localization key, e.g. door.open. Leave empty if this text is not localized: "
                        "then Default Text is shown as is.")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &LocalizedText::m_defaultText,
                        "Default Text",
                        "Shown when there is no translation for the key, or no localization at all. "
                        "Leave both fields empty to show nothing.")
                    ;
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            // A plain value for scripts. Script Canvas can create one: Class<T>() registers the default
            // constructor automatically for default-constructible types (see BehaviorClassBuilder.inl).
            behaviorContext->Class<LocalizedText>("LocalizedText")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Attribute(AZ::Script::Attributes::Storage, AZ::Script::Attributes::StorageType::Value)
                ->Property("Key", BehaviorValueProperty(&LocalizedText::m_key))
                ->Property("DefaultText", BehaviorValueProperty(&LocalizedText::m_defaultText))
                ->Method("IsEmpty", &LocalizedText::IsEmpty)
                ;
        }
    }

    void InteractionInfo::Reflect(AZ::ReflectContext* context)
    {
        // InteractionInfo has LocalizedText fields, so that type is reflected together with it.
        LocalizedText::Reflect(context);

        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            // Version 2: the three texts became LocalizedText (key + default text).
            // They are stored under NEW names on purpose: under the old names ("DisplayName", "OnPressLabel",
            // "OnHoldLabel") existing prefabs hold plain strings, which do not fit into a LocalizedText.
            // Old values are not converted: the JSON loader skips fields it does not know, so the texts
            // come back empty and have to be entered again in the inspector.
            serializeContext->Class<InteractionInfo>()
                ->Version(2)
                ->Field("Name", &InteractionInfo::m_displayName)
                ->Field("Mode", &InteractionInfo::m_mode)
                ->Field("PressLabel", &InteractionInfo::m_OnPressLabel)
                ->Field("HoldLabel", &InteractionInfo::m_OnHoldLabel)
                ->Field("HoldDuration", &InteractionInfo::m_holdDuration)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractionInfo>("Interaction Info", "Static description of an interactable entity.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractionInfo::m_displayName,
                        "Display Name",
                        "Name shown to the player in the interaction prompt. Leave both fields empty to show no name.")
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox,
                        &InteractionInfo::m_mode,
                        "Mode",
                        "Which interactions this entity supports.")
                        ->EnumAttribute(InteractionMode::Press, "Press")
                        ->EnumAttribute(InteractionMode::Hold, "Hold")
                        ->EnumAttribute(InteractionMode::PressOrHold, "Press or hold")
                        // Visibility is re-read only on a full rebuild: AttributesAndValues skips it.
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractionInfo::m_OnPressLabel,
                        "On Press Label",
                        "Optional label for the press interaction. Leave both fields empty to show no label.")
                        ->Attribute(AZ::Edit::Attributes::Visibility, &InteractionInfo::GetOnPressLabelVisibility)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractionInfo::m_OnHoldLabel,
                        "On Hold Label",
                        "Optional label for the hold interaction. Leave both fields empty to show no label.")
                        ->Attribute(AZ::Edit::Attributes::Visibility, &InteractionInfo::GetOnHoldLabelVisibility)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractionInfo::m_holdDuration,
                        "Hold Duration",
                        "How long the button must be held for a Hold interaction. "
                        "In 'Press or hold' mode it also separates a press from a hold. Ignored in 'Press' mode.")
                        ->Attribute(AZ::Edit::Attributes::Visibility, &InteractionInfo::GetHoldDurationVisibility)
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " s")
                    ;
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            // Enums are plain integers for scripts: values are exposed as named constants
            // and the enum field goes through int lambdas (same pattern as AZ::TransformConfig in the engine).
            behaviorContext->Class<InteractionInfo>("InteractionInfo")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Attribute(AZ::Script::Attributes::Storage, AZ::Script::Attributes::StorageType::Value)
                ->Enum<static_cast<int>(InteractionMode::Press)>("Mode_Press")
                ->Enum<static_cast<int>(InteractionMode::Hold)>("Mode_Hold")
                ->Enum<static_cast<int>(InteractionMode::PressOrHold)>("Mode_PressOrHold")
                ->Property("DisplayName", BehaviorValueProperty(&InteractionInfo::m_displayName))
                ->Property("Mode",
                    [](InteractionInfo* info) { return static_cast<int>(info->m_mode); },
                    [](InteractionInfo* info, const int& mode) { info->m_mode = static_cast<InteractionMode>(mode); })
                ->Property("OnPressLabel", BehaviorValueProperty(&InteractionInfo::m_OnPressLabel))
                ->Property("OnHoldLabel", BehaviorValueProperty(&InteractionInfo::m_OnHoldLabel))
                ->Property("HoldDuration", BehaviorValueProperty(&InteractionInfo::m_holdDuration))
                ;

            // InteractionType is not a field of any class, so its values are global constants.
            // EnumProperty (not Enum): Enum<V>() returns BehaviorContext*, which has no ->Attribute().
            behaviorContext->EnumProperty<static_cast<int>(InteractionType::Press)>("InteractionType_Press")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction");
            behaviorContext->EnumProperty<static_cast<int>(InteractionType::Hold)>("InteractionType_Hold")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction");
        }
    }

    AZ::Crc32 InteractionInfo::GetHoldDurationVisibility() const
    {
        return m_mode == InteractionMode::Press
            ? AZ::Edit::PropertyVisibility::Hide
            : AZ::Edit::PropertyVisibility::Show;
    }

    AZ::Crc32 InteractionInfo::GetOnPressLabelVisibility() const
    {
		return m_mode == InteractionMode::Press || m_mode == InteractionMode::PressOrHold
            ? AZ::Edit::PropertyVisibility::Show
			: AZ::Edit::PropertyVisibility::Hide;
	}

    AZ::Crc32 InteractionInfo::GetOnHoldLabelVisibility() const
    {
        return m_mode == InteractionMode::Hold || m_mode == InteractionMode::PressOrHold
            ? AZ::Edit::PropertyVisibility::Show
            : AZ::Edit::PropertyVisibility::Hide;
    }
} // namespace InteractionSystem
