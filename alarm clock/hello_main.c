//되는 코드
#include "wifi.h"
#include <tinyara/gpio.h>
#include <apps/netutils/mqtt_api.h>
#include <apps/netutils/dhcpc.h>

#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>

#include <apps/shell/tash.h>
#include <stdlib.h>

// for NTP
#include <apps/netutils/ntpclient.h>
#include "sh1106.h"
#include "numbers.h"
#include "alphabet.h"

//for buzzer
#include "A053BasicKit.h"

// for JSON
#include <apps/netutils/cJSON.h>

#define DEFAULT_CLIENT_ID "123456789"
#define SERVER_ADDR "api.artik.cloud"
//#define SERVER_ADDR "52.86.204.150"
#define SERVER_PORT 8883
//#define SERVER_PORT 1883 // non-secure mode, Not supported in ARTIK Cloud
#define NET_DEVNAME "wl1"
#define RED_ON_BOARD_LED 45

char device_id[] =      "156120b00e304d219a31b0c79d6a1760";
char device_token[] = "fb77ed7fa6fb44fca815d5b0722ad51a";

char *strTopicMsg;
char *strTopicAct;

int hour = 25;
int minute = 0;
bool inhome = true;
bool fire = false;


//////for buzzer///////////////////
//int fd;
//int pin_buzzer=0;
//int max = 1;
//char *pwm0 = "0";
//char *pwm1 = "1";
//int dutycycle=100;
//////////////////////////////////


