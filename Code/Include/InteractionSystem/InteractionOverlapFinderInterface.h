
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace InteractionSystem
{
    //! Requests to the target finder.
    //! Address: the OWNER entity (the one with the Interactor), not the camera entity the finder sits on.
    //! This lets the Interactor ask by its own GetEntityId() without knowing where the camera is.
    class InteractionOverlapFinderRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractionOverlapFinderRequests, "{017727A7-D77D-4900-BFA8-BF0243AA6F27}");

        //! One finder per owner: with two, the answer would depend on handler order.
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        //! Interactable currently in front of the owner. Invalid EntityId if there is none.
        virtual AZ::EntityId GetCurrentTarget() const = 0;
    };

    using InteractionOverlapFinderRequestBus = AZ::EBus<InteractionOverlapFinderRequests>;

    //! Notifications sent by the target finder.
    //! Address: the OWNER entity, same as the request bus.
    //! Intended for UI prompts and highlighting.
    class InteractionOverlapFinderNotifications
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractionOverlapFinderNotifications, "{4229F620-A5A4-4339-AC7B-3090622B0FD7}");

        //! Sent only when the target changes, including when it is lost (newTargetId is then invalid).
        virtual void OnTargetChanged(
            [[maybe_unused]] AZ::EntityId previousTargetId,
            [[maybe_unused]] AZ::EntityId newTargetId) {}
    };

    using InteractionOverlapFinderNotificationBus = AZ::EBus<InteractionOverlapFinderNotifications>;

} // namespace InteractionSystem
