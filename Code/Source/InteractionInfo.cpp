
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
                        AZ::Edit::UIHandlers::Default,
                        &InteractionInfo::m_holdDuration,
                        "Hold Duration",
                        "How long the interaction button must be held. 0 means a single press.")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " s")
                    ;
            }
        }

        if (auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractionInfo>("InteractionInfo")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Attribute(AZ::Script::Attributes::Storage, AZ::Script::Attributes::StorageType::Value)
                ->Property("DisplayName", BehaviorValueProperty(&InteractionInfo::m_displayName))
                ->Property("HoldDuration", BehaviorValueProperty(&InteractionInfo::m_holdDuration))
                ;
        }
    }
} // namespace InteractionSystem
