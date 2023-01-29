/*
MIT License

Copyright(c) 2018 Liam Bindle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <mqtt.h>
#include "wifi.h"

/** 
 * @file 
 * @brief Implements @ref mqtt_pal_sendall and @ref mqtt_pal_recvall and 
 *        any platform-specific helpers you'd like.
 * @cond Doxygen_Suppress
 */




#include <errno.h>

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    uint16_t sent = 0;
    uint16_t SentDatalen;
    WIFI_Status_t retval;
    while(sent < len) {
        //ssize_t tmp = send(fd, (char*)buf + sent, len - sent, flags);
         retval = WIFI_SendData(fd,(uint8_t *)buf+sent,len-sent,&SentDatalen,20);
    	if (retval) {
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += SentDatalen;
    }
    return (ssize_t) sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const char *const start = buf;
    uint16_t RcvDatalen;
    WIFI_Status_t retval;
    do {
    	retval= WIFI_ReceiveData(fd, buf, bufsz, &RcvDatalen, 0);
        if (retval) return  MQTT_ERROR_SOCKET_ERROR;
    	//rv = recv(fd, buf, bufsz, flags);
        if (RcvDatalen > 0) {
            /* successfully read bytes from the socket */
            buf = (char*)buf + RcvDatalen;
            bufsz -= RcvDatalen;
        }

    } while (RcvDatalen > 0);

    return (ssize_t)((char*)buf - start);
}



/** @endcond */
