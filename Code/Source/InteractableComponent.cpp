
#include "InteractableComponent.h"

#include <InteractionSystem/InteractionInfo.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    //! Lets Script Canvas and Lua handle InteractableNotificationBus events.
    class InteractableNotificationBusBehaviorHandler
        : public InteractableNotificationBus::Handler
        , public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(InteractableNotificationBusBehaviorHandler, "{B13063AF-8099-4C02-8028-CF44CE7FA742}",
            AZ::SystemAllocator, OnInteracted, OnCanInteractQuery);

        void OnInteracted(AZ::EntityId interactorId) override
        {
            Call(FN_OnInteracted, interactorId);
        }

        void OnCanInteractQuery(AZ::EntityId interactorId, bool& canInteract) override
        {
            Call(FN_OnCanInteractQuery, interactorId, canInteract);
        }
    };

    AZ_COMPONENT_IMPL(InteractableComponent, "InteractableComponent", "{11B4071B-3140-40FA-8904-A2804A3B70FE}");

    void InteractableComponent::Activate()
    {
        InteractableRequestBus::Handler::BusConnect(GetEntityId());
    }

    void InteractableComponent::Deactivate()
    {
        InteractableRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    InteractionInfo InteractableComponent::GetInteractionInfo() const
    {
        return m_info;
    }

    bool InteractableComponent::CanInteract(AZ::EntityId interactorId) const
    {
        bool canInteract = true;   // allowed unless someone objects
        InteractableNotificationBus::Event(GetEntityId(), &InteractableNotifications::OnCanInteractQuery, interactorId, canInteract);
        return canInteract;
    }

    void InteractableComponent::Interact(AZ::EntityId interactorId)
    {
        // The interactable does not know what the interaction does: reaction components on this entity decide.
        InteractableNotificationBus::Event(GetEntityId(), &InteractableNotifications::OnInteracted, interactorId);
    }

    void InteractableComponent::Reflect(AZ::ReflectContext* context)
    {
        // Reflect the data types this component uses before the component itself.
        InteractionInfo::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractableComponent, AZ::Component>()
                ->Version(1)
				->Field("InteractionInfo", &InteractableComponent::m_info)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractableComponent>(
                    "Interactable",
                    "Marks this entity as something an Interactor can interact with. "
                    "It does not define what happens: reaction components on the same entity listen to InteractableNotificationBus.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Interaction")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)

                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractableComponent::m_info,
                        "Interaction Info",
						"Static description of this interactable entity.")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractableComponent>("InteractableComponent")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ;

            behaviorContext->EBus<InteractableRequestBus>("InteractableRequestBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Event("GetInteractionInfo", &InteractableRequests::GetInteractionInfo)
                ->Event("CanInteract", &InteractableRequests::CanInteract)
                ->Event("Interact", &InteractableRequests::Interact)
                ;

            behaviorContext->EBus<InteractableNotificationBus>("InteractableNotificationBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Handler<InteractableNotificationBusBehaviorHandler>()
                ;
        }
    }

    void InteractableComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractableComponentService"));
    }

    void InteractableComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        // Only one Interactable per entity: InteractableRequestBus allows a single handler per address.
        incompatible.push_back(AZ_CRC_CE("InteractableComponentService"));
    }

    void InteractableComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractableComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
