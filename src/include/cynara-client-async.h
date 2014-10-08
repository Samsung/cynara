/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        src/include/cynara-client-async.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains asynchronous client APIs of Cynara available
 *              with libcynara-client-asynchronous.
 */

#ifndef CYNARA_CLIENT_ASYNC_H
#define CYNARA_CLIENT_ASYNC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t cynara_check_id;
typedef struct cynara_async cynara_async;
typedef struct cynara_async_configuration cynara_async_configuration;

/**
 * \enum cynara_async_status
 * Values indicating the status of connected cynara socket.
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_READ
 * Wait for read events on socket.
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_RW
 * Wait for both read and write events on socket.
 */
typedef enum {
    CYNARA_STATUS_FOR_READ,
    CYNARA_STATUS_FOR_RW
} cynara_async_status;

/**
 * \enum cynara_async_call_cause
 * Values indicating the reason of cynara_response_callback call.
 *
 * \var cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER
 * Callback was called due to response to previous cynara_async_create_request() call.
 *
 * \var cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL
 * Callback was called due to request cancelation with cynara_async_cancel_request() call.
 *
 * \var cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH
 * Callback was called due to cynara_async_finish() call.
 *
 * \var cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE
 * Callback was called due to service not available.
 */
typedef enum {
    CYNARA_CALL_CAUSE_ANSWER,
    CYNARA_CALL_CAUSE_CANCEL,
    CYNARA_CALL_CAUSE_FINISH,
    CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE
} cynara_async_call_cause;

/**
 * \brief Response_callback is registered once in cynara_async_create_request()
 * and will be triggered exactly once in 4 kind of situations:
 * 1) after response is received from cynara service (CYNARA_CALL_CAUSE_ANSWER)
 * 2) when request is canceled with cynara_async_cancel_request() (CYNARA_CALL_CAUSE_CANCEL)
 * 3) when request was pending for response, but cynara_async_finish() was called
 *    (CYNARA_CALL_CAUSE_FINISH)
 * 4) when connection to cynara service was broken and cannot be established again
 *    - probably cynara is unoperational (CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE)
 *
 * \param[in] check_id Number identifying check request. Number is generated in
 *            cynara_async_cancel_request() and returned to user. It can be used to match
 *            response with sent request.
 *            Number is valid since cynara_async_create_request() call till callback call.
 *            After that the number can be reused by cynara to run new request.
 * \param[in] cause Cause of triggering this callback.
 * \param[in] response Response for created request. Should be ignored if cause is not
 *            an answer to request (cause != CYNARA_CALL_CAUSE_ANSWER).
 * \param[in] user_response_data User specific data - passed to cynara library in
 *            cynara_async_cancel_request() is being only remembered by library.
 *            Cynara library does not take any actions on this pointer,
 *            except for giving it back to user in cynara_response_callback.
 *            After that cynara forgets this data.
 */
typedef void (*cynara_response_callback) (cynara_check_id check_id, cynara_async_call_cause cause,
                                          int response, void *user_response_data);

/**
 * \brief Callback used by cynara async API when status of cynara socket is changed in
 * cynara_async_initialize(), cynara_async_create_request(), cynara_async_process(),
 * cynara_async_cancel_request() or cynara_async_finish().
 * File descriptor changes every time cynara library connects or disconnects cynara service.
 * Status change is triggered when check_request or cancel needs to be send to
 * cynara service or sending data has finished and there is nothing more to send to cynara
 * service.
 * Note, that provided file descriptor is used internally by libcynara
 * so user should not use it in other way than waiting on it in event loop.
 * In particular user should not write to, read from or close this fd.
 *
 * \param[in] old_fd Old descriptor which should be unregistered from event loop,
 *            Special value -1 is used when callback is called after first
 *            successful connect.
 * \param[in] new_fd New descriptor which should be registered in event loop,
 *            Special value -1 is used when cynara_async_finish() is called and
 *            cynara is disconnected. In this case status should be ignored.
 * \param[in] status Status indicating which events should be awaited on socket
 * \param[in] user_status_data User specific data - passed to cynara library in
 *            cynara_async_initialize() is being only remembered by library.
 *            Cynara library does not take any actions on this pointer,
 *            except for giving it back to user in cynara_status_callback.
 *            Data should be valid at least until cynara_async_finish() is called.
 */
