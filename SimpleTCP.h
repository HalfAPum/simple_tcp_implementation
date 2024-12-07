//
// Created by o.narvatov on 11/24/2024.
//

#ifndef SIMPLETCP_H
#define SIMPLETCP_H
#include <string>
#include <unordered_map>

#include "Constants.h"
#include "tcb/LocalConnection.h"
#include "ReceiveParams.h"
#include "tcb/TransmissionControlBlock.h"

constexpr unsigned DEFAULT_TIMEOUT = 5 * 60 * 1000;

class SimpleTCP {
    const char* listenAddress;
    const uint16_t listenPort;

    SOCKET listenSocket = 0;

    std::unordered_map<std::string, TransmissionControlBlock*> tcbMap {};

    void listenNewConnections();
public:
    SimpleTCP(const char* _localAddress, const uint16_t _localPort) : listenAddress(_localAddress), listenPort(_localPort) {}

    bool initialize() const;

    LocalConnection open(
        uint16_t localPort,
        uint16_t foreignPort = EPHEMERAL_PORT,
        bool passive = true,
        unsigned timeout = DEFAULT_TIMEOUT
    );

    /*
    * In the simplest implementation, SEND would not return control to
        the sending process until either the transmission was complete
        or the timeout had been exceeded.  However, this simple method
        is both subject to deadlocks (for example, both sides of the
        connection might try to do SENDs before doing any RECEIVEs) and
        offers poor performance, so it is not recommended.  A more
        sophisticated implementation would return immediately to allow
        the process to run concurrently with network I/O, and,
        furthermore, to allow multiple SENDs to be in progress.
        Multiple SENDs are served in first come, first served order, so
        the TCP will queue those it cannot service immediately.
     */
    void send(
        const LocalConnection &localConnection,
        const char* buffer,
        unsigned byteCount,
        bool PSH,
        bool URG,
        unsigned timeout = DEFAULT_TIMEOUT
    );

   /*
    *In the simplest implementation, control would not return to the
         calling program until either the buffer was filled, or some
         error occurred, but this scheme is highly subject to deadlocks.
         A more sophisticated implementation would permit several
         RECEIVEs to be outstanding at once.  These would be filled as
         segments arrive.  This strategy permits increased throughput at
         the cost of a more elaborate scheme (possibly asynchronous) to
         notify the calling program that a PUSH has been seen or a buffer
         filled.
    */
    ReceiveParams receive(
        LocalConnection &localConnection,
        char* buffer,
        unsigned byteCount
    );

    void close(const LocalConnection &localConnection);

    void abort(const LocalConnection &localConnection);

};



#endif //SIMPLETCP_H
