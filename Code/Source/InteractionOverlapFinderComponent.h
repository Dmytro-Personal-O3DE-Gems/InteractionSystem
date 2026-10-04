
#pragma once

#include <AzCore/Component/Component.h>
#include <InteractionSystem/InteractionOverlapFinderInterface.h>
#include <InteractionSystem/InteractableInterface.h>

#include <AzCore/Component/TickBus.h>

// Math
#include <AzCore/Math/Vector3.h>            // AZ::Vector3
#include <AzCore/Math/Transform.h>          // AZ::Transform
#include <AzCore/Component/TransformBus.h>  // AZ::TransformBus

#include <AzFramework/Physics/PhysicsSystem.h>              // AzPhysics::SystemInterface — GetSceneHandle / GetScene
#include <AzFramework/Physics/PhysicsScene.h>               // AzPhysics::Scene — method QueryScene(request)
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h> // RayCastRequest / ShapeCastRequest / OverlapRequest / SceneQueryHits / *RequestHelpers
#include <AzFramework/Physics/Collision/CollisionGroups.h>  // CollisionGroup


namespace InteractionSystem
{
    class InteractionOverlapFinderComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , public InteractionOverlapFinderRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(InteractionOverlapFinderComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time) override;

        // InteractionOverlapFinderRequestBus
        AZ::EntityId GetCurrentTarget() const override;

    private:
        float m_fCastDistance = 2.0f;
        AZ::EntityId m_ownerEntityId;
        AZ::EntityId m_currentTargetId;

		AZ::EntityId CheckForOverlaps() const;
    };
} // namespace InteractionSystem
