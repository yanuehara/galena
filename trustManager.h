#pragma once

#include <map>

#include "ns3/socket.h"

#include "galenaConstants.h"

using namespace std;
using namespace ns3;

namespace galena{
    class trustManager{
        private:
            map<Ipv6Address, double>    neighDirect;
            map<Ipv6Address, double>    neighIndirect;
            map<Ipv6Address, double>    neighTrust;
            map<Ipv6Address, int>       neighTotalInteractions; //micra
            map<Ipv6Address, int>       neighPositiveInteractions; //tau

            double alpha;
            double beta;
            double gamma;
            double delta;
            double lambda;
        
        public:
            Ipv6Address myaddr;
            std::string logdir;
            AttackType attack{AttackType::NoAttack};

            trustManager();

            double getGalenaTrust(Ipv6Address peer, double distance, double similarity, double timedelta, vector<double> rec); //Calculates and updates trust for peer
            double getDirectTrust(Ipv6Address peer); //Gets direct trust
            double getIndirectTrust(Ipv6Address peer); //Gets indirect trust
            double getNeighTrust(Ipv6Address peer);
            int getNeighInteractions(Ipv6Address peer); //Get number of interactions with node peer
            int getPositiveNeighInteractions(Ipv6Address peer);
            
            void updatePositiveInteractions(Ipv6Address peer); //Updates peer interactions
            void clear();
    };

}