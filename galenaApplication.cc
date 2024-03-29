#include <algorithm>

#include "ns3/udp-socket-factory.h"
#include "ns3/ipv6.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/node-list.h"
#include "ns3/mobility-module.h"
#include "ns3/mac64-address.h"

#include "singletonLogger.h"
#include "galenaApplication.h"
#include "galenaSybil.h"
#include "galenaDataProvenance.h"
#include "galenaUUIDTag.h"

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

        auto logger = SingletonLogger::getInstance();
        logger->writeEntry("Setup " + std::to_string((*this->nodemap)[this->GetNodeIpAddress()]) + " successfully");
    }

    int GalenaApplication::sendMessageHelper(MessageTypes type, Ipv6Address addr, uint8_t* buffer, int size){
        uint16_t port = 2020;
        int status;
        Inet6SocketAddress remote = Inet6SocketAddress(addr, port);

        Ptr<Packet> pack = Create<Packet>(buffer, size);
        galenaTag tag;
        tag.SetSimpleValue(type);
        pack->AddPacketTag(tag);

        if (type == MessageTypes::TrustAnswer){
            uuidTag uuid_tag;
            boost::uuids::uuid uuid_value;
            boost::uuids::random_generator uuid_gen;
            
            uuid_value = uuid_gen();
            uuid_tag.SetUuidValue(uuid_value);
            pack->AddPacketTag(uuid_tag);

            auto logger = SingletonLogger::getInstance();
            std::stringstream ss;

            if (this->tManager->attack != AttackType::NoAttack){
                NS_LOG_INFO("(" << this->GetNodeIpAddress() << "," << addr << ", A)=>" << uuid_value);
                ss << "(" << this->GetNodeIpAddress() << "," << addr << ", A)=>" << uuid_value;
            }else{
                NS_LOG_INFO("(" << this->GetNodeIpAddress() << "," << addr << ")=>" << uuid_value);
                ss << "(" << this->GetNodeIpAddress() << "," << addr << ")=>" << uuid_value;
            }
            logger->writeEntry(ss.str());
        }

        bool isSybil = this->tManager->attack == AttackType::SybilConstant || this->tManager->attack == AttackType::SybilOnOff;

        if(isSybil && type == MessageTypes::TrustAnswer){
            uint8_t buffer[8];
            Ptr<NormalRandomVariable> x = CreateObject<NormalRandomVariable> ();
            for (size_t i = 0; i < 8; i++)
                buffer[i] = (uint8_t) x->GetInteger();

            Mac64Address mac;
            mac.CopyFrom(buffer);
            Ipv6Address sybil = Ipv6Address::MakeAutoconfiguredAddress(mac, Ipv6Address("fe80::/64"));

            galenaSybil new_addr;
            new_addr.SetSimpleValue(sybil);

            pack->AddPacketTag(new_addr);
        }
        
        status = this->m_socket->SendTo(pack, 0, remote);

        if(status == -1){
            NS_LOG_INFO(this->GetNodeIpAddress() << "Could not send package");
        }

        return status;
    }

    void GalenaApplication::StartApplication(){
        if(this->validateProvenance){
            stringstream ss;
            ss << "/NodeList/" << GetNode()->GetId() << "/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/MonitorSnifferTx";
            Config::ConnectWithoutContext(ss.str(), MakeCallback(&GalenaApplication::signMessage, this));
            ss.str(std::string());
            ss << "/NodeList/" << GetNode()->GetId() << "/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/MonitorSnifferRx";
            Config::ConnectWithoutContext(ss.str(), MakeCallback(&GalenaApplication::getRxRSSI, this));
        }


        // If socket is not created yet
        if(!this->m_socket){
            // Create socket
            auto netdev = this->GetNode()->GetDevice(1);
            
            this->m_socket = Socket::CreateSocket(GetNode(), m_tid);
            this->m_socket->BindToNetDevice(netdev);
            
            this->m_socket->SetAllowBroadcast(true);
            this->m_socket->Bind(Inet6SocketAddress(Ipv6Address::GetAny (), 2020));
            this->m_socket->Listen();
            this->m_socket->SetRecvCallback(MakeCallback (&GalenaApplication::recvCallback, this));
        }

        // Run beaconing
        Simulator::ScheduleNow(&GalenaApplication::beacon, this);
        stringstream ss;
        ss << "/NodeList/" << GetNode()->GetId() << "/$ns3::MobilityModel/CourseChange";
        Config::ConnectWithoutContext(ss.str(), MakeCallback (&GalenaApplication::beaconCallback, this));

        double stop1 = stod(getenv("GALENA_STOP1"), nullptr);
        double stop2 = stod(getenv("GALENA_STOP2"), nullptr);
        double stop3 = stod(getenv("GALENA_STOP3"), nullptr);
        double stop4 = stod(getenv("GALENA_STOP4"), nullptr);

        if (stop1 >= 0.0)
            Simulator::Schedule(Seconds(stop1), &GalenaApplication::clearTrust, this);

        if (stop2 >= 0.0)
            Simulator::Schedule(Seconds(stop2), &GalenaApplication::clearTrust, this);

        if (stop3 >= 0.0)
            Simulator::Schedule(Seconds(stop3), &GalenaApplication::clearTrust, this);

        if (stop4 >= 0.0)
            Simulator::Schedule(Seconds(stop4), &GalenaApplication::clearTrust, this);
    }

    void GalenaApplication::StopApplication(){
        this->m_socket->Close();
    }

    void GalenaApplication::beacon(){
        Ipv6Address addr = Ipv6Address("FF02::1");
        this->sendMessageHelper(MessageTypes::Beacon, addr, nullptr, 0);

        //Simulator::Schedule(Seconds(5), &GalenaApplication::beacon, this);
    }

    void GalenaApplication::beaconCallback(Ptr<const MobilityModel> model){
        this->beacon();
    }

    void GalenaApplication::recvCallback(Ptr<Socket> socket){
        Ptr<Packet> packet;
        Address from;
        Ipv6Address fromIP;
        galenaTag tag;
        galenaSybil sybilTag;
        uuidTag uuid_tag;
        
        while((packet = socket->RecvFrom(from))){
            fromIP = Inet6SocketAddress::ConvertFrom(from).GetIpv6();
            packet->PeekPacketTag(tag);

            if(packet->PeekPacketTag(sybilTag)){
                fromIP = sybilTag.GetSimpleValue();
                stringstream ss;
                ss << "Sybil IP " << fromIP;
                SingletonLogger::getInstance()->writeEntry(ss.str());
            }

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

                    if(!this->validateProvenance){
                        this->recomendations.push_back(trust);
                    }else if(this->validateProvenance && this->data_provenance_matches){
                        this->recomendations.push_back(trust);
                    }else if(this->validateProvenance && !this->data_provenance_matches){
                        packet->PeekPacketTag(uuid_tag);

                        auto logger = SingletonLogger::getInstance();
                        stringstream ss;
                        
                        NS_LOG_INFO("[" << this->GetNodeIpAddress() << "]" << " Discarded trust answer from " << fromIP << " " << uuid_tag.GetSimpleValue());
                        ss << "[" << this->GetNodeIpAddress() << "]" << " Discarded trust answer from " << fromIP << " " << uuid_tag.GetSimpleValue();

                        logger->writeEntry(ss.str());
                    }
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
                    Simulator::Schedule(Seconds(2.5), &GalenaApplication::clearAuthentication, this);
                }
                    break;
                case MessageTypes::ServiceExchange:{
                    this->is_authenticating = false;
                    std::string authenticationMethod = std::string((char*)buffer);
                    authenticationMethod = authenticationMethod.substr(0,8);

                    Ipv6Address myaddr = this->GetNodeIpAddress();
                    /*if( fromIP < myaddr){
                        std::swap(fromIP, myaddr);
                    }*/

                    auto logger = SingletonLogger::getInstance();
                    stringstream ss;

                    if(this->authMethod.compare(authenticationMethod) == 0){
                        // Auth successfull
                        this->tManager->updatePositiveInteractions(fromIP);
                        NS_LOG_INFO("AUTH END(" << fromIP << "," << authenticationMethod << "), (" << myaddr << "," << this->authMethod << ") AT " << Simulator::Now().GetSeconds() << " WITH AUTHM=" << authenticationMethod);
                        ss << "AUTH END(" << fromIP << "," << authenticationMethod << "), (" << myaddr << "," << this->authMethod << ") AT " << Simulator::Now().GetSeconds() << " WITH AUTHM=" << authenticationMethod;
                    }else{
                        NS_LOG_INFO("AUTH END(" << fromIP << "," << authenticationMethod << "), (" << myaddr << "," << this->authMethod << ") AT " << Simulator::Now().GetSeconds() << " WITH AUTHM=NONE");
                        ss << "AUTH END(" << fromIP << "," << authenticationMethod << "), (" << myaddr << "," << this->authMethod << ") AT " << Simulator::Now().GetSeconds() << " WITH AUTHM=NONE";
                    }

                    logger->writeEntry(ss.str());
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

        bool should_authenticate = (bool)x->GetInteger ();
        should_authenticate = should_authenticate && this->GetNodeIpAddress() < peer && !this->is_authenticating;

        if(should_authenticate){
            auto myaddr = this->GetNodeIpAddress();
            if (peer < myaddr){
                std::swap(peer, myaddr);
            }
            stringstream ss;
            NS_LOG_INFO("AUTH BEGIN(" << this->GetNodeIpAddress() << "), (" << peer << ") AT " << Simulator::Now().GetSeconds());
            ss << "AUTH BEGIN(" << this->GetNodeIpAddress() << "), (" << peer << ") AT " << Simulator::Now().GetSeconds();
            auto logger = SingletonLogger::getInstance();
            logger->writeEntry(ss.str());

            this->is_authenticating = true;
            this->is_authenticating_with = peer;

            this->sendMessageHelper(MessageTypes::ServiceRequest, peer, nullptr, 0);

            Ipv6Address addr = Ipv6Address("FF02::1");
            uint8_t buf[16];
            peer.Serialize(buf);
            this->sendMessageHelper(MessageTypes::TrustRequest, addr, buf, 16);
            Simulator::Schedule(Seconds(2), &GalenaApplication::authenticationPhase2, this);
            Simulator::Schedule(Seconds(2.5), &GalenaApplication::clearAuthentication, this);
        }
    }

    void GalenaApplication::authenticationPhase2(){
        this->is_authenticating = false;
        Ipv6Address peer = this->is_authenticating_with;

        int nodeIDPeer = (*this->nodemap)[peer];
        double distance = this->getDistance(nodeIDPeer);
        double normDistance = 1.0 - (distance/4000.0); //Minmax
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
        uint8_t* buffer = new uint8_t[bufsize+1]{'\0'};
        //std::copy(authenticationMethod.begin(), authenticationMethod.end(), &buffer[0]);
        memmove(buffer, authenticationMethod.c_str(), bufsize+1);

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
        auto logger = SingletonLogger::getInstance();
        logger->writeEntry("Clearing trust");
        NS_LOG_INFO("Clearing trust");
        this->tManager->clear();
    }

    void GalenaApplication::clearAuthentication(){
        this->is_authenticating = false;
    }

    void GalenaApplication::signMessage(const Ptr< const Packet > packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu){
        galenaTag tag;
        bool isGalena = packet->PeekPacketTag(tag);
        if(!isGalena || tag.GetSimpleValue() != MessageTypes::TrustAnswer)
            return;
        this->txPower = txVector.GetTxPowerLevel();

        Ipv6Address from, to;
        from = GetPacketSource(packet);
        to = GetPacketDestination(packet);
        
        
        galenaSybil sybil;
        bool isSybil = packet->PeekPacketTag(sybil);
        
        if(isSybil)
            from = sybil.GetSimpleValue();

        sign_message(from, to, packet, this->txPower);
    }

    void GalenaApplication::getRxRSSI(Ptr< const Packet > packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise){
        galenaTag tag;
        bool isGalena = packet->PeekPacketTag(tag);
        if(!isGalena || tag.GetSimpleValue() != MessageTypes::TrustAnswer)
            return;

        Ipv6Address from, to;
        from = GetPacketSource(packet);
        to = GetPacketDestination(packet);

        this->rxPower = txVector.GetTxPowerLevel();
        
        this->data_provenance_matches = verify_message(from, to, packet, this->rxPower);
    }
}