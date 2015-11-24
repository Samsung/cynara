/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @author      Oskar Switalski <o.switalski@samsung.com>
 * @version     1.0
 * @brief       This file contains asynchronous client APIs of Cynara available
 *              with libcynara-client-asynchronous.
 */

#ifndef CYNARA_CLIENT_ASYNC_H
#define CYNARA_CLIENT_ASYNC_H

#include <stddef.h>
#include <stdint.h>

#include <cynara-error.h>
#include <cynara-limits.h>

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
 * Callback was called due to response to previous cynara_async_create_request() or
 * cynara_async_create_simple_request() call.
 *
 * \var cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL
 * Callback was called due to request cancellation with cynara_async_cancel_request() call.
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
 * \brief Response_callback is registered once in cynara_async_create_request() or
 * cynara_async_create_simple_request() and will be triggered exactly once in 4 kinds of situations:
 *
 * -# after response is received from cynara service (CYNARA_CALL_CAUSE_ANSWER)
 * -# when request is canceled with cynara_async_cancel_request() (CYNARA_CALL_CAUSE_CANCEL)
 * -# when request was pending for response, but cynara_async_finish() was called
 *    (CYNARA_CALL_CAUSE_FINISH)
 * -# when connection to cynara service was broken and cannot be established again - probably cynara
 *    is unoperational (CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE)
 *
 * API functions called during this callback with CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE
 * or CYNARA_CALL_CAUSE_FINISH cause will return CYNARA_API_OPERATION_NOT_ALLOWED.
 * cynara_async_finish() will be ignored if called from within this callback.
 *
 * \param[in] check_id Number identifying check request. Number is generated in
 *            cynara_async_create_request() or cynara_async_create_simple_request()
 *            and returned to user. It can be used to match response with sent request.
 *            Number is valid since cynara_async_create_request() call or
 *            cynara_async_create_simple_request() call till callback call.
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
 * cynara_async_initialize(), cynara_async_create_request(), cynara_async_create_simple_request(),
 * cynara_async_process(), cynara_async_cancel_request() or cynara_async_finish().
 *
 * File descriptor changes every time cynara library connects or disconnects cynara service.
 * Status change is triggered when check_request or cancel needs to be send to
 * cynara service or sending data has finished and there is nothing more to send to cynara
 * service.
 *
 * Note, that provided file descriptor is used internally by libcynara-client-async
 * so user should not use it in other way than waiting on it in event loop.
 * In particular user should not write to, read from or close this fd.
 * CYNARA_API_OPERATION_NOT_ALLOWED will be returned for every api function called in this callback.
 * cynara_async_finish() will be ignored if called from within this callback.
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
 * Initialize cynara_async_configuration. Create structure used in following configuration
 * API calls.
 *
 * \par Purpose:
 * For configuration parameter to be used in cynara async initialization function, this API must be
 * called before any other cynara async configuration API function.
 * It will create cynara_async_configuration structure, an optional parameter of cynara async
 * initialization.
 *
 * \par Typical use case:
 * Once before setting parameters of cynara async configuration and passing to
 * cynara_async_initialize().
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success returns pointer
 * to created cynara_async_configuration structure.
 *
 * \par Sync (or) Async:
 * This as a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Structure cynara_async_configuration created by cynara_async_configuration_create() call
 * should be released with cynara_async_configuration_destroy().
 * Structure cynara_async_configuration should be destroyed after passing it to
 * cynara_async_initialize().
 *
 * \param[out] pp_conf Placeholder for created cynara_async_configuration structure.
 *
 * \return CYNARA_API_SUCCESS on success
 * \return negative error code on error
 */
int cynara_async_configuration_create(cynara_async_configuration **pp_conf);

/**
 * \par Description:
 * Release cynara_async_configuration structure created with cynara_async_configuration_create().
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara_async_configuration.
 *
 * \par Typical use case:
 * Once cynara_async_configuration is not needed.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_async_configuration structure.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \param[in] p_conf cynara_async_configuration structure. If NULL, the call has no effect.
 */
void cynara_async_configuration_destroy(cynara_async_configuration *p_conf);

/**
 * \par Description:
 * Set client cache size.
 *
 * \par Purpose:
 * This API is used to change default number of cached responses returned from cynara.
 *
 * \par Typical use case:
 * Once before setting parameters of cynara async configuration and passing to
 * cynara_async_initialize().
 *
 * \par Method of function operation:
 * This API initializes cache with given capacity.
 *
 * \par Sync (or) Async:
 * This as a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * After passing cynara_async_configuration to cynara_async_initialize() calling this API will have
 * no effect.
 *
 * \param[in] p_conf cynara_async_configuration structure pointer.
 * \param[in] cache_size Cache size to be set.
 *
 * \return CYNARA_API_SUCCESS on success
 * \return negative error code on error
 */
