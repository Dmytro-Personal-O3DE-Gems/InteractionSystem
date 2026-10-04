
#include <InteractionSystem/InteractionInfo.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    void InteractionInfo::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractionInfo>()
                ->Version(1)
                ->Field("DisplayName", &InteractionInfo::m_displayName)
                ->Field("Mode", &InteractionInfo::m_mode)
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
                        "Name shown to the player in the interaction prompt. Leave empty to show no name.")
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
} // namespace InteractionSystem
