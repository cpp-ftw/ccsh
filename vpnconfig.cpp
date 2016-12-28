#include <ccsh/ccsh.hpp>
#include <string>
#include <iostream>

static const std::string cert_template = R"delim(
client
dev tun
proto tcp
remote 79.172.212.234 443
resolv-retry infinite
nobind
user nobody
group nobody
persist-key
persist-tun
remote-cert-tls server
comp-lzo
verb 3

<ca>
-----BEGIN CERTIFICATE-----
MIIE0DCCA7igAwIBAgIJAITQac7yU8kwMA0GCSqGSIb3DQEBCwUAMIGgMQswCQYD
VQQGEwJIVTENMAsGA1UECBMEUEVTVDERMA8GA1UEBxMIQnVkYXBlc3QxEDAOBgNV
BAoTB0NQUCBGVFcxFDASBgNVBAsTC0NQUCBGVFcgVlBTMRMwEQYDVQQDEwpDUFAg
RlRXIENBMQ8wDQYDVQQpEwZzZXJ2ZXIxITAfBgkqhkiG9w0BCQEWEmhqYW5vczk1
QGdtYWlsLmNvbTAeFw0xNjA3MTEyMTM5NTJaFw0yNjA3MDkyMTM5NTJaMIGgMQsw
CQYDVQQGEwJIVTENMAsGA1UECBMEUEVTVDERMA8GA1UEBxMIQnVkYXBlc3QxEDAO
BgNVBAoTB0NQUCBGVFcxFDASBgNVBAsTC0NQUCBGVFcgVlBTMRMwEQYDVQQDEwpD
UFAgRlRXIENBMQ8wDQYDVQQpEwZzZXJ2ZXIxITAfBgkqhkiG9w0BCQEWEmhqYW5v
czk1QGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOMe
EpbjYr4ulur2WEdFxHPwfeLCxjz80AxJeQcgW/L1bkJkbQaQSYZFfEQxHh/PmGa6
bfxiDqUfuq5Jbdf+pVNRqYlKL7pv3uKxYf+VlNKZBydWB2eajA9l5hOqQxnZunG+
Pmve3aPAPUWdHys/QgWfpKpKKRqoy2XQ4yu0id/x8H81BRH70T6OIWPcFrHSHkiA
SjYGpj7sz3y8XeTY/+x8H6CcQ0fLTOvSXIZik2i9gxR4V71PrLxgY2pGfLsIY6CZ
UKfKsApXzaKPwSZhoeSEB++UVd7RU/gluXug61XVvSUJ/Md91C1TiEo68GWARlKp
iltP8oieaPH2YqNwCksCAwEAAaOCAQkwggEFMB0GA1UdDgQWBBR/jfsGqWMS+zjQ
yVz1QjPEYvAhNDCB1QYDVR0jBIHNMIHKgBR/jfsGqWMS+zjQyVz1QjPEYvAhNKGB
pqSBozCBoDELMAkGA1UEBhMCSFUxDTALBgNVBAgTBFBFU1QxETAPBgNVBAcTCEJ1
ZGFwZXN0MRAwDgYDVQQKEwdDUFAgRlRXMRQwEgYDVQQLEwtDUFAgRlRXIFZQUzET
MBEGA1UEAxMKQ1BQIEZUVyBDQTEPMA0GA1UEKRMGc2VydmVyMSEwHwYJKoZIhvcN
AQkBFhJoamFub3M5NUBnbWFpbC5jb22CCQCE0GnO8lPJMDAMBgNVHRMEBTADAQH/
MA0GCSqGSIb3DQEBCwUAA4IBAQApcvkRfa/Vo6i5uKe3xQrtPVAlFPOZ1U6DHEIz
1iEOM1/hnVElBhLmlgcFeto290Zo89XF3D27Be6vJBLN+X8eNaCyBv87bOpc4oom
skldoVVVtHrJTEM3YdfzgpAdnMZ6ZST4jaPB+Fjgr98laIIQbaEzQtR+Vac28Xkb
PweO1v2btlxpAE3ojX56tV5XLu0dqr1Esg+o+YCQL9W2m4fwBsvGYpSUmpBdaQqQ
HtqJ0gwRnrvDdikfJDmNqAb9alU2zTH+TWore84Pn7EePabSUQ2b0eRCgxQrPI1d
9TFhXhNDQ6jy28lPaMg5oSjjaKsHeGBYWwYAUAhkQoM4IdIf
-----END CERTIFICATE-----
</ca>

<cert>
[[[CERT]]]
</cert>

<key>
[[[KEY]]]
</key>
)delim";

bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void print_usage()
{
    std::cerr << "Usage: vpnconfig <config-name> <config-email>" << std::endl;
}

int main(int argc, char ** argv)
{
    using namespace ccsh::literals;

    // input validation:
    //   - first arg is config name
    //   - second arg is config email

    ccsh::fs::path our_name = argv[0];
    std::string name_str = our_name.filename().string();

    if(argc != 3)
    {
        print_usage(name_str);
        exit(1);
    }

    std::string config_name = argv[1];
    std::string config_email = argv[2];

    if(config_name.find('@') != std::string::npos ||
        config_email.find('@') == std::string::npos)
    {
        print_usage(name_str);
        exit(1);
    }

    ccsh::source("/etc/openvpn/easy-rsa/vars"_p).run();
    ccsh::dollar("KEY_NAME") = config_name;
    ccsh::dollar("KEY_EMAIL") = config_email;
    ccsh::shell("/etc/openvpn/easy-rsa/pkitool", {"--batch", config_name}).run();

    // cert -> keys/dobragab-test.crt
    std::string cert_content;
    (ccsh::shell("cat", {"/etc/openvpn/easy-rsa/keys/" + config_name + ".crt"}) > cert_content).run();

    // key  -> keys/dobragab-test.key
    std::string key_content;
    (ccsh::shell("cat", {"/etc/openvpn/easy-rsa/keys/" + config_name + ".key"}) > key_content).run();

    std::string new_key = cert_template;
    replace(new_key, "[[[CERT]]]", cert_content);
    replace(new_key, "[[[KEY]]]",  key_content);

    (ccsh::shell("cat") < new_key > (config_name + ".ovpn")).run();
}


