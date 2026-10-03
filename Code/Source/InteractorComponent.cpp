
#include "InteractorComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    AZ_COMPONENT_IMPL(InteractorComponent, "InteractorComponent", "{1BB2245A-07ED-4ED9-B40F-FF0CD33F0436}");

    void InteractorComponent::Activate()
    {
        InteractorRequestBus::Handler::BusConnect(GetEntityId());
    }

    void InteractorComponent::Deactivate()
    {
        InteractorRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void InteractorComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractorComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractorComponent>("InteractorComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractorComponent>("Interactor Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "InteractionSystem Gem Group")
                ;
        }
    }

    void InteractorComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractorComponentService"));
    }

    void InteractorComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void InteractorComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractorComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