typedef void (*cynara_status_callback) (int old_fd, int new_fd, cynara_async_status status,
                                        void *user_status_data);

/**
 * \par Description:
 * Initialize cynara-async-client library with given configuration. Create structure used in
 * following API calls and register callback and user_status_data for
 * further cynara async API calls.
 *
 * \par Purpose:
 * This API must be used prior to calling any other cynara async API function.
 * It will create cynara_async structure required for any other cynara async API calls.
 *
 * \par Typical use case:
 * Once before entering event loop and before any other cynara async API is called.
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success
 * returns cynara_async structure.
 *
 * \par Sync (or) Async:
 * This is an synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Structure cynara_async created by cynara_async_initialize() call should be released
 * with cynara_async_finish().
 *
 * \param[out] pp_cynara Placeholder for created cynara_async structure.
 * \param[in] p_conf Configuration for cynara-async-client library.
 *            NULL should be used for default configuration.
 *            Configuration management functions will be added later.
 *            Configuration will be able to specify e.g. size of cache used by library
 *            for holding checks results.
 * \param[in] callback Function called when connection is started.
 *            If NULL, no callback will be called, when status changes.
 * \param[in] user_status_data User specific data, passed to callback is being only remembered
 *            by library. Cynara library does not take any actions on this pointer,
 *            except for giving it back to user in cynara_status_callback.
 *            Data should be valid at least until cynara_async_finish() is called.
 *            Can be NULL.
 *
 * \return CYNARA_API_SUCCESS on success
 *         or negative error code on error.
 */
int cynara_async_initialize(cynara_async **pp_cynara, const cynara_async_configuration *p_conf,
                            cynara_status_callback callback, void *user_status_data);

/**
 * \par Description:
 * Release cynara-async-client library and destroy structure created with cynara_async_initialize().
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-async-client library.
 *
 * \par Typical use case:
 * Once after connection to cynara is not needed.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_async structure. Connection to
 * cynara server is closed. Upon disconnecting this will trigger cynara_status_callback callback
 * with -1 as new_fd param so client can unregister file descriptor connected with cynara. It will
 * also trigger cynara_response_callback callback for each created request with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH as cause param.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * No other call to cynara-async-client library should be made after call to cynara_async_finish().
 *
 * \param[in] p_cynara cynara_async structure. If NULL, then the call has no effect.
 */
void cynara_async_finish(cynara_async *p_cynara);

/**
 * \par Description:
 * Check access to given privilege for specified user, client and client_session in cache.
 *
 * \par Purpose:
 * This API should be used to check if cache holds information about access of user,
 * running application identified as clients to a privilege.
 * This API should be used for fast check in cache.
 *
 * \par Typical use case:
 * A service wants to check in cache, if a client requesting access to some privilege
 * has proper rights.
 *
 * \par Method of function operation:
 * Client (a process / application) requesting access to a privilege is running as some user.
 * For such triple (client, user, privilege) a cache is checked.
 * If cache is invalid it is cleared and call returns same as access not found.
 * Additional parameter client_session
 * may be used to distinguish between client session (e.g. for allowing access only for this
 * particular application launch). EMpty string "" can be used, when session differentation
 * is not needed.
 *
 * \par Sync (or) Async:
 * This is an synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_check_cache() needs cynara_async structure to be created first.
 * Use cynara_async_initialize() before calling this function.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Client defined session.
 * \param[in] user User of running client.
 * \param[in] privilege Privilege that is a subject of a check.
 *
 * \return CYNARA_API_ACCESS_ALLOWED on checked access allowed,
 *         CYNARA_API_ACCESS_DENIED on checked access denied,
 *         CYNARA_API_CACHE_MISS on access not in cache,
 *         or other negative error code on error.
 */
int cynara_async_check_cache(cynara_async *p_cynara, const char *client, const char *client_session,
                             const char *user, const char *privilege);