static const char mqtt_ca_cert_str[] = \
      "-----BEGIN CERTIFICATE-----\r\n"
      "MIIGrTCCBZWgAwIBAgIQASAP9e8Tbenonqd/EQFJaDANBgkqhkiG9w0BAQsFADBN\r\n"
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMScwJQYDVQQDEx5E\r\n"
      "aWdpQ2VydCBTSEEyIFNlY3VyZSBTZXJ2ZXIgQ0EwHhcNMTgwMzA4MDAwMDAwWhcN\r\n"
      "MjAwNDA1MTIwMDAwWjBzMQswCQYDVQQGEwJVUzETMBEGA1UECBMKQ2FsaWZvcm5p\r\n"
      "YTERMA8GA1UEBxMIU2FuIEpvc2UxJDAiBgNVBAoTG1NhbXN1bmcgU2VtaWNvbmR1\r\n"
      "Y3RvciwgSW5jLjEWMBQGA1UEAwwNKi5hcnRpay5jbG91ZDCCASIwDQYJKoZIhvcN\r\n"
      "AQEBBQADggEPADCCAQoCggEBANghNaTXWDfYV/JWgBnX4hmhcClPSO0onx5B2url\r\n"
      "YzpvTc3MBaQ+08YBpAKvTqZvPqrJUIM45Q91M301I5e2kz0DMq2zQZOGB0B83V/O\r\n"
      "O4vwETq4PCjAPhMinF4dN6HeJCuqo1CLh8evhfkFiJvpEfQWTxdjzPJ0Zdj/2U8E\r\n"
      "8Ht7zV5pWiDtuejtIDHB5H6fCx4xeQy/E+5l4V6R3BnRKpZsJtlhTh0RFqWhw5DJ\r\n"
      "/WWpGP//1VTZSHyW9SABsPd+jP1YgDraRD4b4lZBU6c8nC5qT3dhdiYoG6xUgTb3\r\n"
      "kfgUhhlOFpe3sBtR32OS8RuFrFeQDGaa3r6pfSy06Kph/eECAwEAAaOCA2EwggNd\r\n"
      "MB8GA1UdIwQYMBaAFA+AYRyCMWHVLyjnjUY4tCzhxtniMB0GA1UdDgQWBBSNBf6r\r\n"
      "7S/j0oV3A0XmEflXErutQDAlBgNVHREEHjAcgg0qLmFydGlrLmNsb3VkggthcnRp\r\n"
      "ay5jbG91ZDAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsG\r\n"
      "AQUFBwMCMGsGA1UdHwRkMGIwL6AtoCuGKWh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNv\r\n"
      "bS9zc2NhLXNoYTItZzYuY3JsMC+gLaArhilodHRwOi8vY3JsNC5kaWdpY2VydC5j\r\n"
      "b20vc3NjYS1zaGEyLWc2LmNybDBMBgNVHSAERTBDMDcGCWCGSAGG/WwBATAqMCgG\r\n"
      "CCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BTMAgGBmeBDAEC\r\n"
      "AjB8BggrBgEFBQcBAQRwMG4wJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2lj\r\n"
      "ZXJ0LmNvbTBGBggrBgEFBQcwAoY6aHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29t\r\n"
      "L0RpZ2lDZXJ0U0hBMlNlY3VyZVNlcnZlckNBLmNydDAJBgNVHRMEAjAAMIIBfwYK\r\n"
      "KwYBBAHWeQIEAgSCAW8EggFrAWkAdgCkuQmQtBhYFIe7E6LMZ3AKPDWYBPkb37jj\r\n"
      "d80OyA3cEAAAAWIHFb1dAAAEAwBHMEUCIQCQ0UjVVJSQDRB3oxzI5aD1Hs5GhbXj\r\n"
      "I6Cqt3/tkXT1WQIgNVWRgbJ72Ik9gp5QoNxhCZ+h//or0uL7PHnv3cP5L9UAdgBv\r\n"
      "U3asMfAxGdiZAKRRFf93FRwR2QLBACkGjbIImjfZEwAAAWIHFb73AAAEAwBHMEUC\r\n"
      "IQDxCxJCsZjuqbQvuwipgdUf1l6qXdiekM5zn33i1+KYxgIgKDMJEuKHzhkweT2S\r\n"
      "Y4dWBuzSdOAzZfoDrIGdsFvkxi0AdwC72d+8H4pxtZOUI5eqkntHOFeVCqtS6BqQ\r\n"
      "lmQ2jh7RhQAAAWIHFb1YAAAEAwBIMEYCIQCNDYdxWmqUGGwNzXlJ1/NXxzwqPYIB\r\n"
      "eSJDuR1xfWtSsQIhAJsygf2rqPS+O7qQAzggCQ2V/3JDRUhuxNDPqwooo47uMA0G\r\n"
      "CSqGSIb3DQEBCwUAA4IBAQBvRGWibvHFrRUWsArJ9lmS5MMZFbXXQPXbflgv3nSG\r\n"
      "ShmhBC3o+k97J0Wgp/wH7uDf01RrRMAVNm458g1Mr4AMAXq3zzxNNTwjGYw/USuG\r\n"
      "UprrKqc9onugtAUX8DGvlZr8SWO3FhPlyamWQ69jutx/X4nfHyZr41bX9WQ/ay0F\r\n"
      "GQJ1tRTrX1eUPO+ucXeG8vTbt09bRNnoY+i97dzrwHakXySfHohNsIbwmrsS4SQv\r\n"
      "7eG9g5+5vsc2B9ugGcELIYKrzDWNPshir37KSpcwLUCmDJkTQp8+KhJUKgbTALTa\r\n"
      "nxuDyNwZIwW66vv1t0Zi4vKU8hfUsAN2N3wcsb6pY/RA\r\n"
      "-----END CERTIFICATE-----\r\n"
      "-----BEGIN CERTIFICATE-----\r\n"
      "MIIElDCCA3ygAwIBAgIQAf2j627KdciIQ4tyS8+8kTANBgkqhkiG9w0BAQsFADBh\r\n"
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
      "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n"
      "QTAeFw0xMzAzMDgxMjAwMDBaFw0yMzAzMDgxMjAwMDBaME0xCzAJBgNVBAYTAlVT\r\n"
      "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJzAlBgNVBAMTHkRpZ2lDZXJ0IFNIQTIg\r\n"
      "U2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n"
      "ANyuWJBNwcQwFZA1W248ghX1LFy949v/cUP6ZCWA1O4Yok3wZtAKc24RmDYXZK83\r\n"
      "nf36QYSvx6+M/hpzTc8zl5CilodTgyu5pnVILR1WN3vaMTIa16yrBvSqXUu3R0bd\r\n"
      "KpPDkC55gIDvEwRqFDu1m5K+wgdlTvza/P96rtxcflUxDOg5B6TXvi/TC2rSsd9f\r\n"
      "/ld0Uzs1gN2ujkSYs58O09rg1/RrKatEp0tYhG2SS4HD2nOLEpdIkARFdRrdNzGX\r\n"
      "kujNVA075ME/OV4uuPNcfhCOhkEAjUVmR7ChZc6gqikJTvOX6+guqw9ypzAO+sf0\r\n"
      "/RR3w6RbKFfCs/mC/bdFWJsCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8C\r\n"
      "AQAwDgYDVR0PAQH/BAQDAgGGMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY\r\n"
      "aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMHsGA1UdHwR0MHIwN6A1oDOGMWh0dHA6\r\n"
      "Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwN6A1\r\n"
      "oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RD\r\n"
      "QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v\r\n"
      "d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHQYDVR0OBBYEFA+AYRyCMWHVLyjnjUY4tCzh\r\n"
      "xtniMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA0GCSqGSIb3DQEB\r\n"
      "CwUAA4IBAQAjPt9L0jFCpbZ+QlwaRMxp0Wi0XUvgBCFsS+JtzLHgl4+mUwnNqipl\r\n"
      "5TlPHoOlblyYoiQm5vuh7ZPHLgLGTUq/sELfeNqzqPlt/yGFUzZgTHbO7Djc1lGA\r\n"
      "8MXW5dRNJ2Srm8c+cftIl7gzbckTB+6WohsYFfZcTEDts8Ls/3HB40f/1LkAtDdC\r\n"
      "2iDJ6m6K7hQGrn2iWZiIqBtvLfTyyRRfJs8sjX7tN8Cp1Tm5gr8ZDOo0rwAhaPit\r\n"
      "c+LJMto4JQtV05od8GiG7S5BNO98pVAdvzr508EIDObtHopYJeS4d60tbvVS3bR0\r\n"
      "j6tJLp07kzQoH3jOlOrHvdPJbRzeXDLz\r\n"
      "-----END CERTIFICATE-----\r\n";
