
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
        virtual bool CanInteract(AZ::EntityId interactorId) const = 0;

        //! Performs the interaction. The interactable does not know what the interaction does:
        //! it only notifies reaction components through InteractableNotificationBus.
        virtual void Interact(AZ::EntityId interactorId) = 0;
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

        //! Sent after a successful interaction.
        //! @param interactorId the entity that performed the interaction.
        virtual void OnInteracted([[maybe_unused]] AZ::EntityId interactorId) {}
    };

    using InteractableNotificationBus = AZ::EBus<InteractableNotifications>;

} // namespace InteractionSystem