/**
 * \par Description:
 * Creates access check request to cynara service for client, user accessing given privilege.
 * Set callback and user_response_data to be called and passed when request processing is finished.
 *
 * \par Purpose:
 * This API should be used to create check request for client identified by a triple
 * (client, user, privilege) in custom defined session.
 * Response can be received with cynara_async_process().
 * Check id is returned to pair request with response for canceling purposes.
 *
 * \par Typical use case:
 * When cynara_async_check_cache() returned CYNARA_API_CACHE_MISS, so cynara service
 * has to be asked, if a client requesting access to some privilege has proper rights.
 * To receive matching response client sets callback and specifies arbitrary data to be passed
 * to this callback.
 *
 * \par Method of function operation:
 * Client (a process / application) requesting access to a privilege is running as some user.
 * For such triple (client, user, privilege) a request event is created and added to pending
 * requests for cynara_async_process() to process.
 * Socket status will be changed to CYNARA_STATUS_FOR_RW, to ensure that cynara_async_process()
 * will be triggered in event loop after socket is ready to send request to cynara service.
 * After request is sent and there is nothing more to send to cynara service, status will change
 * back to CYNARA_STATUS_FOR_READ. Status changes are delivered with cynara_status_callback.
 * When function is succesfully called unique check_id is returned. It is used for matching
 * generated request with response, that will be received by registered callback.
 * Because check_id is coded as 16-bit unsigned integer, there can be only 2^16 = 65536 pending
 * requests. When response callback is called either because of getting answer or because
 * of cancel check_id used for taht request is released and can be reused by cynara library.
 * When maximum of pending requests is reached cynara_async_create_request() fails with
 * CYNARA_API_MAX_PENDING_REQUESTS error code.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_create_request() needs cynara_async structure to be created first with
 * cynara_async_initialize().
 * Call cynara_async_cancel_request() to cancel pending request.
 * Call cynara_async_process() to receive response.
 * It is guaranteed that if cynara_async_create_request() function suceeds (CYNARA_API_SUCCESS)
 * a callback will be called exactly once and that it will receive user_response_data.
 * If function fails (returns negative error code) request won't be generated and won't be pending,
 * callback function won't be ever called and user_response_data won't be remembered by library.
 * Also no check_id will be generated and *p_check_id value should be ignored.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Client defined session.
 * \param[in] user User of running client.
 * \param[in] privilege Privilege that is a subject of a check.
 * \param[out] p_check_id Placeholder for check id. If NULL, then no check_id is returned.
 * \param[in] callback Function called when matching response is received.
 *            If NULL then no callback will be called when response, cancel, finish
 *            or service not availble error happens.
 * \param[in] user_response_data User specific data, passed to callback is being only remembered
 *            by library. Cynara library does not take any actions on this pointer,
 *            except for giving it back to user in cynara_response_callback.
 *            Can be NULL.
 *
 * \return CYNARA_API_SUCCESS on success,
 *         CYNARA_API_MAX_PENDING_REQUESTS on too much pending requests,
 *         or other negative error code on error.
 */
int cynara_async_create_request(cynara_async *p_cynara, const char *client,
                                const char *client_session, const char *user, const char *privilege,
                                cynara_check_id *p_check_id, cynara_response_callback callback,
                                void *user_response_data);

/**
 * \par Description:
 * Process events that appeared on cynara socket.
 *
 * \par Purpose:
 * Process events after they appear on cynara socket.
 *
 * \par Typical use case:
 * After request was queued with cynara_async_create_request() this API will return response.
 * When event loop will return readiness on cynara socket, client should use this API.
 *
 * \par Method of function operation:
 * This API sends pending requests, receives all responses and reacts when cynara
 * has disconnected. If cynara has disconnected all values in cache become invalid. During these
 * operations status of cynara socket may change, so cynara_status_callback callback will be
 * triggered to indicate these changes. cynara_response_callback callback will be triggered with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER as cause param when response is available.
 * If cynara has disconnected it will be triggered with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE as cause param.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_process() requires initialized cynara_async structure. For this use
 * cynara_async_initialize().
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_API_SUCCESS on success
 *         or negative error code on error.
 */
