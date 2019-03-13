/* WiFi Example
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef treasure_data_rest_h
#define treasure_data_rest_h


#include "mbed.h"
#include "https_request.h"
#include "treasuredata-sslcert.h"
// #define  TDURL "http://in.treasuredata.com/postback/v3/event/{database}/{table}?td_write_key={APIKEY}"
const char *TDURL = "https://in.treasuredata.com/postback/v3/event/%s/%s";
#define  URL_SIZE 200
#define  POST_BUFFER_SIZE 200

#define TD_DEBUG false

class TreasureData_RESTAPI {

public:

	TreasureData_RESTAPI(NetworkInterface* aNetwork,char* aDatabase, char* aTable, const char *aAPIKey){
		apikey 		= aAPIKey;
		network 	= aNetwork;
		table 		= aTable;
		database 	= aDatabase;

		// Assemble URL
		int x = 0;
        x = sprintf(urlbuff,TDURL,database,table);
        urlbuff[x] = 0; // null terminate string
        printf("\r\n TD Initialized, will send data to TDURL: %s",urlbuff);

        // Assemble TD APIKey
        x = 0;
        x = sprintf(apikeybuff,"%s",apikey);
        apikeybuff[x] = 0; // null terminate string
        // printf("\r\n API Key is %s",apikeybuff);
	}

	// Input expected to be JSON string
	// EX) "{\"key\":\"value\",}"
	int sendData(char * keyvalue, uint32_t size){
		{
			// char buffer [POST_BUFFER_SIZE] = {0}; // use for data
			HttpsRequest* post_req;
	        HttpResponse* post_res;
	        int x = 0; // use to check size of arrays

	        post_req = new HttpsRequest(network,SSL_CA_PEM, HTTP_POST, urlbuff);
	        post_req->set_header("Content-type", "application/json");
	        post_req->set_header("Accept","text/plain");
	        post_req->set_header("X-TD-Write-Key",apikeybuff);

	        #ifdef TD_DEBUG
		        printf("\r\n Posting data: '%s'\r\n",keyvalue);
		        printf("Posting Data to %s",urlbuff);
	        #endif

	        post_res = post_req->send(keyvalue, size);
	        if (!post_res) {
	            printf("\r\nHttpsRequest failed (error code %d)\n", post_req->get_error());
	            delete post_req;
	            return 1;
	        }

	        #ifdef TD_DEBUG
	        	printf("\n----- HTTPS POST response -----\n");
	        	dump_response(post_res);
	        #endif
	        
	        delete post_req;
	        return 0;
		}
	}

private:

	NetworkInterface* network;
	const char * apikey;
	char apikeybuff[50]={0};
	char * table;
	char * database;
	char urlbuff [URL_SIZE]={0};	// use for URL

#ifdef TD_DEBUG
	void dump_response(HttpResponse* res) {
	    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

	    printf("Headers:\n");
	    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
	        printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
	    }
	    printf("\nBody (%d bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
	}
#endif

};


#endif