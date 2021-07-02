#pragma once

namespace galena{
    enum MessageTypes{Beacon, ServiceInquiry, ServiceAnswer, ServiceRequest, ServiceExchange, TrustRequest, TrustAnswer};
    enum AuthenticationMechanisms{ECC, RSA, SIM, NOPASS};
    enum AttackType{NoAttack = 0, SelfConstant = 1, SelfOnOff = 2, SybilConstant = 3, SybilOnOff = 4};
}