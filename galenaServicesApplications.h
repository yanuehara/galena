#pragma once

#include <vector>
#include <string>
#include <map>

using namespace std;

namespace galena{
    enum galenaServices{
        Location = 1, Datetime, PoiEvent, PeoplePresense,
        Environment, Weather, EnergyConsumption, TrafficStatus,
        StreetLight, Movement, BusPeopleCounter, GarbageTruckStatus,
        MedicalData, IndoorData, GarbageTruckMovement
    };
    
    class galenaServicesApplication{
        public:
            vector<galenaServices> requiredServices;
            string description;
            string name;

            galenaServicesApplication(string name, vector<galenaServices> reqSvc,string desc){
                this->name = name;
                this->requiredServices = reqSvc;
                this->description = desc;
            }
    };

    static vector<galenaServicesApplication> galenaApplications{
        galenaServicesApplication{ "NoService", 
            vector<galenaServices>{               
            }, "Empty service"
        },
        galenaServicesApplication{ "CrashDetect", 
            vector<galenaServices>{
                static_cast<galenaServices>(5), static_cast<galenaServices>(8), static_cast<galenaServices>(10)
            }, "Evaluate car crash with environment and traffic sensors"
        },
        galenaServicesApplication{ "StreetLight", 
            vector<galenaServices>{
                static_cast<galenaServices>(4), static_cast<galenaServices>(8), static_cast<galenaServices>(9)
            }, "Street light control based on bright intensity and presence of objects"
        },
        galenaServicesApplication{ "CityConsumption", 
            vector<galenaServices>{
                static_cast<galenaServices>(7)
            }, "Energy consumption of public objects in the Smart City"
        },
        galenaServicesApplication{ "Traffic", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(5), static_cast<galenaServices>(8), static_cast<galenaServices>(10)
            }, "Traffic information using presence and sound sensors, and other information obtained by cars"
        },
        galenaServicesApplication{ "Bus", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(2), static_cast<galenaServices>(10), static_cast<galenaServices>(11)
            }, "Information regarding public transport (such as occupancy, frequency, timeliness) in real time"
        },
        galenaServicesApplication{ "Temperature", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(5)
            }, "Temperature in a selected location"
        },
        galenaServicesApplication{ "HomeConsumption", 
            vector<galenaServices>{
                static_cast<galenaServices>(16)
            }, "Users' home energy consumption analysis using home sensors"
        },
        galenaServicesApplication{ "Event", 
            vector<galenaServices>{
                static_cast<galenaServices>(3)
            }, "Information about events in Point of Interests"
        },
        galenaServicesApplication{ "Parking", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(13)
            }, "Search for available parking slots near the user"
        },
        galenaServicesApplication{ "Crowd", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(5), static_cast<galenaServices>(6)
            }, "Using sound, air and weather sensors, evaluate crowd and status of the location"
        },
        galenaServicesApplication{ "DriveMonitoring", 
            vector<galenaServices>{
                static_cast<galenaServices>(5), static_cast<galenaServices>(6), static_cast<galenaServices>(8), static_cast<galenaServices>(9), static_cast<galenaServices>(10)
            }, "Monitor the driver style considering the road, the weather and the car"
        },
        galenaServicesApplication{ "FireDetect", 
            vector<galenaServices>{
                static_cast<galenaServices>(4), static_cast<galenaServices>(5)
            }, "Fire detector using air and other environment sensors"
        },
        //------------------------------------------------------------------------
        galenaServicesApplication{ "Irrigation", 
            vector<galenaServices>{
                static_cast<galenaServices>(5), static_cast<galenaServices>(6)
            }, "Irrigation park controller considering environment and weather information"
        },
        galenaServicesApplication{ "Panel", 
            vector<galenaServices>{
                static_cast<galenaServices>(3), static_cast<galenaServices>(8), static_cast<galenaServices>(13)
            }, "Data visualization in panels around the city"
        },
        galenaServicesApplication{ "PollutionDetect", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(5)
            }, "Pollution detector using environment data in  order to reduce air contamination"
        },
        galenaServicesApplication{ "Health", 
            vector<galenaServices>{
                static_cast<galenaServices>(14)
            }, "Health analysis with smartphone and/or smart-fitness data"
        },
        galenaServicesApplication{ "ApplianceOptimize", 
            vector<galenaServices>{
                static_cast<galenaServices>(7), static_cast<galenaServices>(15)
            }, "Appliances optimisation using home sensors"
        },
        galenaServicesApplication{ "Authorization", 
            vector<galenaServices>{
                static_cast<galenaServices>(4)
            }, "Security system in certain locations, such as Point of Interests"
        },
        galenaServicesApplication{ "EletricCarCharge", 
            vector<galenaServices>{
                static_cast<galenaServices>(5), static_cast<galenaServices>(6), static_cast<galenaServices>(7)
            }, "Optimizer for charging the electrical cars"
        },
        galenaServicesApplication{ "Cinema", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(2), static_cast<galenaServices>(3), static_cast<galenaServices>(4)
            }, "Film and crowd information for cinemas"
        },
        galenaServicesApplication{ "IntrusionDetection", 
            vector<galenaServices>{
                static_cast<galenaServices>(4), static_cast<galenaServices>(15)
            }, "Intrusion detection system in a users' home"
        },
        galenaServicesApplication{ "Alarm", 
            vector<galenaServices>{
                static_cast<galenaServices>(4), static_cast<galenaServices>(5)
            }, "General danger detector and warning to the interested population"
        },
        galenaServicesApplication{ "WasteStatus", 
            vector<galenaServices>{
                static_cast<galenaServices>(12), static_cast<galenaServices>(16)
            }, "Garbage trucks status to optimise routes"
        },
        galenaServicesApplication{ "SmartphoneDetection", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(4)
            }, "General danger detector and warning to the interested population"
        },
        galenaServicesApplication{ "ChildrenExplorer", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(2)
            }, "Children monitoring"
        },
        galenaServicesApplication{ "Weather", 
            vector<galenaServices>{
                static_cast<galenaServices>(1), static_cast<galenaServices>(2), static_cast<galenaServices>(5), static_cast<galenaServices>(6)
            }, "Weather information around the city"
        }
    };
}