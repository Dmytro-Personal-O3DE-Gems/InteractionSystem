
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace InteractionSystem
{
    //! Requests to an Interactor (the entity that initiates interactions).
    //! Address: the EntityId of the entity that owns the Interactor component.
    //! The player's input component and NPC AI both talk to the Interactor through this bus.
    class InteractorRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractorRequests, "{25EB8BBC-FEB4-4995-B8D3-49D5C832EEE4}");

        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        //! Starts an interaction with an explicit target (the "button down" moment).
        //! Press targets complete immediately; Hold and PressOrHold targets start a timer and complete
        //! on the hold threshold or on StopInteraction(). Ignored while another attempt is in progress.
        //! Entry point for AI, which picks its target by other means.
        virtual void StartInteraction(AZ::EntityId targetId) = 0;

        //! Starts an interaction with the target currently reported by the target finder
        //! (InteractionOverlapFinderRequestBus at this entity's address). Entry point for player input.
        virtual void StartInteractionWithCurrentTarget() = 0;

        //! The "button up" moment. In PressOrHold mode a quick release counts as a press;
        //! otherwise an attempt that has not completed yet is cancelled. Has no effect if no attempt is in progress.
        virtual void StopInteraction() = 0;
    };

    using InteractorRequestBus = AZ::EBus<InteractorRequests>;

} // namespace InteractionSystem
