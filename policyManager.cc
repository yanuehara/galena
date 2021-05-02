#include <algorithm>
#include <cmath>

#include "policyManager.h"

namespace galena{
    void policyManager::addPolicy(policy pol){
        this->policies.push_back(pol);
    }

    string policyManager::applyPolicies(Ipv6Address id, set<AuthenticationMechanisms> authm, double trust, string context){
        vector<int> ranking;

        for(policy pol: this->policies){
            int rank = this->rankPolicy(pol, id, authm, trust, context);
            ranking.push_back(rank);
        }

        int position = std::distance(ranking.begin(), max_element(ranking.begin(), ranking.end()));

        return this->policies[position].final;
    }

    int policyManager::rankPolicy(policy pol, Ipv6Address id, set<AuthenticationMechanisms> authm,  double trust, string context){
        int rank = 0;

        int rankAddressValue = this->rankAddress(pol, id);
        int rankAuthmValue = this->rankAuthm(pol, authm);
        int rankTrustValue = this->rankTrust(pol, trust);
        int rankContextValue = this->rankContext(pol, context);

        rank = rankAddressValue + rankAuthmValue + rankTrustValue + rankContextValue;

        return rank;
    }

    int policyManager::rankAddress(policy pol, Ipv6Address id){
        int value = 0;

        //If address set has one element
        if(pol.ids.size() == 1 ){
            if ((*pol.ids.begin()).IsAllNodesMulticast()){
                value += 1;
            }else if((*pol.ids.begin()) == id){
                value += 10;
            }
        } else {
            auto elementIterator = pol.ids.find(id);
            if( elementIterator == pol.ids.end() )
                ;
            else
                value += 1;
        }

        return value;
    }

    int policyManager::rankAuthm(policy pol, set<AuthenticationMechanisms> authm) {
        int value = 0;

        set<AuthenticationMechanisms> inter;
        
        bool is_included = includes(pol.auth.begin(), pol.auth.end(), authm.begin(), authm.end());
        
        //auto it = 
        set_symmetric_difference(pol.auth.begin(), pol.auth.end(), authm.begin(), authm.end(),
            std::inserter(inter, inter.begin())
        );

        if(is_included && inter.size() == 0){
            value += 10;
        }else{
            value += 1;
        }

        return value;
    }

    int policyManager::rankTrust(policy pol, double trust){
        int value = 0;

        switch (pol.trustCompare)
        {
        case policyTrustComparator::GT:{
                if(std::isgreater(trust, pol.trust) && std::isless(trust, pol.upperbound)){
                    value += 10;
                }
            }
            break;
        
        default:
            break;
        }

        return value;
    }

    int policyManager::rankContext(policy pol, string context){
        int value = 0;

        auto it = pol.contexts.find(context);
        if(it == pol.contexts.end())
            ;
        else{
            if(pol.contexts.size() == 1)
                value += 10;
            else
                value += 1;
        }
        
        return value;

    }
}