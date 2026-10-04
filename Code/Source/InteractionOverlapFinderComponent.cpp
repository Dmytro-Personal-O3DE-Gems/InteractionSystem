
#include "InteractionOverlapFinderComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    //! Lets Script Canvas and Lua handle InteractionOverlapFinderNotificationBus events.
    class InteractionOverlapFinderNotificationBusBehaviorHandler
        : public InteractionOverlapFinderNotificationBus::Handler
        , public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(InteractionOverlapFinderNotificationBusBehaviorHandler, "{9DD3B145-8CC5-4496-A5AD-1D036D39B3C0}",
            AZ::SystemAllocator, OnTargetChanged);

        void OnTargetChanged(AZ::EntityId previousTargetId, AZ::EntityId newTargetId) override
        {
            Call(FN_OnTargetChanged, previousTargetId, newTargetId);
        }
    };

    AZ_COMPONENT_IMPL(InteractionOverlapFinderComponent, "InteractionOverlapFinderComponent", "{9FEDCC1A-63A3-42BE-965F-78D7C05B1EE8}");

    void InteractionOverlapFinderComponent::Activate()
    {
        // The request bus is addressed by the OWNER, so the Interactor can ask by its own id.
        if (m_ownerEntityId.IsValid())
        {
            InteractionOverlapFinderRequestBus::Handler::BusConnect(m_ownerEntityId);
        }
        else
        {
            AZ_Warning("InteractionOverlapFinder", false,
                "Owner Entity is not set on entity %s. The finder will not answer target requests.",
                GetEntityId().ToString().c_str());
        }
        AZ::TickBus::Handler::BusConnect();
    }

    void InteractionOverlapFinderComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        InteractionOverlapFinderRequestBus::Handler::BusDisconnect();

		m_currentTargetId = AZ::EntityId();
    }

    AZ::EntityId InteractionOverlapFinderComponent::GetCurrentTarget() const
    {
        return m_currentTargetId;
    }

    void InteractionOverlapFinderComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        // Cast once per tick: a second call could give a different answer (CanInteract listeners may change their mind).
        const AZ::EntityId newTargetId = CheckForOverlaps();
        if (newTargetId == m_currentTargetId)
        {
            return;
        }

        const AZ::EntityId previousTargetId = m_currentTargetId;
        m_currentTargetId = newTargetId;
        InteractionOverlapFinderNotificationBus::Event(
            m_ownerEntityId, &InteractionOverlapFinderNotifications::OnTargetChanged, previousTargetId, m_currentTargetId);
    }

    AZ::EntityId InteractionOverlapFinderComponent::CheckForOverlaps() const
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!sceneInterface)
        {
            return AZ::EntityId();
        }

        const AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        if (sceneHandle == AzPhysics::InvalidSceneHandle)
        {
            return AZ::EntityId();
        }

        AZ::Transform worldTransform = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTransform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

        AzPhysics::RayCastRequest request;
        request.m_start = worldTransform.GetTranslation();
        // O3DE is Z-up, Y-forward: the camera looks along its world Y basis.
        request.m_direction = worldTransform.GetBasisY().GetNormalized();
        request.m_distance = m_fCastDistance;

        // Ignore the owner's own colliders (e.g. the character capsule the camera sits inside).
        // With m_reportMultipleHits left at false, the ray stops at the closest non-owner hit,
        // so walls correctly block anything behind them.
        const AZ::EntityId ownerId = m_ownerEntityId;
        request.m_filterCallback =
            [ownerId](const AzPhysics::SimulatedBody* body, [[maybe_unused]] const Physics::Shape* shape)
            {
                if (body && body->GetEntityId() == ownerId)
                {
                    return AzPhysics::SceneQuery::QueryHitType::None;
                }
                return AzPhysics::SceneQuery::QueryHitType::Block;
            };

        const AzPhysics::SceneQueryHits result = sceneInterface->QueryScene(sceneHandle, &request);
        if (result.m_hits.empty())
        {
            return AZ::EntityId();
        }

        const AZ::EntityId hitEntityId = result.m_hits.front().m_entityId;

        // A wall is a valid hit, but it is not a target: only entities with an active Interactable count.
        if (!InteractableRequestBus::HasHandlers(hitEntityId))
        {
            return AZ::EntityId();
        }

        // Stays false if nobody answers.
        bool bCanInteract = false;
        InteractableRequestBus::EventResult(bCanInteract, hitEntityId, &InteractableRequests::CanInteract, m_ownerEntityId);

        return bCanInteract ? hitEntityId : AZ::EntityId();
    }

    void InteractionOverlapFinderComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractionOverlapFinderComponent, AZ::Component>()
                ->Version(1)
                ->Field("CastDistance", &InteractionOverlapFinderComponent::m_fCastDistance)
                ->Field("OwnerEntity", &InteractionOverlapFinderComponent::m_ownerEntityId)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractionOverlapFinderComponent>(
                    "Interaction Overlap Finder",
                    "Casts a ray every tick along this entity's forward axis (+Y) to find the interaction target in front of it. "
                    "Place it on the camera entity (or an NPC's head) and assign the owner entity.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Interaction")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractionOverlapFinderComponent::m_ownerEntityId,
                        "Owner Entity",
                        "Entity that owns this finder, usually the character root. "
                        "Its colliders are ignored by the ray so it never targets itself.")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractionOverlapFinderComponent::m_fCastDistance,
                        "Cast Distance",
                        "Maximum distance of the interaction ray.")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " m")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractionOverlapFinderComponent>("InteractionOverlapFinderComponent")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ;

            behaviorContext->EBus<InteractionOverlapFinderRequestBus>("InteractionOverlapFinderRequestBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Event("GetCurrentTarget", &InteractionOverlapFinderRequests::GetCurrentTarget)
                ;

            behaviorContext->EBus<InteractionOverlapFinderNotificationBus>("InteractionOverlapFinderNotificationBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Handler<InteractionOverlapFinderNotificationBusBehaviorHandler>()
                ;
        }
    }

    void InteractionOverlapFinderComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractionOverlapFinderComponentService"));
    }

    void InteractionOverlapFinderComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("InteractionOverlapFinderComponentService"));
    }

    void InteractionOverlapFinderComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractionOverlapFinderComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