#define TIME_STRING_LEN         16
#define DATE_STRING_LEN         16

#ifndef CONFIG_EXAMPLES_NTPCLIENT_TEST_SERVER_PORT
#define CONFIG_EXAMPLES_NTPCLIENT_TEST_SERVER_PORT   123
#endif

/****************************************************************************
 * Enumeration
 ****************************************************************************/
enum timezone_e {
   TIMEZONE_UTC = 0,
   TIMEZONE_KST,
   TIMEZONE_END
};

/****************************************************************************
 * Structure
 ****************************************************************************/
struct timezone_info_s {
   char str[4];
   int offset;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static char g_time_str[TIME_STRING_LEN + 1];
static char g_date_str[DATE_STRING_LEN + 1];
struct timezone_info_s g_timezone[TIMEZONE_END] = {
      {"UTC", 0},
      {"KST", 9}
};
int currTimeS;      // save second
int currTimeM;      // save minute
int currTimeH;      // save hour

int alarm_sound[64]={O5_SO,O5_MI,O5_MI,O5_MI,O5_MI,O5_MI,O5_SO,O5_MI,O5_DO,O5_MI,O5_SO,O5_MI,O5_DO,O5_MI,O5_SO,O5_MI,O5_MI,O5_DO,O5_DO,O5_DO,O5_DO,O5_MI,O5_MI,O5_MI,O5_MI,O5_SO,O5_MI,O5_DO,O5_MI,O5_SO,O5_MI,O5_DO,O5_MI,O5_SO,O5_MI,O5_MI,O5_DO,O5_DO,O5_DO,O5_DO,O5_SO,O5_SO,O5_MI,O5_DO,O5_MI,O5_DO,O5_MI,O5_DO,O5_MI,O5_SO,O5_DO,O5_DO,O5_MI,O5_MI,O5_SO,O5_SO,O5_MI,O5_DO,O5_MI,O5_DO,O5_MI,O5_DO,O5_MI,O5_SO};
int alarm_bit[64]={4, 4, 2, 2, 4, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 4, 2, 1, 1, 4, 2, 2, 4, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 4, 2, 1, 1, 5, 2, 1, 1, 2, 2, 1, 1, 1, 1, 5, 2, 2, 2, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 5};

// mqtt client handle
mqtt_client_t* pClientHandle = NULL;

// mqtt client parameters
mqtt_client_config_t clientConfig;

mqtt_tls_param_t clientTls;

struct ntpc_server_conn_s g_server_conn[2];
static void dispClock(void);

const unsigned char *get_ca_cert(void) {
   return (const unsigned char*)mqtt_ca_cert_str;
}
struct tm alarm;

// mqtt client on connect callback
void onConnect(void* client, int result) {
   printf("mqtt client connected to the server\n");
}

// mqtt client on disconnect callback
void onDisconnect(void* client, int result) {
   printf("mqtt client disconnected from the server\n");
}

// mqtt client on publish callback
void onPublish(void* client, int result) {
   printf("mqtt client Published message\n");
}

// mqtt client on message callback
// header: <apps/netutils/cJSON.h>
void onMessage(void *client, mqtt_msg_t *msg) {
   int i;
   cJSON *jsonMsg = NULL;
   char *strActName = NULL;
   char *strActName2 = NULL;
   char *strActName3 = NULL;
   char *payload = strdup(msg->payload);

   printf("Received message\n");
   printf("Topic: %s\n", msg->topic);
   printf("Message: %s\n", payload);

   jsonMsg = cJSON_Parse((const char*)payload);
   cJSON *data = cJSON_GetObjectItem(jsonMsg, "actions");

   if (data == NULL) {
      printf("data is null\n");
      return;
   }

   //int dataLength = cJSON_GetArraySize(data);
   //printf("array length: %d\n", dataLength);

   cJSON *action = cJSON_GetArrayItem(data, 0);
   cJSON *actName = cJSON_GetObjectItem(action, "name");
   if(strncmp(cJSON_Print(actName), "\"setValue\"", 10) == 0){
      cJSON *actName2 = cJSON_GetObjectItem(action, "parameters");
      cJSON *actName3 = cJSON_GetObjectItem(actName2, "hour");
      cJSON *actName4 = cJSON_GetObjectItem(actName2, "minute");

      strActName = cJSON_Print(actName);

      hour = atoi(cJSON_Print(actName3));
      minute = atoi(cJSON_Print(actName4));
      set_alarm(hour,minute,0);

   } else if(strncmp(cJSON_Print(actName), "\"enterhome\"", 11) == 0){
      inhome = true;
      printf("true!!!!!!!!");
   } else if(strncmp(cJSON_Print(actName), "\"outhome\"", 9) == 0){
      inhome = false;
      printf("false!!!!!!!!!!!!!!");
   } else if(strncmp(cJSON_Print(actName), "\"fireon\"", 8) == 0){
      fire = true;
      printf("fire!!!!!!!!!!!!!!!!");
   } else if(strncmp(cJSON_Print(actName), "\"fireoff\"", 9) == 0){
      fire = false;
      printf("safe!!!!!!!!!!!!!!!!!!!");
   }
   cJSON_Delete(jsonMsg);
   free(strActName);
   free(payload);
}

// Utility function to configure mqtt client
void initializeConfigUtil(void) {
   uint8_t macId[IFHWADDRLEN];
   int result = netlib_getmacaddr("wl1", macId);
   if (result < 0) {
      printf("Get MAC Address failed. Assigning \
                Client ID as 123456789");
      clientConfig.client_id =
            DEFAULT_CLIENT_ID; // MAC id Artik 053
   } else {
      printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            ((uint8_t *) macId)[0],
            ((uint8_t *) macId)[1], ((uint8_t *) macId)[2],
            ((uint8_t *) macId)[3], ((uint8_t *) macId)[4],
            ((uint8_t *) macId)[5]);
      char buf[12];
      sprintf(buf, "%02x%02x%02x%02x%02x%02x",
            ((uint8_t *) macId)[0],
            ((uint8_t *) macId)[1], ((uint8_t *) macId)[2],
            ((uint8_t *) macId)[3], ((uint8_t *) macId)[4],
            ((uint8_t *) macId)[5]);
      clientConfig.client_id = buf; // MAC id Artik 053
      printf("Registering mqtt client with id = %s\n", buf);
   }

