
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
#include "ns3/wifi-module.h"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

#include "galenaDevicesProfile.h"
#include "galenaApplication.h"
#include "galenaConstants.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("GALENA_V1");

int main(int argc, char *argv[])
{
    NS_LOG_UNCOND ("GALENA_V1");
    LogComponentEnable("GALENA_V1", LOG_LEVEL_ALL);
	LogComponentEnable("GALENA_Application", LOG_LEVEL_ALL);

    uint32_t nNodes = 16216;
    std::string traceFile;
    int64_t duration = 2591810;

	CommandLine cmd;
	cmd.AddValue ("nNodes", "Number of node devices", nNodes);
    cmd.AddValue ("duration", "duration", duration);
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

	WifiHelper wifi;
	wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
			                      StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold",
								  UintegerValue (0));

	// MAC Layer non QoS

	WifiMacHelper wifiMac;
	wifiMac.SetType ("ns3::AdhocWifiMac");

	// PHY layer
	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
	wifiPhy.SetChannel (wifiChannel.Create ());

	// Creating and installing netdevices in all nodes
	NetDeviceContainer devices;
	devices = wifi.Install (wifiPhy, wifiMac, nodes);

	NS_LOG_INFO ("Create networks and assign IPv6 Addresses");
   	Ipv6AddressHelper ipv6;
  	ipv6.SetBase (Ipv6Address ("2020:1::"), Ipv6Prefix (64));
  	Ipv6InterfaceContainer i1 = ipv6.Assign(devices);

	NS_LOG_INFO("Creating default policies");
	std::map<Ipv6Address, int>* nodeAddrs = new std::map<Ipv6Address, int>;
	
	galena::policy pol1;
	pol1.trust = 0.5;
	pol1.upperbound = 0.9;
	pol1.ids = std::set<Ipv6Address>{Ipv6Address{"FF02::1"}};
	pol1.auth = std::set<galena::AuthenticationMechanisms>{galena::AuthenticationMechanisms::ECC,
                                                    galena::AuthenticationMechanisms::RSA,
                                                    galena::AuthenticationMechanisms::SIM,
                                                    galena::AuthenticationMechanisms::NOPASS };
	pol1.trustCompare = galena::policyTrustComparator::GT;
	pol1.contexts = std::set<string>{"any"};
	pol1.final = "ECC_1___";

	galena::policy pol2;
	pol2.trust = 0.0;
	pol2.upperbound = 0.5;
	pol2.ids = std::set<Ipv6Address>{Ipv6Address{"FF02::1"}};
	pol2.auth = std::set<galena::AuthenticationMechanisms>{ galena::AuthenticationMechanisms::RSA,
                                                    galena::AuthenticationMechanisms::SIM,
                                                    galena::AuthenticationMechanisms::NOPASS  };
	pol2.trustCompare = galena::policyTrustComparator::GT;
	pol2.contexts = std::set<string>{"any"};
	pol2.final = "RSA_1___";

	galena::policy pol3;
	pol3.trust = 0.9;
	pol3.upperbound = 1.0;
	pol3.ids = std::set<Ipv6Address>{Ipv6Address{"FF02::1"}};
	pol3.auth = std::set<galena::AuthenticationMechanisms>{ galena::AuthenticationMechanisms::NOPASS };
	pol3.trustCompare = galena::policyTrustComparator::GT;
	pol3.contexts = std::set<string>{"home"};
	pol3.final = "NOPASS_1";


	ifstream capFile("capabilities.txt");
	std::string line;

	NS_LOG_INFO("Creating galena application");
	for (size_t i = 0; i < nodes.GetN() && getline(capFile, line); i++){
		NS_LOG_INFO("Setting node " << i << " with " << line);
		Ptr<galena::GalenaApplication> nodeApplication = Create<galena::GalenaApplication>();
		nodeApplication->SetStartTime(Seconds(0.0));
		nodeApplication->SetStopTime(Seconds(duration));
		nodes.Get(i)->AddApplication(nodeApplication);

		vector<std::string> strs;
		//boost::algorithm::split_regex(strs, line, boost::regex("\t"));
		boost::split(strs,line,boost::is_any_of("\t"));
		int profileIndex = std::strtol(strs[1].c_str(), nullptr, 10);
		double xHome = std::strtod(strs[2].c_str(), nullptr);
		double yHome = std::strtod(strs[3].c_str(), nullptr);

		auto addr = nodeApplication->GetNodeIpAddress();
		nodeAddrs->insert(make_pair(addr, i));
		nodeApplication->nodemap = nodeAddrs;
		nodeApplication->setup(profileIndex, xHome, yHome);
		nodeApplication->polManager->addPolicy(pol1);
		nodeApplication->polManager->addPolicy(pol2);
		nodeApplication->polManager->addPolicy(pol3);
		nodeApplication->tManager->myaddr = addr;

		/*Ptr<LrWpanNetDevice> nodenetdev = DynamicCast<LrWpanNetDevice>(nodes.Get(i)->GetDevice(1));
		auto phy = nodenetdev->GetPhy();
		LrWpanSpectrumValueHelper svh;
		auto psd = svh.CreateTxPowerSpectralDensity (-10, 11); //Range of 50m according to lr-wpan-error-distance-plot
		phy->SetTxPowerSpectralDensity(psd);*/
	}

    NS_LOG_INFO("Setting up mobility");
    Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
    ns2.Install();

    NS_LOG_INFO("Starting Simulation");	
	Simulator::Stop( Seconds(duration) );
	Simulator::Run();
	
	Simulator::Destroy();

	NS_LOG_INFO("Simulation end");
    return 0;
}