int cynara_async_process(cynara_async *p_cynara);

/**
 * \par Description:
 * Cancel request created by cynara_async_create_request().
 *
 * \par Purpose:
 * This API should be used to cancel pending check request,
 * created by cynara_async_create_request().
 *
 * \par Typical use case:
 * When cynara client is no longer interested in receiving an answer.
 * Same check_id value should be used to identify proper request as was generated during
 * request creation with cynara_async_create_request().
 *
 * \par Method of function operation:
 * Cancels request created by cynara_async_create_request() call.
 * cynara_status_callback callback may be triggered to be able to send cancel to cynara.
 * cynara_response_callback callback will be triggered with with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL as cause param.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_cancel_request() needs cynara_async structure to be created first. For this
 * use cynara_async_initialize().
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] check_id Check id to be cancelled
 *
 * \return CYNARA_API_SUCCESS on success
 *         or negative error code on error.
 */
int cynara_async_cancel_request(cynara_async *p_cynara, cynara_check_id check_id);

/*
 *  Example of usage:
 *
 *  void change_status(int old_fd, int new_fd, cynara_async_status status, void *user_status_data)
 *  {
 *      // unregister using old_fd
 *      ...
 *      //register using new_fd and new status
 *  }
 *
 *  int process_response(int check_id, cynara_async_call_cause cause, int response,
 *                       void *user_response_data)
 *  {
 *      switch (cause) {
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER:
 *          // handle answer from cynara service - use response value
 *          break;
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL:
 *          // handle canceled request
 *          break;
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH:
 *          // handle finish of client async
 *          break;
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE:
 *          // handle disconnection
 *      }
 *      ...
 *      free_user_response_data(user_response_data);
 *  }
 *
 *  void main_process
 *  {
 *      //initialize all required objects
 *      cynara_async *p_cynara;
 *      int ret;
 *      //change_status will be called passing file descriptor connected to cynara server
 *      if ((ret = cynara_async_initalize(&p_cynara, NULL, change_status, &fd_sets)) !=
 *            CYNARA_API_SUCCESS) {
 *          // handle error
 *      }
 *
 *      //start event loop
 *      event_process(fd_sets, ...)
 *
 *      //event loop stopped, clean-up, before closing program
 *      cynara_async_finish(p_cynara);
 *  }
 *
 *  void event_process(fd_sets, ...)
 *  {
 *      //event loop of user choice
 *      select(..., fd_sets.read, fd_sets.write) {
 *         ...
 *         if(active_socket == cynara_socket) {
 *             if(cynara_async_process(p_cynara) != CYNARA_API_SUCCESS) {
 *                // handle error
 *             }
 *         } else if (active_socket == some_client_socket) {
 *             //processing clients, which may require cynara checks
 *             ...
 *             int ret = cynara_async_check_cache(p_cynara, client, client_session, user,
 *                                                privilege);
 *             switch(ret) {
 *             case CYNARA_API_ACCESS_ALLOWED:
 *                 // allow client
 *                 break;
 *             case CYNARA_API_ACCESS_DENIED:
 *                 // deny client
 *                 break;
 *             case CYNARA_API_CACHE_MISS:
 *                 // not in cache - prepare data that will be passed to callback
 *                 allocate_user_response_data(&user_response_data);
 *
 *                 // create request
 *                 ret = cynara_async_create_request(p_cynara,
 *                                                   client, client_session, user, privilege,
 *                                                   &check_id,
 *                                                   process_response, user_response_data);
 *                 if(ret != CYNARA_API_SUCCESS) {
 *                     free_user_response_data(user_response_data);
 *                     // handle error
 *                 }
 *             default:
 *                 // handle error
 *             }
 *             ...
 *             // waiting for some request too long
 *             if (ret = cynara_async_cancel_request(p_cynara, check_id)) != CYNARA_API_SUCCESS) {
 *                 // handle error
 *             }
 *         }
 *         ...
 *      }
 *      ...
 *  }
 */

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_ASYNC_H */
