#include <cmath>
#include <numeric>
#include <fstream>

#include "ns3/core-module.h"

#include "trustManager.h"

namespace galena{
    double trustManager::getDirectTrust(Ipv6Address peer){
        double direct;
            try
            {
                direct = this->neighDirect.at(peer);
            }
            catch(const std::out_of_range& e)
            {
                direct = this->neighDirect[peer] = 0.5;
            }
            
            return direct;
    }

    double trustManager::getIndirectTrust(Ipv6Address peer){
        double indirect;

        try
        {
            indirect = this->neighIndirect.at(peer);
        }
        catch(const std::out_of_range& e)
        {
            indirect = this->neighIndirect[peer] = 0.5;
        }
        
        return indirect;
    }

    int trustManager::getNeighInteractions(Ipv6Address peer){
        int totalInteractions;

        try
        {
            totalInteractions = this->neighTotalInteractions.at(peer);
        }
        catch(const std::out_of_range& e)
        {
            totalInteractions = this->neighTotalInteractions[peer] = 0;
        }
        
        return totalInteractions;
    }

    int trustManager::getPositiveNeighInteractions(Ipv6Address peer){
        int positiveInteractions;

        try
        {
            positiveInteractions = this->neighTotalInteractions.at(peer);
        }
        catch(const std::out_of_range& e)
        {
            positiveInteractions = this->neighTotalInteractions[peer] = 0;
        }
        
        return positiveInteractions;
    }

    double trustManager::getNeighTrust(Ipv6Address peer){
        double trust;
        try
        {
            trust = this->neighTrust.at(peer);
        }
        catch(const std::out_of_range& e)
        {
            trust = this->neighTrust[peer] = 0.5;
        }
        
        return trust;
    }

    void trustManager::updatePositiveInteractions(Ipv6Address peer){
        this->neighPositiveInteractions[peer]++;
    }

    double trustManager::getGalenaTrust(Ipv6Address peer, double distance, double similarity, double timedelta, vector<double> rec){
        // trust = alpha*distance + beta*similarity + gamma*SOR
        // SOR = delta* (micra/pi)*e^{-lambda*(timedelta) + (1 - delta)*recomendations
        double trust, sor, direct, indirect;
        double alpha, beta, gamma, delta, micra, pi, lambda;
        const double eulerNumber = exp(1.0);

        alpha = stod(getenv("GALENA_ALPHA"), nullptr);
        beta = stod(getenv("GALENA_BETA"), nullptr);
        gamma = stod(getenv("GALENA_GAMMA"), nullptr);
        delta = stod(getenv("GALENA_DELTA"), nullptr);
        lambda = stod(getenv("GALENA_LAMBDA"), nullptr); //decay fator

        micra = (double)this->getNeighInteractions(peer);
        pi = (double) this->getPositiveNeighInteractions(peer);


        direct = (micra/pi)*pow(eulerNumber, -lambda*timedelta);
        if(std::isnan(direct)){
            direct = 0.0;
        }

        indirect = accumulate(rec.begin(), rec.end(), 0.0) / rec.size();
        if(std::isnan(indirect)){
            indirect = 0.0;
        }

        sor = delta*direct+(1-delta)*indirect;
        trust = alpha*distance + beta*similarity + gamma*sor;

        this->neighTrust[peer] = trust;
        this->neighDirect[peer] = direct;
        this->neighIndirect[peer] = indirect;
        this->neighTotalInteractions[peer]++;

        std::ofstream fout;
        stringstream ss;
        ss << "trustfile-" << myaddr << ".txt";
        fout.open(ss.str(), std::ios_base::app);
        fout << Simulator::Now ().GetSeconds () << "\t"
                << myaddr << "\t"
                << peer << "\t"
                << direct << "\t"
                << indirect << "\t"
                << sor << "\t"
                << distance << "\t"
                << similarity << "\t"
                << trust
                ;
        fout << "\n";
        fout.close();

        return trust;
    }

    void trustManager::clear(){
        this->neighTrust.clear();
        this->neighDirect.clear();
        this->neighIndirect.clear();
        this->neighPositiveInteractions.clear();
        this->neighTotalInteractions.clear();

    }
}