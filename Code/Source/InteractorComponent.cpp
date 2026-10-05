
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

    void InteractorComponent::StartInteractionWithCurrentTarget()
    {
        // Stays invalid if no overlap finder answers: EventResult does not touch it without a handler.
        AZ::EntityId currentTargetId;
        InteractionOverlapFinderRequestBus::EventResult(
            currentTargetId, GetEntityId(), &InteractionOverlapFinderRequests::GetCurrentTarget);

        if (!InteractableRequestBus::HasHandlers(currentTargetId)) { return; }

        StartInteraction(currentTargetId);
    }

    void InteractorComponent::StartInteraction([[maybe_unused]] AZ::EntityId targetId)
    {
        // TODO: core path shared by player and AI.
        if (!InteractableRequestBus::HasHandlers(targetId)) { return; }

    }

    void InteractorComponent::StopInteraction()
    {
        // TODO: cancel a hold in progress.
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
                editContext->Class<InteractorComponent>(
                    "Interactor",
                    "Initiates interactions with entities that have an Interactable component. "
                    "Works the same on players and NPCs: it receives a target entity and does not care how that target was found.")
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
            behaviorContext->Class<InteractorComponent>("InteractorComponent")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ;

            behaviorContext->EBus<InteractorRequestBus>("InteractorRequestBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Event("StartInteraction", &InteractorRequests::StartInteraction)
                ->Event("StartInteractionWithCurrentTarget", &InteractorRequests::StartInteractionWithCurrentTarget)
                ->Event("StopInteraction", &InteractorRequests::StopInteraction)
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
