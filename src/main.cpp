#include <WiFi.h>
#include <Web3.h>
#include <Util.h>
#include <Contract.h>
#include <chainIds.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20PIN 18

/* Create an instance of OneWire */
OneWire oneWire(DS18B20PIN);

DallasTemperature sensor(&oneWire);



const char *ssid = "POCO X3";
const char *password = "amal123#";
#define MY_ADDRESS "0xC4BD0366229f53481677e051aBee3070c5a183Fc" // Wallet Address
#define CONTRACT "0xAC368aCf7A5379793D84466E5D34087dF4993A41" // Contract Address

const char *PRIVATE_KEY = "082150a573d30ae168107230d966b59f4e1fa63071977ea9897bd968f1a1702e"; // Private key

int wificounter = 0;
Web3 *web3;

void setup_wifi();
void PushTransaction();

void setup()
{
  Serial.begin(115200);
  sensor.begin();
  setup_wifi();
  web3 = new Web3(ARBITRUM_SEPOLIA_ID); //Network
  string address = string(MY_ADDRESS);
}

void loop()
{
  sensor.requestTemperatures();
  float tempinC=sensor.getTempCByIndex(0);
  Serial.print("Temperature = ");
  Serial.print(tempinC);
  Serial.println("C");
  delay(1000);
  if (tempinC>=32)
  {
    PushTransaction();
  }
}


void PushTransaction()
{
  string contractAddr = "0xAC368aCf7A5379793D84466E5D34087dF4993A41";
  Contract contract(web3, contractAddr.c_str());
  contract.SetPrivateKey(PRIVATE_KEY);

  unsigned long long gasPriceVal = 22000000000ULL;
  uint32_t gasLimitVal = 4300000;

  // tampered transit ID
  uint256_t transit_ID = 10000;

  // get nonce
  string addr = string(MY_ADDRESS);
  uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(&addr);
  uint256_t valueStr = 0;

  // Setup contract function call
  string p = string(contract.SetupContractData("tempTampered(uint256)", &transit_ID)); // contract function with its params

  // push transaction to ethereum
  string result = contract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &contractAddr, &valueStr, &p);
  string transactionHash = web3->getString(&result);
  Serial.println(result.c_str());
}

void setup_wifi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return;
  }

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, password);
  }

  wificounter = 0;
  while (WiFi.status() != WL_CONNECTED && wificounter < 10)
  {
    for (int i = 0; i < 500; i++)
    {
      delay(1);
    }
    Serial.print(".");
    wificounter++;
  }

  if (wificounter >= 10)
  {
    Serial.println("Restarting ...");
    ESP.restart(); 
  }

  delay(10);

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}