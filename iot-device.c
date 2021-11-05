#include <stdio.h>
#include <stdlib.h>

#include <iothub.h>
#include <iothub_device_client_ll.h>
#include <iothub_client_options.h>
#include <iothub_message.h>
#include <azure_c_shared_utility/threadapi.h>
#include <azure_c_shared_utility/crt_abstractions.h>
#include <azure_c_shared_utility/shared_util_options.h>

#include "electric_shock.h"

/* --- Connection Protocol --- */

#define PROTOCOL_MQTT

#ifdef PROTOCOL_MQTT
#include "iothubtransportmqtt.h"
#endif // PROTOCOL_MQTT

/* --- Variables --- */

static const char* connectionString = "";
static bool g_continueRunning = true;


/* --- Methods --- */
static void ConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context) {
	(void)reason;
	(void)user_context;

	if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) {
		//(void)printf("The device client is connected to IoTHub\r\n");
	}
	else {
		//(void)printf("The device client has been disconnected\r\n");
	}
}


static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* user_context) {
	(void)user_context;

	const char* buffer;
	size_t size;
	const char* messageId;
	const char* correlationId;

	// Message Properties
	if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL) {
		messageId = "<null>";
	}

	if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL) {
		correlationId = "<null>";
	}

	(void)printf("MessageId: %s\r\n", messageId);
	(void)printf("CorrelationId: %s\r\n", correlationId);

	// Message Content
	if (IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size) != IOTHUB_MESSAGE_OK) {
		(void)printf("Unable to retrive the message\r\n");
	}
	else {
		(void)printf("Received Message From IoTHub\r\n");

		(void)InitElectricShot();

		// Finish if message is quit
		if (size == (strlen("quit") * sizeof(char)) && memcmp(buffer, "quit", size) == 0) {
			g_continueRunning = false;
		}
	}

	return IOTHUBMESSAGE_ACCEPTED;
}


int main(void) {
	(void)printf("Init AI quiz device\r\n");

	IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
	
#ifdef PROTOCOL_MQTT
	protocol = MQTT_Protocol;
#endif // PROTOCOL_MQTT

	// Init IoTHub SUbsystem
	(void)IoTHub_Init();

	IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;

	(void)printf("Creating IoTHub Device Handle\r\n");

	device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, protocol);
	if (device_ll_handle == NULL) {
		(void)printf("Failure creating IoTHub Device. Check the connection string.\r\n");
	}
	else {
		(void)printf("Connection Success\r\n");

#if defined PROTOCOL_MQTT
		bool urlEncodeOn = true;
		(void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);
#endif 

		// Set Connection Status Callback for IoTHub connection status
		(void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(device_ll_handle, ConnectionStatusCallback, NULL);

		// Get Message from IoTHub
		if (IoTHubDeviceClient_LL_SetMessageCallback(device_ll_handle, ReceiveMessageCallback, NULL) != IOTHUB_CLIENT_OK) {
			(void)printf("Error setting callback for Receive Message from IoTHub\r\n");
			g_continueRunning = false;
		}
		else {
			(void)printf("Successfully set callback for Receive Message\r\n");
		}

		do {
			IoTHubDeviceClient_LL_DoWork(device_ll_handle);
			ThreadAPI_Sleep(1);
		} while (g_continueRunning);


		// End IoT Hub connection
		IoTHubDeviceClient_LL_Destroy(device_ll_handle);
	}

	// Free subsystem
	IoTHub_Deinit();

	(void)printf("Press any key to finish");
	(void)getchar();

	return 0;
}