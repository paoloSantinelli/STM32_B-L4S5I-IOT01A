#ifndef __MQTT_PAL_H__
#define __MQTT_PAL_H__

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Includes/supports the types/calls required by the MQTT-C client.
 * 
 * @note This is the \em only file included in mqtt.h, and mqtt.c. It is therefore 
 *       responsible for including/supporting all the required types and calls. 
 * 
 * @defgroup pal Platform abstraction layer
 * @brief Documentation of the types and calls required to port MQTT-C to a new platform.
 * 
 * mqtt_pal.h is the \em only header file included in mqtt.c. Therefore, to port MQTT-C to a 
 * new platform the following types, functions, constants, and macros must be defined in 
 * mqtt_pal.h:
 *  - Types:
 *      - \c size_t, \c ssize_t
 *      - \c uint8_t, \c uint16_t, \c uint32_t
 *      - \c va_list
 *      - \c mqtt_pal_time_t : return type of \c MQTT_PAL_TIME() 
 *      - \c mqtt_pal_mutex_t : type of the argument that is passed to \c MQTT_PAL_MUTEX_LOCK and 
 *        \c MQTT_PAL_MUTEX_RELEASE
 *  - Functions:
 *      - \c memcpy, \c strlen
 *      - \c va_start, \c va_arg, \c va_end
 *  - Constants:
 *      - \c INT_MIN
 * 
 * Additionally, three macro's are required:
 *  - \c MQTT_PAL_HTONS(s) : host-to-network endian conversion for uint16_t.
 *  - \c MQTT_PAL_NTOHS(s) : network-to-host endian conversion for uint16_t.
 *  - \c MQTT_PAL_TIME()   : returns [type: \c mqtt_pal_time_t] current time in seconds. 
 *  - \c MQTT_PAL_MUTEX_LOCK(mtx_pointer) : macro that locks the mutex pointed to by \c mtx_pointer.
 *  - \c MQTT_PAL_MUTEX_RELEASE(mtx_pointer) : macro that unlocks the mutex pointed to by 
 *    \c mtx_pointer.
 * 
 * Lastly, \ref mqtt_pal_sendall and \ref mqtt_pal_recvall, must be implemented in mqtt_pal.c 
 * for sending and receiving data using the platforms socket calls.
 */



	#include <string.h>
	#include "cmsis_os.h"
	#include "stdarg.h"
	#include <limits.h>
    #include <time.h>
    #include <stdint.h>
	extern  osMutexId_t  mqtt_mutexHandle;

	typedef uint8_t mqtt_pal_socket_handle;
    //typedef uint16_t ssize_t;
	#define MQTT_PAL_HTONS(x) ( ((x) << 8) | ( ((x) >> 8) & 0xFF) )
    //define MQTT_PAL_HTONS(s) htons(s)
	#define MQTT_PAL_NTOHS(x) MQTT_PAL_HTONS(x)
    //#define MQTT_PAL_NTOHS(s) ntohs(s)

    #define MQTT_PAL_TIME() time(NULL)

    typedef time_t mqtt_pal_time_t;
    typedef osMutexId_t mqtt_pal_mutex_t;

    #define MQTT_PAL_MUTEX_INIT(mtx_ptr) (*mtx_ptr = mqtt_mutexHandle)
    #define MQTT_PAL_MUTEX_LOCK(mtx_ptr) osMutexAcquire (mtx_ptr, osWaitForever)
    #define MQTT_PAL_MUTEX_UNLOCK(mtx_ptr) osMutexRelease(mtx_ptr)


    /*#ifndef MQTT_USE_CUSTOM_SOCKET_HANDLE
        #ifdef MQTT_USE_BIO
            #include <openssl/bio.h>
            typedef BIO* mqtt_pal_socket_handle;
        #else
            typedef SOCKET mqtt_pal_socket_handle;
        #endif
    #endif*/


/**
 * @brief Sends all the bytes in a buffer.
 * @ingroup pal
 * 
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the first byte in the buffer to send.
 * @param[in] len The number of bytes to send (starting at \p buf).
 * @param[in] flags Flags which are passed to the underlying socket.
 * 
 * @returns The number of bytes sent if successful, an \ref MQTTErrors otherwise.
 */
ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags);

/**
 * @brief Non-blocking receive all the byte available.
 * @ingroup pal
 * 
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the receive buffer.
 * @param[in] bufsz The max number of bytes that can be put into \p buf.
 * @param[in] flags Flags which are passed to the underlying socket.
 * 
 * @returns The number of bytes received if successful, an \ref MQTTErrors otherwise.
 */
ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags);



#ifdef __cplusplus
}
#endif


#endif
