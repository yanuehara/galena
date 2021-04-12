#pragma once

namespace galena{
    enum MessageTypes{Beacon, ServiceInquiry, ServiceAnswer, ServiceRequest, ServiceExchange, TrustRequest, TrustAnswer};
    enum AuthenticationMechanisms{ECC, RSA, PUF, NOPASS};
}