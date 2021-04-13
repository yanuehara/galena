#include "ns3/core-module.h"
#include "ns3/node-container.h"
#include "ns3/lr-wpan-net-device.h"
#include "ns3/lr-wpan-helper.h"
#include "ns3/lr-wpan-spectrum-value-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/sixlowpan-helper.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/spectrum-value.h"

#include <iostream>
#include <fstream>

#include "galenaDevicesProfile.h"
#include "galenaApplication.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("GALENA_V1");

int main(int argc, char *argv[])
{
    NS_LOG_UNCOND ("GALENA_V1");
    LogComponentEnable("GALENA_V1", LOG_LEVEL_ALL);

    uint32_t nNodes = 16216;
    std::string traceFile;
    int64_t duration = 2591810;

	CommandLine cmd;
	cmd.AddValue ("nNodes", "Number of node devices", nNodes);
    cmd.AddValue ("duration", "duration", traceFile);
    cmd.AddValue ("traceFile", "Ns2 movement trace file", traceFile);
	cmd.Parse (argc,argv);

	NS_LOG_INFO("Creating " << nNodes << " nodes");
	NodeContainer nodes;
	nodes.Create(nNodes);

	NS_LOG_INFO("Creating internet stack");
	InternetStackHelper internetv6;
	internetv6.SetIpv4StackInstall(false);
	internetv6.SetIpv6StackInstall(true);
	internetv6.Install(nodes);

    NS_LOG_INFO ("Create channels");
	LrWpanHelper lrwpan(false);
	NetDeviceContainer netdevices = lrwpan.Install(nodes);
	lrwpan.AssociateToPan(netdevices, 0);
	//lrwpan.EnablePcapAll("galena-");

    NS_LOG_INFO("Creating sixlowpan");
	SixLowPanHelper sixlowpan;
   	//sixlowpan.SetDeviceAttribute("ForceEtherType", BooleanValue (true) );
   	NetDeviceContainer six1 = sixlowpan.Install(netdevices);

	NS_LOG_INFO ("Create networks and assign IPv6 Addresses");
   	Ipv6AddressHelper ipv6;
  	ipv6.SetBase (Ipv6Address ("2020:1::"), Ipv6Prefix (64));
  	Ipv6InterfaceContainer i1 = ipv6.Assign(six1);

    NS_LOG_INFO("Setting up mobility");
    Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
    ns2.Install();

	NS_LOG_INFO("Creating default policies");
	std::map<Ipv6Address, int>* nodeAddrs = new std::map<Ipv6Address, int>;
	galena::policy pol1;
	pol1.trust = 0.5;
	pol1.ids = std::set<Ipv6Address>{Ipv6Address{"FF02::1"}};
	pol1.auth = std::set<galena::AuthenticationMechanisms>{ galena::AuthenticationMechanisms::ECC };
	pol1.trustCompare = galena::policyTrustComparator::GT;
	pol1.contexts = std::set<string>{"any"};
	pol1.final = "ECC_1";

	NS_LOG_INFO("Creating galena application");
	for (size_t i = 0; i < nodes.GetN(); i++){
		Ptr<galena::GalenaApplication> nodeApplication = Create<galena::GalenaApplication>();
		nodeApplication->SetStartTime(Seconds(0.0));
		nodeApplication->SetStopTime(Seconds(duration));
		nodes.Get(i)->AddApplication(nodeApplication);

		nodeApplication->setup(1, 0, 0);

		nodeApplication->polManager->addPolicy(pol1);
		nodeApplication->nodemap = nodeAddrs;
		nodeAddrs->insert(make_pair(nodeApplication->GetNodeIpAddress(), i));

		Ptr<LrWpanNetDevice> nodenetdev = DynamicCast<LrWpanNetDevice>(nodes.Get(i)->GetDevice(1));
		auto phy = nodenetdev->GetPhy();
		LrWpanSpectrumValueHelper svh;
		auto psd = svh.CreateTxPowerSpectralDensity (-10, 11); //Range of 50m according to lr-wpan-error-distance-plot
		phy->SetTxPowerSpectralDensity(psd);
	}

    NS_LOG_INFO("Starting Simulation");	
	Simulator::Stop( Seconds(duration) );
	Simulator::Run();
	
	Simulator::Destroy();

	NS_LOG_INFO("Simulation end");
    return 0;
}
