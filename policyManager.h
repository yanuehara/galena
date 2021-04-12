#pragma once

#include <set>

#include "ns3/socket.h"

#include "galenaConstants.h"

using namespace std;
using namespace ns3;

namespace galena{
    enum policyTrustComparator{GT};

    struct policy
    {
        set<Ipv6Address> ids;
        set<AuthenticationMechanisms> auth;
        policyTrustComparator trustCompare;
        double trust;
        set<string> contexts;
        string final;
    };
    

    class policyManager{
        private:
            vector<policy> policies;

            int rankPolicy(policy pol, Ipv6Address id,set<AuthenticationMechanisms> authm,  double trust, string context);
            int rankAddress(policy pol, Ipv6Address id);
            int rankAuthm(policy pol, set<AuthenticationMechanisms> authm);
            int rankTrust(policy pol, double trust);
            int rankContext(policy pol, string context);
        
        public:
            void addPolicy(policy pol);
            string applyPolicies(Ipv6Address id, set<AuthenticationMechanisms>, double trust, string context);

    };
}