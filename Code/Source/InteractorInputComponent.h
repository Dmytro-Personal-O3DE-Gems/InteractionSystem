
#pragma once

#include <AzCore/Component/Component.h>
#include <InteractionSystem/InteractorInputInterface.h>
#include <InteractionSystem/InteractorInterface.h>

#include <StartingPointInput/InputEventNotificationBus.h>

namespace InteractionSystem
{
    //! Player-only bridge from the "Interact" input action to the Interactor on the same entity.
    //! Button down -> StartInteractionWithCurrentTarget(), button up -> StopInteraction().
    //! It does not time anything: press/hold logic lives in the Interactor, so NPCs work without this component.
    class InteractorInputComponent
        : public AZ::Component
        , public InteractorInputRequestBus::Handler
        , protected StartingPointInput::InputEventNotificationBus::MultiHandler
    {
    public:
        AZ_COMPONENT_DECL(InteractorInputComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        void Activate() override;
        void Deactivate() override;

        // StartingPointInput::InputEventNotificationBus
        // OnHeld is not overridden: holding is timed by the Interactor, not by input.
        void OnPressed(float value) override;
        void OnReleased(float value) override;

    private:
        //! Action name from the .inputbindings asset. Name only = any local user.
        inline static const StartingPointInput::InputEventNotificationId InteractEventId{ "Interact" };
    };
} // namespace InteractionSystem
