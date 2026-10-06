
#include "InteractorComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    //! Lets Script Canvas and Lua handle InteractorNotificationBus events (e.g. a UI on the player).
    class InteractorNotificationBusBehaviorHandler
        : public InteractorNotificationBus::Handler
        , public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(InteractorNotificationBusBehaviorHandler, "{659C40A5-2567-4BC1-80CC-7CF35FF1C21A}",
            AZ::SystemAllocator, OnInteractionDenied);

        void OnInteractionDenied(AZ::EntityId targetId, InteractionType type) override
        {
            Call(FN_OnInteractionDenied, targetId, type);
        }
    };

    AZ_COMPONENT_IMPL(InteractorComponent, "InteractorComponent", "{1BB2245A-07ED-4ED9-B40F-FF0CD33F0436}");

    void InteractorComponent::Activate()
    {
        InteractorRequestBus::Handler::BusConnect(GetEntityId());

        // The overlap finder sends OnTargetChanged to its owner, which is this entity.
        // NPCs have no finder: the connection simply never receives anything.
        InteractionOverlapFinderNotificationBus::Handler::BusConnect(GetEntityId());
    }

    void InteractorComponent::Deactivate()
    {
        // Deactivated in the middle of a hold: drop the attempt so nothing fires later
        // and a re-activated component starts clean.
        EndInteraction();
        InteractionOverlapFinderNotificationBus::Handler::BusDisconnect(GetEntityId());
        InteractorRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void InteractorComponent::OnTick(float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        // Only Hold and PressOrHold attempts tick; both complete as a Hold once the threshold is reached,
        // without waiting for the button to be released.
        m_fCurrentInteractionHoldTime += deltaTime;
        if (m_fCurrentInteractionHoldTime < m_interactionInfo.m_holdDuration)
        {
            return;
        }

        // Second check: the state may have changed during the hold (door locked, item taken by someone else).
        if (CanInteractWithTarget(InteractionType::Hold))
        {
            InteractableRequestBus::Event(m_currentTargetId, &InteractableRequests::Interact, GetEntityId(), InteractionType::Hold);
        }
        else
        {
            NotifyInteractionDenied(InteractionType::Hold);
        }

        EndInteraction();
    }

    void InteractorComponent::OnTargetChanged([[maybe_unused]] AZ::EntityId previousTargetId, AZ::EntityId newTargetId)
    {
        // Compare with the target of the attempt, not with "any change": an attempt started through
        // StartInteraction(target) may have a target the finder is not looking at.
        // Losing the target (newTargetId invalid) also cancels.
        // This is a cancel, not a denial: nothing refused the interaction, the player looked away.
        if (m_currentTargetId.IsValid() && newTargetId != m_currentTargetId)
        {
            EndInteraction();
        }
    }

    void InteractorComponent::StartInteractionWithCurrentTarget()
    {
        // Stays invalid if no overlap finder answers: EventResult does not touch it without a handler.
        AZ::EntityId interactedEntityId;
        InteractionOverlapFinderRequestBus::EventResult(
            interactedEntityId, GetEntityId(), &InteractionOverlapFinderRequests::GetCurrentTarget);

        StartInteraction(interactedEntityId);
    }

    void InteractorComponent::StartInteraction(AZ::EntityId targetId)
    {
        // One attempt at a time: a second Start while holding is ignored.
        if (m_currentTargetId.IsValid())
        {
            return;
        }

        if (!InteractableRequestBus::HasHandlers(targetId))
        {
            return;
        }

        // Snapshot first: the mode decides which interaction type to ask about.
        m_currentTargetId = targetId;
        InteractableRequestBus::EventResult(m_interactionInfo, m_currentTargetId, &InteractableRequests::GetInteractionInfo);
        m_fCurrentInteractionHoldTime = 0.0f;

        switch (m_interactionInfo.m_mode)
        {
        case InteractionMode::Press:
            // Press fires immediately on button down; there is nothing to wait for.
            if (CanInteractWithTarget(InteractionType::Press))
            {
                InteractableRequestBus::Event(m_currentTargetId, &InteractableRequests::Interact, GetEntityId(), InteractionType::Press);
            }
            else
            {
                NotifyInteractionDenied(InteractionType::Press);
            }
            EndInteraction();
            return;

        case InteractionMode::Hold:
            if (!CanInteractWithTarget(InteractionType::Hold))
            {
                NotifyInteractionDenied(InteractionType::Hold);
                EndInteraction();
                return;
            }
            break;

        case InteractionMode::PressOrHold:
            // The type is not known yet: start if at least one of the two is allowed.
            if (!CanInteractWithTarget(InteractionType::Press) && !CanInteractWithTarget(InteractionType::Hold))
            {
                // Both are denied and the player has not chosen yet: reported as Press,
                // the action a quick release of this button would have produced.
                NotifyInteractionDenied(InteractionType::Press);
                EndInteraction();
                return;
            }
            break;

        default:
            AZ_Assert(false, "Unknown InteractionMode");
            EndInteraction();
            return;
        }

        // Hold and PressOrHold: wait for the threshold (OnTick) or for the release (StopInteraction).
        AZ::TickBus::Handler::BusConnect();
    }

    void InteractorComponent::StopInteraction()
    {
        // Button released. No attempt in progress: pressed at nothing, a Press that already fired,
        // or a Hold that already completed on the threshold.
        if (!m_currentTargetId.IsValid())
        {
            return;
        }

        // Only PressOrHold can turn a release into an interaction: a quick release is a press.
        // A Hold released before the threshold, or a PressOrHold released after the window, is cancelled.
        if (m_interactionInfo.m_mode == InteractionMode::PressOrHold
            && m_fCurrentInteractionHoldTime <= m_fInteractionWindow)
        {
            if (CanInteractWithTarget(InteractionType::Press))
            {
                InteractableRequestBus::Event(m_currentTargetId, &InteractableRequests::Interact, GetEntityId(), InteractionType::Press);
            }
            else
            {
                NotifyInteractionDenied(InteractionType::Press);
            }
        }

        EndInteraction();
    }

    bool InteractorComponent::CanInteractWithTarget(InteractionType type) const
    {
        // Stays false if the target has no Interactable anymore (e.g. it was destroyed during the hold).
        bool canInteract = false;
        InteractableRequestBus::EventResult(canInteract, m_currentTargetId, &InteractableRequests::CanInteract, GetEntityId(), type);
        return canInteract;
    }

    void InteractorComponent::NotifyInteractionDenied(InteractionType type) const
    {
        InteractableNotificationBus::Event(m_currentTargetId, &InteractableNotifications::OnInteractionDenied, GetEntityId(), type);
        InteractorNotificationBus::Event(GetEntityId(), &InteractorNotifications::OnInteractionDenied, m_currentTargetId, type);
    }

    void InteractorComponent::EndInteraction()
    {
        AZ::TickBus::Handler::BusDisconnect();   // no-op if not connected
        m_currentTargetId = AZ::EntityId();
        m_fCurrentInteractionHoldTime = 0.0f;
    }

    void InteractorComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractorComponent, AZ::Component>()
                ->Version(1)
                ->Field("InteractionWindow", &InteractorComponent::m_fInteractionWindow)
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
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractorComponent::m_fInteractionWindow,
                        "Press Window",
                        "For 'Press or hold' interactables: releasing the button within this time counts as a press. "
                        "Releasing later but before the interactable's Hold Duration cancels. "
                        "Keep it shorter than the shortest Hold Duration, otherwise a press can never happen.")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.05f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " s")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractorComponent>("InteractorComponent")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ;

            behaviorContext->EBus<InteractorNotificationBus>("InteractorNotificationBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Handler<InteractorNotificationBusBehaviorHandler>()
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

    void InteractorComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        // Only one Interactor per entity: InteractorRequestBus allows a single handler per address.
        incompatible.push_back(AZ_CRC_CE("InteractorComponentService"));
    }

    void InteractorComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractorComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
