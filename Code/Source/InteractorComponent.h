
#pragma once

#include <AzCore/Component/Component.h>
#include <InteractionSystem/InteractorInterface.h>
#include <InteractionSystem/InteractableInterface.h>
#include <InteractionSystem/InteractionOverlapFinderInterface.h>

#include <AzCore/Component/TickBus.h>

namespace InteractionSystem
{
    class InteractorComponent
        : public AZ::Component
        , public InteractorRequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(InteractorComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        void Activate() override;
        void Deactivate() override;

        // AZ::TickBus: connected only while a Hold / PressOrHold attempt is in progress.
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        // InteractorRequestBus
        void StartInteraction(AZ::EntityId targetId) override;
        void StartInteractionWithCurrentTarget() override;
        void StopInteraction() override;

    private:
        //! Asks the current target whether this interactor may perform the given interaction right now.
        bool CanInteractWithTarget(InteractionType type) const;

        //! Ends the current attempt (success, denial or cancel): stops the timer and clears all runtime state.
        //! Safe to call when no attempt is in progress.
        void EndInteraction();

        // Configuration (edited in the inspector, saved in the prefab).

        //! In PressOrHold mode a release within this many seconds counts as a press.
        float m_fInteractionWindow = 0.3f;

        // Runtime state of the current attempt (not reflected). An attempt is in progress while m_currentTargetId is valid.

        //! Seconds the button has been held in the current attempt.
        float m_fCurrentInteractionHoldTime = 0.0f;

        //! The entity being interacted with. Invalid when no attempt is in progress.
        AZ::EntityId m_currentTargetId;

        //! Snapshot of the target's InteractionInfo taken when the attempt started (mode, hold duration).
        InteractionInfo m_interactionInfo;
    };
} // namespace InteractionSystem