   clientConfig.user_name = device_id;
   clientConfig.password = device_token;
   clientConfig.debug = true;
   clientConfig.on_connect = (void*) onConnect;
   clientConfig.on_disconnect = (void*) onDisconnect;
   clientConfig.on_message = (void*) onMessage;
   clientConfig.on_publish = (void*) onPublish;

   clientConfig.protocol_version = MQTT_PROTOCOL_VERSION_311;
   clientConfig.clean_session = true;

   clientTls.ca_cert = get_ca_cert();
   clientTls.ca_cert_len = sizeof(mqtt_ca_cert_str);
   clientTls.cert = NULL;
   clientTls.cert_len = 0;
   clientTls.key = NULL;
   clientTls.key_len = 0;

   clientConfig.tls = &clientTls;
}

static void ntp_link_error(void)
{
   printf("ntp_link_error() callback is called.\n");
}
void set_alarm(int h, int m, int s)
{
   alarm.tm_hour = h;
   alarm.tm_min = m;
   alarm.tm_sec = s;
}

void check_alart(int ch,int cm, int cs)
{

   if(alarm.tm_hour == ch&& alarm.tm_min == cm){
	   if(alarm.tm_sec <= cs+10 && alarm.tm_sec >= cs)
		   alart();
   }

}

void alart()
{
   int i = 0;
//   fd = pwm_open(pin_buzzer);
   sh1106_clear();
   while(inhome)
   {
	   int fd;
	   int pin_buzzer=0;
	   int max = 1;
	   char *pwm0 = "0";
	   char *pwm1 = "1";
	   int dutycycle=100;
	   int umm[8]={O5_DO, O5_RE, O5_MI, O5_FA, O5_SO, O5_LA, O5_TI, O6_DO};

	   fd = pwm_open(pin_buzzer);

	   printf("Ring! ring! ring! ring! ring!\n");
	   if((i%3) == 0){
		   sh1106_write_btm(ab_R, 5, 1, 24, 40);
		   sh1106_write_btm(ab_i, 34, 1, 24, 40);
		   sh1106_write_btm(ab_n, 63, 1, 24, 40);
		   sh1106_write_btm(ab_g, 92, 1, 24, 40);
	   }
	   else if((i%3)== 1){
		   sh1106_write_btm(ab_D, 5, 1, 24, 40);
	   }
	   else if((i%3)== 2){
		   sh1106_write_btm(ab_o, 34, 1, 24, 40);
      }
      pwm_write(fd, alarm_sound[i%64], alarm_bit[i%64]*200);
      i++;
      up_mdelay(alarm_bit[i%64]*200+50);
      pwm_write(fd, O5_SO, 0);
      pwm_close(fd);

   }

   //end buzzer
//   pwm_write(fd, O5_DO, 0);
//   pwm_close(fd);


   //end alart
   sh1106_clear();
   sh1106_write_btm(dot,62,2,8,7);
   sh1106_write_btm(dot,62,5,8,7);
   time_t current;
   struct tm *ptm;

   current = time(NULL);         /* UTC time */
   current += (g_timezone[TIMEZONE_KST].offset * 3600);
   //    current += (9 * 3600);         //choi 9 hours gap
   ptm = gmtime(&current);
   if (currTimeS != ptm->tm_sec) {
      currTimeS = ptm->tm_sec;
      (void)strftime(g_time_str, TIME_STRING_LEN, "%H:%M:%S", ptm); // current time
      (void)strftime(g_date_str, DATE_STRING_LEN, "%b %d, %Y", ptm); // current date
      printf("current time: %s %s %s\n", g_date_str, g_time_str, g_timezone[TIMEZONE_KST].str);
      sh1106_write_string(10 ,0, g_date_str);
      sh1106_write_string(105 ,7, g_time_str+5);
   }
   sh1106_write_btm(num[currTimeM/10],70,1,24,40);
   sh1106_write_btm(num[currTimeM%10],99,1,24,40);
   sh1106_write_btm(num[currTimeH/10],5,1,24,40);
   sh1106_write_btm(num[currTimeH%10],34,1,24,40);


}

