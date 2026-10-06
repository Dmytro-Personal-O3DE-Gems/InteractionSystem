
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <InteractionSystem/InteractionInfo.h>

namespace InteractionSystem
{
    //! Requests to an interactable entity.
    //! Address: the EntityId of the interactable itself.
    //! Called by an Interactor (player or NPC), or from Script Canvas.
    class InteractableRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractableRequests, "{EEEFAD27-6FD1-4459-BF21-A9EB4243A3EE}");

        //! Exactly one handler per entity: two answers to "can I interact?" would be ambiguous.
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        //! Static description of this interactable (display name, hold duration).
        virtual InteractionInfo GetInteractionInfo() const = 0;

        //! Whether the given interactor may interact with this entity right now
        //! (e.g. false if a door is locked or an item was already picked up).
        //! The Interactor asks this right before calling Interact().
        virtual bool CanInteract(AZ::EntityId interactorId, [[maybe_unused]] InteractionType type) const = 0;

        //! Performs the interaction. The interactable does not know what the interaction does:
        //! it only notifies reaction components through InteractableNotificationBus.
        virtual void Interact(AZ::EntityId interactorId, [[maybe_unused]] InteractionType type) = 0;
    };

    using InteractableRequestBus = AZ::EBus<InteractableRequests>;

    //! Notifications sent by an interactable entity.
    //! Address: the EntityId of the interactable itself.
    //! Reaction components (pickup, door, sound, quest trigger, Script Canvas) on the same entity listen here.
    class InteractableNotifications
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractableNotifications, "{FED0BC48-82A5-4D09-8F1C-8EB9755F203A}");

        //! Sent after a successful press interaction (InteractionType::Press).
        //! @param interactorId the entity that performed the interaction.
        virtual void OnPressInteracted([[maybe_unused]] AZ::EntityId interactorId) {}

        //! Sent after a successful hold interaction (InteractionType::Hold),
        //! once the button has been held for InteractionInfo::m_holdDuration.
        //! @param interactorId the entity that performed the interaction.
        virtual void OnHoldInteracted([[maybe_unused]] AZ::EntityId interactorId) {}

        //! Sent when an interactor tried to interact but CanInteract() said no
        //! (e.g. a locked door: play the "rattle the handle" animation and sound).
        //! @param interactorId the entity that tried to interact (player or NPC).
        //! @param type what it tried to do, so the reaction can differ for a press and a hold.
        virtual void OnInteractionDenied(
            [[maybe_unused]] AZ::EntityId interactorId,
            [[maybe_unused]] InteractionType type) {}

        //! Sent to the interactable's own entity when someone asks CanInteract().
        //! Listeners may only set canInteract to false, never to true.
        //! @param interactorId the entity that is asking if it can interact.
        //! @param type what the interactor is trying to do (press or hold), so a listener can block one and allow the other.
        //! @param canInteract true by default; set it to false to prevent the interaction.
        virtual void OnCanInteractQuery(
            [[maybe_unused]] AZ::EntityId interactorId,
            [[maybe_unused]] InteractionType type,
            [[maybe_unused]] bool& canInteract) {
        }
    };

    using InteractableNotificationBus = AZ::EBus<InteractableNotifications>;

} // namespace InteractionSystem
