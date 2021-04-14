#pragma once

#include "ns3/core-module.h"
#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/mobility-model.h"

#include "galenaDevicesProfile.h"
#include "galenaTag.h"
#include "galenaConstants.h"
#include "trustManager.h"
#include "policyManager.h"

using namespace ns3;

namespace galena{
    class GalenaApplication : public Application{
        private:
            int                                                     galenaProfileIndex;
            std::set<Ipv6Address>*                                  neighList;
            std::map<Ipv6Address, int>                              neighCap;
            std::map<Ipv6Address,
                std::set<AuthenticationMechanisms> >                neighAuthCap;
            std::map<Ipv6Address, double>                           neighLastSeen;
            Ptr<MobilityModel>                                      home;
            
            bool                                                    is_authenticating{false};
            Ipv6Address                                             is_authenticating_with;
            std::vector<double>                                     recomendations;
            string                                                  authMethod;


            Ptr<Socket>                                             m_socket;       	// Associated socket
            Address                                                 m_node;				// Node's
            TypeId                                                  m_tid;          	// Type of the socket used

        public:
            std::map<Ipv6Address, int>*                             nodemap;
            std::vector<AuthenticationMechanisms> authm{AuthenticationMechanisms::ECC,
                                                    AuthenticationMechanisms::RSA,
                                                    AuthenticationMechanisms::SIM,
                                                    AuthenticationMechanisms::NOPASS
                                                };
            policyManager* polManager;
            trustManager* tManager;

            static TypeId GetTypeId();
            void setup(int services, double homeX, double homeY);
            Ipv6Address GetNodeIpAddress();
            void StartApplication();
            void StopApplication();
            void recvCallback(Ptr<Socket> socket);

            void beacon();
            void requestServicesAuth(Ipv6Address peer); //Request Services and Authentication methods
            void responseServicesAuth(Ipv6Address peer); //Answer a request for services list and authentication
            void doHandleServiceAnswer(Ipv6Address peer, uint8_t* buffer, int size); //Receive a service answer
            void authenticationPhase2();
            double getDistance(int peerID);
            double getSimilarity(Ipv6Address peer);
            std::string getContext();

            int sendMessageHelper(MessageTypes type, Ipv6Address addr, uint8_t* buffer, int size);
            void clearTrust();
    };

}
