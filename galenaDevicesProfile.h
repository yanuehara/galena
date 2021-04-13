#pragma once

#include "galenaServicesApplications.h"

using namespace std;

namespace galena{
    class galenaProfile{
        public:
            vector<galenaServices> servicesOffered;
            vector<int> applicationsRequested;

            galenaProfile(vector<galenaServices> svcOff, vector<int> appReq){
                this->servicesOffered = svcOff;
                this->applicationsRequested = appReq;
            }
    };

    static vector<galenaProfile> galenaProfiles{
        galenaProfile{
            vector<galenaServices>{},
            vector<int>{}
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(4),
                static_cast<galenaServices>(5),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(10)
            },
            vector<int>{
                1,4,5,6,8,9,10,11,16,17,21,22,23,24,25,26
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(4),
                static_cast<galenaServices>(5),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(8),
                static_cast<galenaServices>(9),
                static_cast<galenaServices>(10)
            },
            vector<int>{
                1, 4, 9, 11, 20, 28
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(4),
                static_cast<galenaServices>(5),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(10)
            },
            vector<int>{
                1, 4, 5, 6, 8, 9, 10, 11, 16, 17, 18, 21, 22, 23, 24, 25, 26
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(4)
            },
            vector<int>{
                6
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(4),
                static_cast<galenaServices>(7)
            },
            vector<int>{
                5, 6, 8, 10, 11, 16, 17, 18, 21, 23, 24, 25
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(4),
                static_cast<galenaServices>(7)
            },
            vector<int>{
                16, 17, 18, 22, 24, 26
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(16)
            },
            vector<int>{
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(5),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(17)
            },
            vector<int>{
                7, 17, 22
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(3),
                static_cast<galenaServices>(4)
            },
            vector<int>{
                3, 7, 13, 15, 19, 28
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(5),
                static_cast<galenaServices>(6),
                static_cast<galenaServices>(7)
            },
            vector<int>{
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(6),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(8),
                static_cast<galenaServices>(9),
                static_cast<galenaServices>(10),
                static_cast<galenaServices>(11),
                static_cast<galenaServices>(12)
            },
            vector<int>{
                1, 4, 5, 11
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(3),
                static_cast<galenaServices>(5),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(8)
            },
            vector<int>{
                1, 3, 4, 5, 7, 8, 12, 14, 15
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(13),
                static_cast<galenaServices>(18)
            },
            vector<int>{
                1, 4, 6, 11
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(9)
            },
            vector<int>{
                2
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(14)
            },
            vector<int>{
            }
        },
        galenaProfile{
            vector<galenaServices>{
                static_cast<galenaServices>(1),
                static_cast<galenaServices>(2),
                static_cast<galenaServices>(4),
                static_cast<galenaServices>(7),
                static_cast<galenaServices>(8)
            },
            vector<int>{
                1, 4
            }
        }
    };

}