void alart_fire()
{

	sh1106_clear();

	int fd;
	int pin_buzzer=0;
	int max = 1;
	char *pwm0 = "0";
	char *pwm1 = "1";
	int dutycycle=100;
	fd = pwm_open(pin_buzzer);


	while(fire){

		sh1106_write_btm(ab_F, 5, 1, 24, 40);
		sh1106_write_btm(ab_i, 34, 1, 24, 40);
		sh1106_write_btm(ab_r, 63, 1, 24, 40);
		sh1106_write_btm(ab_e, 92, 1, 24, 40);
		pwm_write(fd, O5_DO, 100);
		up_mdelay(250);
		pwm_write(fd, O5_DO, 100);
		up_mdelay(300);
		pwm_write(fd, O5_SO, 100);
	}

    pwm_write(fd, O5_SO, 0);
    pwm_close(fd);
	//end fire
	sh1106_clear();
	sh1106_write_btm(dot,62,2,8,7);
	sh1106_write_btm(dot,62,5,8,7);
	time_t current;
	struct tm *ptm;

	current = time(NULL);         /* UTC time */
	current += (g_timezone[TIMEZONE_KST].offset * 3600);
	//    current += (9 * 3600);         //choi 9 hours gap
	ptm = gmtime(&current);
	if (currTimeS != ptm->tm_sec) {
		currTimeS = ptm->tm_sec;
		(void)strftime(g_time_str, TIME_STRING_LEN, "%H:%M:%S", ptm); // current time
		(void)strftime(g_date_str, DATE_STRING_LEN, "%b %d, %Y", ptm); // current date
		printf("current time: %s %s %s\n", g_date_str, g_time_str, g_timezone[TIMEZONE_KST].str);
		sh1106_write_string(10 ,0, g_date_str);
		sh1106_write_string(105 ,7, g_time_str+5);
	}
	sh1106_write_btm(num[currTimeM/10],70,1,24,40);
	sh1106_write_btm(num[currTimeM%10],99,1,24,40);
	sh1106_write_btm(num[currTimeH/10],5,1,24,40);
	sh1106_write_btm(num[currTimeH%10],34,1,24,40);
}


