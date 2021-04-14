#include <algorithm>

#include "ns3/udp-socket-factory.h"
#include "ns3/ipv6.h"
#include "ns3/node-list.h"
#include "ns3/mobility-module.h"

#include "galenaApplication.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("GALENA_Application");

namespace galena{
    Ipv6Address GalenaApplication::GetNodeIpAddress(){
        Ptr <Node> PtrNode = this->GetNode();
        Ptr<Ipv6> ipv6 = PtrNode->GetObject<Ipv6> ();
        Ipv6InterfaceAddress iaddr = ipv6->GetAddress(1, 0);
        Ipv6Address ipAddr = iaddr.GetAddress();

        return ipAddr;
    }

    TypeId GalenaApplication::GetTypeId(){
        static TypeId tid = TypeId ("ns3::NodeApplication")
            .SetParent<Application>()
            .SetGroupName("Applications")
            .AddConstructor<GalenaApplication>()
            .AddAttribute ("Protocol", "The type of protocol to use. This should be "
                    "a subclass of ns3::SocketFactory",
                    TypeIdValue (UdpSocketFactory::GetTypeId ()),
                    MakeTypeIdAccessor (&GalenaApplication::m_tid),
                    // This should check for SocketFactory as a parent
                    MakeTypeIdChecker ())
            ;

        return tid;
    }

    void GalenaApplication::setup(int services, double homeX, double homeY){
        this->m_node = GetNodeIpAddress();
        this->m_tid = ns3::UdpSocketFactory::GetTypeId();
        this->m_socket = this->GetNode()->GetObject<Socket>();


        //GALENA init
        this->neighList = new std::set<Ipv6Address>;
        this->galenaProfileIndex = services;
        this->polManager = new policyManager();
        this->tManager = new trustManager();
        this->home = CreateObject<ConstantPositionMobilityModel>();
        this->home->SetPosition(Vector3D(homeX, homeY, 0.0));

        NS_LOG_INFO("Setup " << (*this->nodemap)[this->GetNodeIpAddress()] << " successfully");
    }

    int GalenaApplication::sendMessageHelper(MessageTypes type, Ipv6Address addr, uint8_t* buffer, int size){
        uint16_t port = 2020;
        int status;
        Inet6SocketAddress remote = Inet6SocketAddress(addr, port);

        Ptr<Packet> pack = Create<Packet>(buffer, size);
        galenaTag tag;
        tag.SetSimpleValue(type);
        pack->AddPacketTag(tag);
        
        status = this->m_socket->SendTo(pack, 0, remote);

        if(status == -1){
            NS_LOG_INFO(this->GetNodeIpAddress() << "Could not send package");
        }

        return status;
    }

    void GalenaApplication::StartApplication(){
        // If socket is not created yet
        if(!this->m_socket){
            // Create socket
            auto netdev = this->GetNode()->GetDevice(2);
            
            this->m_socket = Socket::CreateSocket(GetNode(), m_tid);
            this->m_socket->BindToNetDevice(netdev);
            
            this->m_socket->SetAllowBroadcast(true);
            this->m_socket->Bind(Inet6SocketAddress(Ipv6Address::GetAny (), 2020));
            this->m_socket->Listen();
            this->m_socket->SetRecvCallback(MakeCallback (&GalenaApplication::recvCallback, this));
        }

        // Run beaconing
        Simulator::ScheduleNow(&GalenaApplication::beacon, this);
        double stop1 = stod(getenv("GALENA_STOP1"), nullptr);
        double stop2 = stod(getenv("GALENA_STOP2"), nullptr);
        double stop3 = stod(getenv("GALENA_STOP3"), nullptr);
        double stop4 = stod(getenv("GALENA_STOP4"), nullptr);

        Simulator::Schedule(Seconds(stop1), &GalenaApplication::clearTrust, this);
        Simulator::Schedule(Seconds(stop2), &GalenaApplication::clearTrust, this);
        Simulator::Schedule(Seconds(stop3), &GalenaApplication::clearTrust, this);
        Simulator::Schedule(Seconds(stop4), &GalenaApplication::clearTrust, this);
    }

    void GalenaApplication::StopApplication(){
        this->m_socket->Close();
    }

    void GalenaApplication::beacon(){
        Ipv6Address addr = Ipv6Address("FF02::1");
        this->sendMessageHelper(MessageTypes::Beacon, addr, nullptr, 0);

        Simulator::Schedule(Seconds(5), &GalenaApplication::beacon, this);
    }

