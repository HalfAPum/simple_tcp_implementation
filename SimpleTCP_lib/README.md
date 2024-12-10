# Simple TCP protocol implementation

This project is my attempt to implement TCP according to https://www.ietf.org/rfc/rfc793.txt. Project is designed for Windows systems.
In Windows, it's impossible to create pure TCP since Microsoft winsock2.h does not allow for sending packets over raw sockets.
In this implementation I use UDP (from https://www.ietf.org/rfc/rfc768.txt) as transport layer protocol with payload that contains TCP header and actual payload.
This is only available solution that allows you to fully recreate TCP state machine 
with cost of communicating only with clients/servers that know about this structure.

---

# TODO List
 - [X] Create TCP-User interface
 - [X] Add run config for passive/active servers
 - [X] Add IP header processing
 - [X] Add UDP header processing
 - [X] Add TCP header processing
 - [] Add checksum calculator for outgoing packets
 - [] Add checksum verification for incoming packets
 - [X] Create listening socket to accept incoming connections
 - [X] Successfully send/receive packets
 - [X] Create separate socket for each new connection
 - [] Manage connections in multithreaded environment
 - [] Create TCP state machine according to RFC 793
 - [] Describe protocol limitations in following sections
 - [] Add possibility include protocol as lib in other cpp projects

# Protocol Limitations

 - Protocol cannot be used with standard TCP implementations because specified Protocol in IP header in UDP.
 - Protocol can be used with standard UDP implementations although keep it mind that TCP header starts
where payload is expected and only then real payload is defined
 - Designed for Windows systems only