# Traktor.Jungle

set(TRAKTOR_JUNGLE_SOURCES
    INetworkTopology.cpp
    IPeer2PeerProvider.cpp
    IReplicatorEventListener.cpp
    IReplicatorStateListener.cpp
    JungleClassFactory.cpp
    MeasureP2PProvider.cpp
    Module.cpp
    OnlinePeer2PeerProvider.cpp
    Peer2PeerTopology.cpp
    Replicator.cpp
    ReplicatorProxy.cpp
    State/BodyStateTemplate.cpp
    State/BodyStateValue.cpp
    State/BooleanTemplate.cpp
    State/BooleanValue.cpp
    State/FloatTemplate.cpp
    State/FloatValue.cpp
    State/IValue.cpp
    State/IValueTemplate.cpp
    State/State.cpp
    State/StateTemplate.cpp
    State/TransformTemplate.cpp
    State/TransformValue.cpp
    State/VectorTemplate.cpp
    State/VectorValue.cpp
    Test/CaseValueTemplate.cpp
)

add_traktor_module(Traktor.Jungle
    SOURCES ${TRAKTOR_JUNGLE_SOURCES}
    EXPORT_DEFINE T_JUNGLE_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Physics
        Traktor.World
        Traktor.Online
)

target_include_directories(Traktor.Jungle PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)