    void GalenaApplication::recvCallback(Ptr<Socket> socket){
        Ptr<Packet> packet;
        Address from;
        Ipv6Address fromIP;
        galenaTag tag;
        
        while((packet = socket->RecvFrom(from))){
            fromIP = Inet6SocketAddress::ConvertFrom(from).GetIpv6();
            packet->PeekPacketTag(tag);
            uint8_t *buffer = new uint8_t[packet->GetSize()];
	  	    packet->CopyData (buffer, packet->GetSize());

            switch (tag.GetSimpleValue()){
                case MessageTypes::Beacon:{
                    this->neighList->insert(fromIP);

                    Simulator::Schedule(Seconds(0.1), &GalenaApplication::requestServicesAuth, this, fromIP);
                }
                    break;
                case MessageTypes::ServiceInquiry:
                    this->responseServicesAuth(fromIP);
                    break;
                case MessageTypes::ServiceAnswer:
                    Simulator::Schedule(Seconds(1.3), &GalenaApplication::doHandleServiceAnswer, this, fromIP, buffer, packet->GetSize());
                    //this->doHandleServiceAnswer(fromIP, buffer, packet->GetSize());
                    break;
                case MessageTypes::TrustRequest:{
                    Ipv6Address aboutNode;
                    aboutNode.Deserialize(buffer);

                    if(aboutNode == fromIP)
                        break;

                    double trust = this->tManager->getNeighTrust(aboutNode);
                    uint8_t *sendBuf = new uint8_t[sizeof(trust)];
                    memcpy(sendBuf, &trust, sizeof(trust));

                    Simulator::ScheduleNow(&GalenaApplication::sendMessageHelper, this, MessageTypes::TrustAnswer, fromIP, sendBuf, sizeof(sendBuf));
                }
                    break;
                case MessageTypes::TrustAnswer:{
                    if(!is_authenticating){
                        break;
                    }
                    double trust;
                    memcpy(&trust, buffer, sizeof(double));

                    this->recomendations.push_back(trust);
                }
                    break;
                case MessageTypes::ServiceRequest:{
                    if(this->is_authenticating){
                        break;
                    }
                    this->is_authenticating = true;
                    this->is_authenticating_with = fromIP;
                    
                    Ipv6Address addr = Ipv6Address("FF02::1");
                    uint8_t *buf = new uint8_t[16];
                    fromIP.Serialize(buf);
                    this->sendMessageHelper(MessageTypes::TrustRequest, addr, buf, 16);
                    Simulator::Schedule(Seconds(2), &GalenaApplication::authenticationPhase2, this);
                }
                    break;
                case MessageTypes::ServiceExchange:{
                    this->is_authenticating = false;
                    string authenticationMethod((char*)buffer);

                    Ipv6Address myaddr = this->GetNodeIpAddress();
                    if( fromIP < myaddr){
                        std::swap(fromIP, myaddr);
                    }
                    NS_LOG_INFO("AUTH END(" << fromIP << "), (" << myaddr << ") AT " << Simulator::Now().GetSeconds() << " WITH AUTHM=" << authenticationMethod);


                    if(authenticationMethod.compare(this->authMethod) == 0){
                        this->tManager->updatePositiveInteractions(fromIP);
                    }
                }
                    break;

                default:
                    break;
            }
        }
    }

    void GalenaApplication::requestServicesAuth(Ipv6Address peer){
        this->sendMessageHelper(MessageTypes::ServiceInquiry, peer, NULL, 0);
    }

    void GalenaApplication::responseServicesAuth(Ipv6Address peer){
        std::stringstream buf;

        buf << this->galenaProfileIndex << '+';


        for(auto it = this->authm.begin(); it != this->authm.end(); it++){
            buf << (*it);

            if(next(it, 1) != this->authm.end()){
                buf << ",";
            }
        }

        Simulator::ScheduleNow(&GalenaApplication::sendMessageHelper, this, MessageTypes::ServiceAnswer, peer, (uint8_t*) buf.str().c_str(), buf.str().size()+1);
    }