#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int hello_main(int argc, FAR char *argv[])
#endif
{



   bool wifiConnected = false;

   strTopicMsg = (char*)malloc(sizeof(char)*256);
   strTopicAct = (char*)malloc(sizeof(char)*256);
   sprintf(strTopicMsg, "/v1.1/messages/%s", device_id);
   sprintf(strTopicAct, "/v1.1/actions/%s", device_id);

   memset(&clientConfig, 0, sizeof(clientConfig));
   memset(&clientTls, 0, sizeof(clientTls));

   // for NTP Client
   memset(&g_server_conn, 0, sizeof(g_server_conn));
   g_server_conn[0].hostname = "0.asia.pool.ntp.org";
   g_server_conn[0].port = 123;
   g_server_conn[1].hostname = "1.asia.pool.ntp.org";
   g_server_conn[1].port = 123;

#ifdef CONFIG_CTRL_IFACE_FIFO
   int ret;

   while(!wifiConnected) {
      ret = mkfifo(CONFIG_WPA_CTRL_FIFO_DEV_REQ, CONFIG_WPA_CTRL_FIFO_MK_MODE);
      if (ret != 0 && ret != -EEXIST) {
         printf("mkfifo error for %s: %s", CONFIG_WPA_CTRL_FIFO_DEV_REQ, strerror(errno));
      }
      ret = mkfifo(CONFIG_WPA_CTRL_FIFO_DEV_CFM, CONFIG_WPA_CTRL_FIFO_MK_MODE);
      if (ret != 0 && ret != -EEXIST) {
         printf("mkfifo error for %s: %s", CONFIG_WPA_CTRL_FIFO_DEV_CFM, strerror(errno));
      }

      ret = mkfifo(CONFIG_WPA_MONITOR_FIFO_DEV, CONFIG_WPA_CTRL_FIFO_MK_MODE);
      if (ret != 0 && ret != -EEXIST) {
         printf("mkfifo error for %s: %s", CONFIG_WPA_MONITOR_FIFO_DEV, strerror(errno));
      }
#endif

      if (start_wifi_interface() == SLSI_STATUS_ERROR) {
         printf("Connect Wi-Fi failed. Try Again.\n");
      }
      else {
         wifiConnected = true;
         gpio_write(RED_ON_BOARD_LED, 0); // Turn off Red LED to indicate WiFi connection is established
      }
   }

   printf("Connect to Wi-Fi success\n");

   bool mqttConnected = false;
   bool ipObtained = false;
   printf("Get IP address\n");

   struct dhcpc_state state;
   void *dhcp_handle;

   while(!ipObtained) {
      dhcp_handle = dhcpc_open(NET_DEVNAME);
      ret = dhcpc_request(dhcp_handle, &state);
      dhcpc_close(dhcp_handle);

      if (ret != OK) {
         printf("Failed to get IP address\n");
         printf("Try again\n");
         sleep(1);
      }
      else {
         ipObtained = true;
      }
   }
   netlib_set_ipv4addr(NET_DEVNAME, &state.ipaddr);
   netlib_set_ipv4netmask(NET_DEVNAME, &state.netmask);
   netlib_set_dripv4addr(NET_DEVNAME, &state.default_router);

   printf("IP address  %s\n", inet_ntoa(state.ipaddr));

   up_mdelay(2000);

   int ret_ntp = ntpc_start(g_server_conn, 2, 1000, ntp_link_error);
   printf("ret: %d\n", ret_ntp);

   // Connect to the WiFi network for Internet connectivity
   printf("mqtt client tutorial\n");

   // Initialize mqtt client
   initializeConfigUtil();

   pClientHandle = mqtt_init_client(&clientConfig);
   if (pClientHandle == NULL) {
      printf("mqtt client handle initialization fail\n");
      return 0;
   }

   while (mqttConnected == false ) {
      sleep(2);
      // Connect mqtt client to server
      int result = mqtt_connect(pClientHandle, SERVER_ADDR, SERVER_PORT, 60);

      if (result == 0) {
         mqttConnected = true;
         printf("mqtt client connected to server\n");
         break;
      } else {
         continue;
      }
   }

   bool mqttSubscribe = false;

   // Subscribe to topic of interest
   while (mqttSubscribe == false ) {
      sleep(2);
      int result = mqtt_subscribe(pClientHandle, strTopicAct, 0); //topic - color, QOS - 0
      if (result < 0) {
         printf("mqtt client subscribe to topic failed\n");
         continue;
      }
      mqttSubscribe = true;
      printf("mqtt client Subscribed to the topic successfully\n");
   }

   sh1106_init(0,31,30);



   if(sh1106_write_btm(dot,62,2,8,7))
      return 0;
   if(sh1106_write_btm(dot,62,5,8,7))
      return 0;


   while(1){
      up_mdelay(1000);
      dispClock();
      if(fire)
    	  alart_fire();
      printf("%d\t %d\t %d\t %d\t \n",hour, minute, inhome, fire);
   }

}
static void dispClock(void) {
   time_t current;
   struct tm *ptm;

   current = time(NULL);         /* UTC time */
   current += (g_timezone[TIMEZONE_KST].offset * 3600);
   //    current += (9 * 3600);         //choi 9 hours gap
   ptm = gmtime(&current);
   if (currTimeS != ptm->tm_sec) {
      currTimeS = ptm->tm_sec;
      (void)strftime(g_time_str, TIME_STRING_LEN, "%H:%M:%S", ptm); // current time
      (void)strftime(g_date_str, DATE_STRING_LEN, "%b %d, %Y", ptm); // current date
      printf("current time: %s %s %s\n", g_date_str, g_time_str, g_timezone[TIMEZONE_KST].str);
      sh1106_write_string(10 ,0, g_date_str);
      sh1106_write_string(105 ,7, g_time_str+5);
   }
   if(currTimeM != ptm->tm_min){
      currTimeM = ptm->tm_min;
      sh1106_write_btm(num[currTimeM/10],70,1,24,40);
      sh1106_write_btm(num[currTimeM%10],99,1,24,40);
   }
   if(currTimeH != ptm->tm_hour){
      currTimeH = ptm->tm_hour;
      sh1106_write_btm(num[currTimeH/10],5,1,24,40);
      sh1106_write_btm(num[currTimeH%10],34,1,24,40);
   }
   check_alart(currTimeH,currTimeM,currTimeS);

}