int cynara_async_configuration_set_cache_size(cynara_async_configuration *p_conf,
                                              size_t cache_size);

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
 * \return negative error code on error
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
 * cynara_async_finish() called from within cynara_response_callback or cynara_status_callback will
 * be ignored.
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
 * particular application launch). Empty string "" can be used, when session differentation
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
 * \parblock
 * Call to cynara_async_check_cache() needs cynara_async structure to be created first.
 * Use cynara_async_initialize() before calling this function. cynara_async_check_cache() called
 * from within cynara_status_callback or cynara_response_callback with
 * CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE or CYNARA_CALL_CAUSE_FINISH cause will return
 * CYNARA_API_OPERATION_NOT_ALLOWED.
 *
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 * \endparblock
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Client defined session.
 * \param[in] user User of running client.
 * \param[in] privilege Privilege that is a subject of a check.
 *
 * \return CYNARA_API_ACCESS_ALLOWED on checked access allowed
 * \return CYNARA_API_ACCESS_DENIED on checked access denied
 * \return CYNARA_API_CACHE_MISS on access not in cache
 * \return other negative error code on error
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
 * \parblock
 * Client (a process / application) requesting access to a privilege is running as some user.
 * For such triple (client, user, privilege) a request event is created and added to pending
 * requests for cynara_async_process() to process.
 *
 * Socket status will be changed to CYNARA_STATUS_FOR_RW, to ensure that cynara_async_process()
 * will be triggered in event loop after socket is ready to send request to cynara service.
 * After request is sent and there is nothing more to send to cynara service, status will change
 * back to CYNARA_STATUS_FOR_READ. Status changes are delivered with cynara_status_callback.
 * When function is successfully called unique check_id is returned. It is used for matching
 * generated request with response, that will be received by registered callback.
 *
 * Because check_id is coded as 16-bit unsigned integer, there can be only 2^16 = 65536 pending
 * requests. When response callback is called either because of getting answer or because
 * of cancel check_id used for that request is released and can be reused by cynara library.
 * When maximum of pending requests is reached cynara_async_create_request() fails with
 * CYNARA_API_MAX_PENDING_REQUESTS error code.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * \parblock
 * Call cynara_async_create_request() needs cynara_async structure to be created first with
 * cynara_async_initialize().
 * Call cynara_async_cancel_request() to cancel pending request.
 * Call cynara_async_process() to receive response.
 *
 * It is guaranteed that if cynara_async_create_request() function succeeds (CYNARA_API_SUCCESS)
 * a callback will be called exactly once and that it will receive user_response_data.
 *
 * If function fails (returns negative error code) request won't be generated and won't be pending,
 * callback function won't be ever called and user_response_data won't be remembered by library.
 *
 * Also no check_id will be generated and *p_check_id value should be ignored.
 * cynara_async_create_request() called from within cynara_status_callback or
 * cynara_response_callback with CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE or CYNARA_CALL_CAUSE_FINISH
 * cause will return CYNARA_API_OPERATION_NOT_ALLOWED.
 *
 * String length cannot exceed CYNARA_MAX_ID_LENGTH ,otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 * \endparblock
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
 * \return CYNARA_API_SUCCESS on success
 * \return CYNARA_API_MAX_PENDING_REQUESTS on too much pending requests
 * \return other negative error code on error
 */
int cynara_async_create_request(cynara_async *p_cynara, const char *client,
                                const char *client_session, const char *user, const char *privilege,
                                cynara_check_id *p_check_id, cynara_response_callback callback,
                                void *user_response_data);

