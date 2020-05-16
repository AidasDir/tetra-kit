#include "tetra_dl.h"
#include "utils.h"

/**
 * @brief User-Plane traffic handling
 *
 */

void tetra_dl::service_u_plane(vector<uint8_t> pdu, mac_logical_channel_t mac_logical_channel)
{
    
    if (mac_logical_channel == TCH_S)                                           // speech frame
    {
        uint16_t speech_frame[690] = {0};
        
        for (int i = 0; i < 6; i++)
        {
            speech_frame[115 * i] = 0x6b21 + i;
        }
        
        for (int i = 0; i < 114; i++)
        {
            speech_frame[1 + i]  = pdu[i] ? -127 : 127;
        }
        
        for (int i = 0; i < 114; i++)
        {
            speech_frame[116 + i] = pdu[114 + i] ? -127 : 127;
        }
        
        for (int i = 0; i < 114; i++)
        {
            speech_frame[231 + i] = pdu[228 + i] ? -127 : 127;
        }
        
        for (int i = 0; i < 90; i++)
        {
            speech_frame[346 + i] = pdu[342 + i] ? -127 : 127;
        }

        // speech frame will be converted in base64 string
        report_start("UPLANE", "TCH_S");
        
        report_add("usage marker", mac_state.downlink_usage_marker);                  // current usage marker
        report_add_compressed("frame", (const unsigned char *)speech_frame, 2 * 690); // actual binary frame 1380 bytes
             
        report_send();
    }
}
