## Traffic Mimicker
A client and server application, to mimic actual traffic on a ContainerNet(mininet) network.  
Let's define client as `Dispatcher` and the server as `TMAgent`.  
The server side(TMAgent) needs to receive the information about traffic to generate.
Based on this information contains the connection protocol and number of bytes transferred between two hosts(TMAgent).
When a tm-agent receives this info, it will decide whether it is the receiving end or sending end of traffic.
If receiving end, it will start listening for incoming traffic from sending end, otherwise it will send traffic to receiving end.
Traffic info contains following information:  
* granularity: #FIXME
* src ip, port: ipv4 address of the sending end
* dst ip, port: ipv4 address of the receiving end
* protocol: TCP/UDP
* init: whether it is the sending end or not(1/0)
* volume: number of bytes transferred

### TMAgent:

* Runs on ContainerNets hosts
* Listens on a certain port number for traffic info
* Once received info, looks at 'init' to decide whether it needs to initialize connection or not
* if 'init', then sends 'volume' much traffic to 'dst' address, 
* otherwise, listens on 'dst' address for 'volume' much traffic from 'src'

### Dispatcher

* Reads TimeSeries file
* Creates traffic info between two hosts
* sends them to corresponding TMAgents