/**
 * \par Description:
 * Creates simple access check request to cynara service for (potential) permission to take some
 * action or access a resource.
 * Set callback and user_response_data to be called and passed when request processing is finished.
 *
 * \par Purpose:
 * This API should be used for a quick check if a user running application identified as client
 * has access to a given privilege.
 * Response can be received with cynara_async_process().
 * Check id is returned to pair request with response for canceling purposes.
 *
 * \par Typical use case:
 * An application may use this API to check if it has (potential) permission to take some action
 * or access resource in future (permissions may rarely change). The typical use would be to disable
 * or hide some of functionalities if they probably could not be used anyways.
 *
 * \par Method of function operation:
 * \parblock
 * This function is very similar to cynara_async_create_request() with the difference, that in case
 * of answer not being one of CYNARA_API_PERMISSION_DENIED or CYNARA_API_PERMISSION_ALLOWED,
 * no external application will be consulted. Instead, CYNARA_API_ACCESS_NOT_RESOLVED is returned
 * by a callback, meaning, that only creating full request through cynara_async_create_request() API
 * would yield eventual answer.
 *
 * If access permission cannot be acquired without usage of external agents, callback can be
 * called with CYNARA_CALL_CAUSE_ANSWER and response value being CYNARA_API_ACCESS_NOT_RESOLVED.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * \parblock
 * Call cynara_async_create_simple_request() needs cynara_async structure to be created first
 * with cynara_async_initialize().
 * Call cynara_async_cancel_request() to cancel pending request.
 * Call cynara_async_process() to send request and receive response.
 *
 * The answer will be taken from cynara's database without consulting any external applications.
 * If the answer cannot be resolved in one of CYNARA_API_ACCESS_ALLOWED or
 * CYNARA_API_ACCESS_DENIED without communicating with external application, response returned
 * through callback will have value CYNARA_API_ACCESS_NOT_RESOLVED.
 *
 * String length cannot exceed CYNARA_MAX_ID_LENGTH, otherwise CYNARA_API_INVALID_PARAM will be
 * returned.
 * \endparblock
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
 * \param[in] user_response_data User specific data, passed to callback is being only stored by
 *            library. Cynara library does not take any actions on this pointer, except for giving
 *            it back to user in cynara_response_callback.
 *            Can be NULL.
 *
 * \return CYNARA_API_SUCCESS on success
 * \return CYNARA_API_MAX_PENDING_REQUESTS on too much pending requests
 * \return other negative error code on error
 */
int cynara_async_create_simple_request(cynara_async *p_cynara, const char *client,
                                       const char *client_session, const char *user,
                                       const char *privilege, cynara_check_id *p_check_id,
                                       cynara_response_callback callback, void *user_response_data);

/**
 * \par Description:
 * Process events that appeared on cynara socket.
 *
 * \par Purpose:
 * Process events after they appear on cynara socket.
 *
 * \par Typical use case:
 * After request was queued with cynara_async_create_request() or
 * cynara_async_create_simple_request() this API will return response.
 * When event loop will return readiness on cynara socket, client should use this API.
 *
 * \par Method of function operation:
 * \parblock
 * This API sends pending requests, receives all responses and reacts when cynara
 * has disconnected. If cynara has disconnected all values in cache become invalid. During these
 * operations status of cynara socket may change, so cynara_status_callback callback will be
 * triggered to indicate these changes. cynara_response_callback callback will be triggered with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER as cause param when response is available.
 *
 * If cynara has disconnected it will be triggered with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE as cause param.
 * \endparblock
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
 * cynara_async_initialize(). cynara_async_process() called from within cynara_status_callback or
 * cynara_response_callback with CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE or CYNARA_CALL_CAUSE_FINISH
 * cause will return CYNARA_API_OPERATION_NOT_ALLOWED.
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_API_SUCCESS on success
 * \return negative error code on error
 */
int cynara_async_process(cynara_async *p_cynara);

/**
 * \par Description:
 * Cancel request created by cynara_async_create_request() or cynara_async_create_simple_request().
 *
 * \par Purpose:
 * This API should be used to cancel pending check request,
 * created by cynara_async_create_request() or cynara_async_create_simple_request().
 *
 * \par Typical use case:
 * When cynara client is no longer interested in receiving an answer.
 * Same check_id value should be used to identify proper request as was generated during
 * request creation with cynara_async_create_request() or cynara_async_create_simple_request().
 *
 * \par Method of function operation:
 * \parblock
 * Cancels request created by cynara_async_create_request() or cynara_async_create_simple_request()
 * call.
 *
 * cynara_status_callback callback may be triggered to be able to send cancel to cynara.
 * cynara_response_callback callback will be triggered with with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL as cause param.
 *
 * If given id is not valid (was not requested or response callback was already delivered)
 * cynara_async_cancel_request() returns CYNARA_API_INVALID_PARAM.
 * \endparblock
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
 * use cynara_async_initialize(). cynara_async_cancel_request() called from within
 * cynara_status_callback or cynara_response_callback with CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE
 * or CYNARA_CALL_CAUSE_FINISH cause will return CYNARA_API_OPERATION_NOT_ALLOWED.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] check_id Check id to be cancelled
 *
 * \return CYNARA_API_SUCCESS on success
 * \return negative error code on error
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
