# Traktor.Online

set(TRAKTOR_ONLINE_SOURCES
    AttachmentResult.cpp
    IAchievements.cpp
    IGameConfiguration.cpp
    ILeaderboards.cpp
    ILobby.cpp
    IMatchMaking.cpp
    IParty.cpp
    ISaveData.cpp
    ISessionManager.cpp
    IStatistics.cpp
    IUser.cpp
    IVideoSharing.cpp
    IVoiceChat.cpp
    Impl/Achievements.cpp
    Impl/ITask.cpp
    Impl/Leaderboards.cpp
    Impl/Lobby.cpp
    Impl/MatchMaking.cpp
    Impl/Party.cpp
    Impl/SaveData.cpp
    Impl/SessionManager.cpp
    Impl/Statistics.cpp
    Impl/TaskQueue.cpp
    Impl/Tasks/TaskAchievement.cpp
    Impl/Tasks/TaskCreateLobby.cpp
    Impl/Tasks/TaskCreateParty.cpp
    Impl/Tasks/TaskEnumAchievements.cpp
    Impl/Tasks/TaskEnumLeaderboards.cpp
    Impl/Tasks/TaskEnumSaveData.cpp
    Impl/Tasks/TaskEnumStatistics.cpp
    Impl/Tasks/TaskFindMatchingLobbies.cpp
    Impl/Tasks/TaskGetLobbyParticipants.cpp
    Impl/Tasks/TaskGetSaveData.cpp
    Impl/Tasks/TaskGetScores.cpp
    Impl/Tasks/TaskJoinLobby.cpp
    Impl/Tasks/TaskLeaderboard.cpp
    Impl/Tasks/TaskRemoveSaveData.cpp
    Impl/Tasks/TaskSetLobbyMetaValue.cpp
    Impl/Tasks/TaskSetLobbyParticipantMetaValue.cpp
    Impl/Tasks/TaskSetSaveData.cpp
    Impl/Tasks/TaskStatistics.cpp
    Impl/Tasks/TaskUpdateSessionManager.cpp
    Impl/User.cpp
    Impl/UserCache.cpp
    Impl/VideoSharing.cpp
    Impl/VoiceChat.cpp
    LobbyArrayResult.cpp
    LobbyFilter.cpp
    LobbyResult.cpp
    Module.cpp
    OnlineClassFactory.cpp
    PartyResult.cpp
    Provider/IAchievementsProvider.cpp
    Provider/ILeaderboardsProvider.cpp
    Provider/IMatchMakingProvider.cpp
    Provider/ISaveDataProvider.cpp
    Provider/ISessionManagerProvider.cpp
    Provider/IStatisticsProvider.cpp
    Provider/IUserProvider.cpp
    Provider/IVideoSharingProvider.cpp
    Provider/IVoiceChatProvider.cpp
    Score.cpp
    ScoreArrayResult.cpp
    UserArrayResult.cpp
)

add_traktor_module(Traktor.Online
    SOURCES ${TRAKTOR_ONLINE_SOURCES}
    EXPORT_DEFINE T_ONLINE_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Sound
        Traktor.Drawing
        Traktor.Net
)

target_include_directories(Traktor.Online PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Online.Gc

set(TRAKTOR_ONLINE_GC_SOURCES
    Gc/GcGameConfiguration.cpp
)

add_traktor_module(Traktor.Online.Gc
    SOURCES ${TRAKTOR_ONLINE_GC_SOURCES}
    EXPORT_DEFINE T_ONLINE_GC_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Online
)

target_include_directories(Traktor.Online.Gc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Online.Lan

set(TRAKTOR_ONLINE_LAN_SOURCES
    Lan/LanAchievements.cpp
    Lan/LanGameConfiguration.cpp
    Lan/LanLeaderboards.cpp
    Lan/LanMatchMaking.cpp
    Lan/LanSaveData.cpp
    Lan/LanSessionManager.cpp
    Lan/LanStatistics.cpp
    Lan/LanUser.cpp
    Lan/LanVoiceChat.cpp
    Lan/Module.cpp
)

add_traktor_module(Traktor.Online.Lan
    SOURCES ${TRAKTOR_ONLINE_LAN_SOURCES}
    EXPORT_DEFINE T_ONLINE_LAN_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Net
        Traktor.Online
        Traktor.Drawing
)

target_include_directories(Traktor.Online.Lan PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Online.Local

set(TRAKTOR_ONLINE_LOCAL_SOURCES
    Local/LocalAchievements.cpp
    Local/LocalGameConfiguration.cpp
    Local/LocalLeaderboards.cpp
    Local/LocalMatchMaking.cpp
    Local/LocalSaveData.cpp
    Local/LocalSessionManager.cpp
    Local/LocalStatistics.cpp
    Local/LocalUser.cpp
    Local/LocalVideoSharing.cpp
    Local/Module.cpp
)

add_traktor_module(Traktor.Online.Local
    SOURCES ${TRAKTOR_ONLINE_LOCAL_SOURCES}
    EXPORT_DEFINE T_ONLINE_LOCAL_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Online
        Traktor.Sql
        Traktor.Sql.Sqlite3
)

target_include_directories(Traktor.Online.Local PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)