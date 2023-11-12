/*
    Copyright 2021-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <Picovoice_VN.h>
#include "params.h"
#include "contex.h"
#define MEMORY_BUFFER_SIZE (70 * 1024)

static const char* ACCESS_KEY = "3qjTlUDyJsAsY4LxXi4qOWDUcMoJ8PBauYngfCYbrFZSnFdBaWSDew=="; //AccessKey string obtained from Picovoice Console (https://picovoice.ai/console/)

static pv_picovoice_t *handle = NULL;
#define ON_AC "IRHVAC {\"Vendor\":\"DAIKIN160\",\"Model\":-1,\"Mode\":\"Cool\",\"Power\":\"On\"}"
#define OFF_AC "IRHVAC {\"Vendor\":\"DAIKIN160\",\"Model\":-1,\"Mode\":\"Cool\",\"Power\":\"Off\"}"

static int8_t memory_buffer[MEMORY_BUFFER_SIZE] __attribute__((aligned(16)));

static const float PORCUPINE_SENSITIVITY = 0.75f;
static const float RHINO_SENSITIVITY = 0.45f; //0.5
static const float RHINO_ENDPOINT_DURATION_SEC = 1.0f;
static const bool RHINO_REQUIRE_ENDPOINT = true;
String stt = "";

static void wake_word_callback(void) {
    Serial1.println("Wake word detected!");
}

static void inference_callback(pv_inference_t *inference) {
    Serial1.println("{");
    Serial1.print("    is_understood : ");
    Serial1.println(inference->is_understood ? "true" : "false");
    if (inference->is_understood) {
        Serial1.print("    intent : ");
        Serial1.println(inference->intent);
        if (inference->num_slots > 0) {
            Serial1.println("    slots : {");
            for (int32_t i = 0; i < inference->num_slots; i++) {
                Serial1.print("        ");
                Serial1.print(inference->slots[i]);
                Serial1.print(" : ");
                Serial1.println(inference->values[i]);
            }
            Serial1.println("    }");
        }
    }
    Serial1.println("}\n");

    if (inference->is_understood) {
        if (String(inference->intent) == "den") {
            String stt = "";
            String color = "";

            for (int32_t i = 0; i < inference->num_slots; i++) {
                if (String(inference->slots[i]) == "state") {
                    stt = String(inference->values[i]);
                    Serial1.println(stt);
                    
                } 
                else if (String(inference->slots[i]) == "color") {
                    color = String(inference->values[i]);
                    Serial1.println(color);
                }

            }

        }
        else if (String(inference->intent) == "quat") {
 //           String color = "";

            for (int32_t i = 0; i < inference->num_slots; i++) {
                if (String(inference->slots[i]) == "state") {
                    stt= String(inference->values[i]);
                    Serial1.println(stt);
                    
                } 
//                else if (String(inference->slots[i]) == "color") {
//                    color = String(inference->values[i]);
//                    Serial1.printf("Đã %s đèn\n",stt);
//                }

            }
        }
        else if (String(inference->intent) == "dieuhoa") {
            for (int32_t i = 0; i < inference->num_slots; i++) {
                if (String(inference->slots[i]) == "state") {
                    stt = String(inference->values[i]);
                    Serial1.println(stt);
                    if(stt=="bật")
                    {
                      Serial1.println(ON_AC);
                    }
                    else
                    {
                      Serial1.println(OFF_AC);
                    }
                    
                } 
//                else if (String(inference->slots[i]) == "color") {
//                    color = String(inference->values[i]);
//                    Serial1.printf("Đã %s đèn\n",stt);
//                }

            }
        }
        else if (String(inference->intent) == "tivi") {
            for (int32_t i = 0; i < inference->num_slots; i++) {
                if (String(inference->slots[i]) == "state") {
                    stt = String(inference->values[i]);
                    Serial1.println(stt);
                    
                } 
//                else if (String(inference->slots[i]) == "color") {
//                    color = String(inference->values[i]);
//                    Serial1.printf("Đã %s đèn\n",stt);
//                }

            }
        } 

        else if (String(inference->intent) == "hoclenh") {
            for (int32_t i = 0; i < inference->num_slots; i++) {
                if (String(inference->slots[i]) == "model") {
                    stt = String(inference->values[i]);
                    Serial1.println(stt);
//                else if (String(inference->slots[i]) == "color") {
//                    color = String(inference->values[i]);
//                    Serial1.printf("Đã %s đèn\n",stt);
//                }

            }
        }                
    
    pv_inference_delete(inference);
}
}
}

void setup() {
    Serial1.begin(115200);
    while (!Serial1);

    pv_status_t status = pv_audio_rec_init();
    if (status != PV_STATUS_SUCCESS) {
        Serial1.print("Audio init failed with ");
        Serial1.println(pv_status_to_string(status));
        while (1);
    }

    status = pv_picovoice_init(
            ACCESS_KEY,
            MEMORY_BUFFER_SIZE,
            memory_buffer,
            sizeof(KEYWORD_ARRAY),
            KEYWORD_ARRAY,
            PORCUPINE_SENSITIVITY,
            wake_word_callback,
            sizeof(CONTEXT_ARRAY),
            CONTEXT_ARRAY,
            RHINO_SENSITIVITY,
            RHINO_ENDPOINT_DURATION_SEC,
            RHINO_REQUIRE_ENDPOINT,
            inference_callback,
            &handle);
    if (status != PV_STATUS_SUCCESS) {
        Serial1.print("Picovoice init failed with ");
        Serial1.println(pv_status_to_string(status));
        while (1);
    }

    const char *rhino_context = NULL;
    status = pv_picovoice_context_info(handle, &rhino_context);
    if (status != PV_STATUS_SUCCESS) {
        Serial1.print("retrieving context info failed with");
        Serial1.println(pv_status_to_string(status));
        while (1);
    }
    Serial1.println("Wake word: vi xi xi'");
    Serial1.println(rhino_context);
    
}

void loop()
{
    const int16_t *buffer = pv_audio_rec_get_new_buffer();
    if (buffer) {
        const pv_status_t status = pv_picovoice_process(handle, buffer);
        if (status != PV_STATUS_SUCCESS) {
            Serial1.print("Picovoice process failed with ");
            Serial1.println(pv_status_to_string(status));
            while(1);
        }
    }
}
