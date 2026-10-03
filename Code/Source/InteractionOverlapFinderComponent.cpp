
#include "InteractionOverlapFinderComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    AZ_COMPONENT_IMPL(InteractionOverlapFinderComponent, "InteractionOverlapFinderComponent", "{9FEDCC1A-63A3-42BE-965F-78D7C05B1EE8}");

    void InteractionOverlapFinderComponent::Activate()
    {
        InteractionOverlapFinderRequestBus::Handler::BusConnect(GetEntityId());
    }

    void InteractionOverlapFinderComponent::Deactivate()
    {
        InteractionOverlapFinderRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void InteractionOverlapFinderComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractionOverlapFinderComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractionOverlapFinderComponent>("InteractionOverlapFinderComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractionOverlapFinderComponent>("InteractionOverlapFinder Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "InteractionSystem Gem Group")
                ;
        }
    }

    void InteractionOverlapFinderComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractionOverlapFinderComponentService"));
    }

    void InteractionOverlapFinderComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void InteractionOverlapFinderComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractionOverlapFinderComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
