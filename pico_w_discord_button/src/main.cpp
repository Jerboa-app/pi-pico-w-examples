#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"

#include <string>
#include <sstream>
#include <iomanip>

#define TLS_CLIENT_SERVER "discord.com"

#define TLS_CLIENT_TIMEOUT_SECS  10

// This is the PUBLIC root certificate from discord.com
#define TLS_ROOT_CERT_OK "-----BEGIN CERTIFICATE-----\n\
MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX\n\
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\n\
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx\n\
NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\n\
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0\n\
MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube\n\
Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e\n\
WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd\n\
BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd\n\
BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN\n\
l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw\n\
Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v\n\
Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG\n\
SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ\n\
odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY\n\
+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs\n\
kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep\n\
8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1\n\
vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl\n\
-----END CERTIFICATE-----"

extern "C"
{
    bool run_tls_client(const uint8_t *cert, size_t cert_len, const char *server, const char *request, int timeout);
}

void blink()
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

void post(std::string message)
{
    std::string header = "POST /api/webhooks/" WEBHOOK " HTTP/1.1\r\n" \
    "Host: " TLS_CLIENT_SERVER "\r\n" \
    "user-agent: " USER_AGENT "\r\n" \
    "accept: application/json\r\n" \
    "connection: close\r\n" \
    "content-type:application/json\r\n";

    std::string body = "{\"content\": \"" + message + "\"}";
    uint64_t length = body.size();

    const uint8_t cert_ok[] = TLS_ROOT_CERT_OK;
    std::string request = header + "content-length: " + std::to_string(length) + "\r\n\r\n" + body + "\r\n";
    printf("SENDING\n\n%s\n\n", request.c_str());
    run_tls_client(cert_ok, sizeof(cert_ok), TLS_CLIENT_SERVER, request.c_str(), TLS_CLIENT_TIMEOUT_SECS);
}

int main()
{
    stdio_init_all();
    gpio_init(18);
    gpio_set_dir(18, GPIO_IN);

    if (cyw43_arch_init())
    {
        printf("cyw43_arch_init failure\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("Wifi connection failure\n");
        blink(); blink();
        sleep_ms(1000);
        return 1;
    }
    blink();

    while(true)
    {
        bool value = gpio_get(18);
        printf("%s\n", value ? "high" : "low");

        if (!value) { post("Hello from Pi Pico W!"); }
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}

