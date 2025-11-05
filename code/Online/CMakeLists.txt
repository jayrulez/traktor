# Traktor.Online

set(TRAKTOR_ONLINE_SOURCES
	AttachmentResult.h
	IAchievements.h
	IGameConfiguration.h
	ILeaderboards.h
	ILobby.h
	IMatchMaking.h
	IParty.h
	ISaveData.h
	ISessionManager.h
	IStatistics.h
	IUser.h
	IVideoSharing.h
	IVoiceChat.h
	Impl/Achievements.h
	Impl/ITask.h
	Impl/Leaderboards.h
	Impl/Lobby.h
	Impl/MatchMaking.h
	Impl/Party.h
	Impl/SaveData.h
	Impl/SessionManager.h
	Impl/Statistics.h
	Impl/TaskQueue.h
	Impl/Tasks/TaskAchievement.h
	Impl/Tasks/TaskCreateLobby.h
	Impl/Tasks/TaskCreateParty.h
	Impl/Tasks/TaskEnumAchievements.h
	Impl/Tasks/TaskEnumLeaderboards.h
	Impl/Tasks/TaskEnumSaveData.h
	Impl/Tasks/TaskEnumStatistics.h
	Impl/Tasks/TaskFindMatchingLobbies.h
	Impl/Tasks/TaskGetLobbyParticipants.h
	Impl/Tasks/TaskGetSaveData.h
	Impl/Tasks/TaskGetScores.h
	Impl/Tasks/TaskJoinLobby.h
	Impl/Tasks/TaskLeaderboard.h
	Impl/Tasks/TaskRemoveSaveData.h
	Impl/Tasks/TaskSetLobbyMetaValue.h
	Impl/Tasks/TaskSetLobbyParticipantMetaValue.h
	Impl/Tasks/TaskSetSaveData.h
	Impl/Tasks/TaskStatistics.h
	Impl/Tasks/TaskUpdateSessionManager.h
	Impl/User.h
	Impl/UserCache.h
	Impl/VideoSharing.h
	Impl/VoiceChat.h
	LobbyArrayResult.h
	LobbyFilter.h
	LobbyResult.h
	Module.h
	OnlineClassFactory.h
	PartyResult.h
	Provider/IAchievementsProvider.h
	Provider/ILeaderboardsProvider.h
	Provider/IMatchMakingProvider.h
	Provider/ISaveDataProvider.h
	Provider/ISessionManagerProvider.h
	Provider/IStatisticsProvider.h
	Provider/IUserProvider.h
	Provider/IVideoSharingProvider.h
	Provider/IVoiceChatProvider.h
	Score.h
	ScoreArrayResult.h
	Types.h
	UserArrayResult.h
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
	Gc/GcGameConfiguration.h
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
	Lan/LanAchievements.h
	Lan/LanGameConfiguration.h
	Lan/LanLeaderboards.h
	Lan/LanMatchMaking.h
	Lan/LanSaveData.h
	Lan/LanSessionManager.h
	Lan/LanStatistics.h
	Lan/LanUser.h
	Lan/LanVoiceChat.h
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
	Local/LocalAchievements.h
	Local/LocalGameConfiguration.h
	Local/LocalLeaderboards.h
	Local/LocalMatchMaking.h
	Local/LocalSaveData.h
	Local/LocalSessionManager.h
	Local/LocalStatistics.h
	Local/LocalUser.h
	Local/LocalVideoSharing.h
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