    void GalenaApplication::doHandleServiceAnswer(Ipv6Address peer, uint8_t* buffer, int size){
        //desserialize
        std::string authmString = std::string((char*)buffer);
        std::set<AuthenticationMechanisms>* authVector = new std::set<AuthenticationMechanisms>;
        size_t last = 0;
        size_t next = 0;

        next = authmString.find("+", last);
        int peerServices = atoi((authmString.substr(last, next-last)).c_str());
        last = next + 1;
        this->neighCap[peer] = peerServices;
        
        while ((next = authmString.find(",", last)) != string::npos) {
            authVector->insert(static_cast<AuthenticationMechanisms>(
                atoi((authmString.substr(last, next-last)).c_str())
            ));
            last = next + 1;
        }
        authVector->insert(static_cast<AuthenticationMechanisms>(
                atoi((authmString.substr(last, next-last)).c_str())
        ));

        this->neighAuthCap[peer] = *authVector;

        //decide if authenticate
        Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
        x->SetAttribute ("Min", DoubleValue (0));
        x->SetAttribute ("Max", DoubleValue (1.1));

        bool should_authenticate = (bool)x->GetValue ();
        should_authenticate = should_authenticate && this->GetNodeIpAddress() < peer && !this->is_authenticating;

        if(should_authenticate){
            auto myaddr = this->GetNodeIpAddress();
            if (peer < myaddr){
                std::swap(peer, myaddr);
            }
            NS_LOG_INFO("AUTH BEGIN(" << this->GetNodeIpAddress() << "), (" << peer << ") AT " << Simulator::Now().GetSeconds());
            this->is_authenticating = true;
            this->is_authenticating_with = peer;

            this->sendMessageHelper(MessageTypes::ServiceRequest, peer, nullptr, 0);

            Ipv6Address addr = Ipv6Address("FF02::1");
            uint8_t buf[16];
            peer.Serialize(buf);
            this->sendMessageHelper(MessageTypes::TrustRequest, addr, buf, 16);
            Simulator::Schedule(Seconds(2), &GalenaApplication::authenticationPhase2, this);
        }
    }

    void GalenaApplication::authenticationPhase2(){
        this->is_authenticating = false;
        Ipv6Address peer = this->is_authenticating_with;

        int nodeIDPeer = (*this->nodemap)[peer];
        double distance = this->getDistance(nodeIDPeer);
        double normDistance = 1.0 - exp(-distance/4000.0);
        double similarity = this->getSimilarity(peer);
        double lastInteraction = Simulator::Now().ToDouble(Time::S);
        string context = this->getContext();

        //Last interaction
        try
        {
            lastInteraction = this->neighLastSeen.at(peer);
        }
        catch(const std::out_of_range& e)
        {
            ;
        }
        double timedelta = std::abs(lastInteraction - Simulator::Now().ToDouble(Time::S));

        double trust = this->tManager->getGalenaTrust(peer, normDistance, similarity, timedelta, this->recomendations);
        string authenticationMethod = this->polManager->applyPolicies(peer, this->neighAuthCap[peer], trust, context);
        auto currentTime = Simulator::Now();
        this->neighLastSeen[peer] = currentTime.ToDouble(Time::S);
        this->authMethod = authenticationMethod;
        
        int bufsize = authenticationMethod.length();
        uint8_t* buffer = new uint8_t[bufsize]{'\0'};
        //std::copy(authenticationMethod.begin(), authenticationMethod.end(), &buffer[0]);
        memmove(buffer, authenticationMethod.c_str(), bufsize);

        //this->sendMessageHelper(MessageTypes::ServiceExchange, peer, buffer, authenticationMethod.length());
        Simulator::Schedule(Seconds(0.1), &GalenaApplication::sendMessageHelper, this, MessageTypes::ServiceExchange, peer, buffer, authenticationMethod.length());
    }

    double GalenaApplication::getDistance(int peerID){
        Ptr<Node> peerNode = NodeList::GetNode(peerID);
        Ptr<MobilityModel> model1 = this->GetNode()->GetObject<MobilityModel>();
        Ptr<MobilityModel> model2 = peerNode->GetObject<MobilityModel>();
        double distance = model1->GetDistanceFrom (model2);

        return distance;
    }

    double GalenaApplication::getSimilarity(Ipv6Address peer){
        double sim = 0;

        auto profile1 = galenaProfiles[this->galenaProfileIndex];
        auto profile2 = galenaProfiles[this->neighCap[peer]];

        auto cap1 = &(profile1).servicesOffered;
        auto cap2 = &(profile2).servicesOffered;
        auto intersection = new std::vector<galenaServices>(cap1->size()+cap2->size());

        std::sort(cap1->begin(), cap1->end());
        std::sort(cap2->begin(), cap2->end());

        auto it = std::set_intersection(cap1->begin(),cap1->end(),cap2->begin(),cap2->end(), intersection->begin());

        intersection->resize(it-intersection->begin());
        //sim = (double)intersection->size() / sqrt(cap1->size()*cap2->size());
        sim = (double)intersection->size() / (double)cap1->size();

        return sim;
    }

    std::string GalenaApplication::getContext(){
        Ptr<MobilityModel> model1 = this->GetNode()->GetObject<MobilityModel>();
        double distance = model1->GetDistanceFrom(this->home);

        string context;

        if(distance < 5.0){
            context = "home";
        }else{
            context = "any";
        }

        return context;
    }

    void GalenaApplication::clearTrust(){
        NS_LOG_INFO("Clearing trust");
        this->tManager->clear();
    }
}