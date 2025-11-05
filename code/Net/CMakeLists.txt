# Traktor.Net

set(TRAKTOR_NET_SOURCES
    BidirectionalObjectTransport.cpp
    Discovery/DiscoveryManager.cpp
    Discovery/DmFindServices.cpp
    Discovery/DmServiceInfo.cpp
    Discovery/IDiscoveryMessage.cpp
    Discovery/IService.cpp
    Discovery/NetworkService.cpp
    File/FileConnection.cpp
    Ftp/FtpClient.cpp
    Ftp/FtpConnection.cpp
    Http/HttpChunkStream.cpp
    Http/HttpClient.cpp
    Http/HttpClientResult.cpp
    Http/HttpConnection.cpp
    Http/HttpRequest.cpp
    Http/HttpRequestContent.cpp
    Http/HttpResponse.cpp
    Http/HttpServer.cpp
    Http/IHttpRequestContent.cpp
    IcmpSocket.cpp
    Module.cpp
    MulticastUdpSocket.cpp
    NetClassFactory.cpp
    Network.cpp
    Smtp/SmtpMail.cpp
    Socket.cpp
    SocketAddress.cpp
    SocketAddressIPv4.cpp
    SocketAddressIPv6.cpp
    SocketSet.cpp
    SocketStream.cpp
    Stream/RemoteStream.cpp
    Stream/StreamServer.cpp
    TcpSocket.cpp
    UdpSocket.cpp
    Url.cpp
    UrlConnection.cpp
)

add_traktor_module(Traktor.Net
    SOURCES ${TRAKTOR_NET_SOURCES}
    EXPORT_DEFINE T_NET_EXPORT
    DEPENDENCIES
        Traktor.Core
    SYSTEM_LIBS
        iphlpapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.Net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)