
#include "InteractorInputComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    AZ_COMPONENT_IMPL(InteractorInputComponent, "InteractorInputComponent", "{9112ED04-97A4-466B-BBC6-F88557442FC4}");

    void InteractorInputComponent::Activate()
    {
        InteractorInputRequestBus::Handler::BusConnect(GetEntityId());
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(InteractEventId);
    }

    void InteractorInputComponent::Deactivate()
    {
        StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect();
        InteractorInputRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void InteractorInputComponent::OnPressed([[maybe_unused]] float value)
    {
        InteractorRequestBus::Event(GetEntityId(), &InteractorRequestBus::Events::StartInteractionWithCurrentTarget);
    }

    void InteractorInputComponent::OnReleased([[maybe_unused]] float value)
    {
        InteractorRequestBus::Event(GetEntityId(), &InteractorRequestBus::Events::StopInteraction);
    }

    void InteractorInputComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractorInputComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractorInputComponent>(
                    "Interactor Input",
                    "Player only. Forwards the 'Interact' input action to the Interactor on this entity: "
                    "button down starts an interaction, button up stops it. "
                    "Requires an Input component with an 'Interact' event in its input bindings. Do not add it to NPCs.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Interaction")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractorInputComponent>("InteractorInputComponent")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ;
        }
    }

    void InteractorInputComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractorInputComponentService"));
    }

    void InteractorInputComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        // Two of these on one entity would start every interaction twice.
        incompatible.push_back(AZ_CRC_CE("InteractorInputComponentService"));
    }

    void InteractorInputComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        // Without an Interactor every button press would silently go nowhere.
        // Required also guarantees the Interactor is activated before this component.
        required.push_back(AZ_CRC_CE("InteractorComponentService"));
    }

    void InteractorInputComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
