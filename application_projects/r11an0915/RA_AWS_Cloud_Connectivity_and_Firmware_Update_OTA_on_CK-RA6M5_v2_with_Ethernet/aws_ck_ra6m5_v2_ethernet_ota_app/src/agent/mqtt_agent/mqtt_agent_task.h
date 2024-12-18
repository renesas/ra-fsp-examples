/*
 * Lab-Project-coreMQTT-Agent 201215
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 */


#ifndef _MQTT_AGENT_TASK_H_
#define _MQTT_AGENT_TASK_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "core_mqtt_serializer.h"
#include <usr_config.h>
#include "common_utils.h"
/**
 * @brief Enum defines states which MQTT agent exposes to the MQTT application tasks.
 * Application can query the state of the MQTT agent or wait for MQTT agent to reach a
 * desired state.
 */
typedef enum MQTTAgentState
{
    MQTT_AGENT_STATE_NONE = 0,
    MQTT_AGENT_STATE_INITIALIZED = 1,   //Set this state when fleet provisioning demo finish and the MQTT task ready to run.
    MQTT_AGENT_STATE_DISCONNECTED = 2,
    MQTT_AGENT_STATE_CONNECTED = 3,
    MQTT_AGENT_STATE_TERMINATED = 4,
    MQTT_AGENT_NUM_STATES
} MQTTAgentState_t;

#undef LogInfo
#undef LogError
#undef LogWarn
#undef LogDebug
#undef LogErrorTrap

#define LogInfo              IotLogInfo
#define LogError             IotLogError
#define LogWarn              IotLogWarn
#define LogDebug             IotLogDebug
#define LogErrorTrap         IotLogErrorTrap
/* Macros used tunnel the IotLogging to RTT and SWO */
#define IotLogInfo              APP_INFO_PRINT
#define IotLogError             APP_ERR_PRINT
#define IotLogWarn              APP_WARN_PRINT
#define IotLogDebug             APP_DBG_PRINT
#define IotLogErrorTrap         APP_ERR_TRAP

/**
 * @brief Callback function called when receiving a publish.
 *
 * @param[in] pvIncomingPublishCallbackContext The incoming publish callback context.
 * @param[in] pxPublishInfo Deserialized publish information.
 */
typedef void (* IncomingPubCallback_t )( void * pvIncomingPublishCallbackContext,
					MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Initialize the MQTT Agent.
 * Create the semaphore and Event group.
 */
BaseType_t xMQTTAgentInit( void );

/**
 * @brief Starts the MQTT agent task.
 * MQTT agent task calls MQTTAgent_CommandLoop(), until MQTTAgent_Terminate()
 * is called. If an error occurs in the command loop, then it will reconnect the
 * TCP and MQTT connections.
 *
 * @param[in] uxStackSize Stack size for MQTT agent task.
 * @param[in] uxPriority Priority of MQTT agent task.
 */
void vStartMQTTAgent( configSTACK_DEPTH_TYPE uxStackSize,
        UBaseType_t uxPriority  );


/**
 * @brief Get the current state of MQTT agent.
 * MQTT agent task can be any of the states as defined by the state enum MQTTAgentState_t. The
 * function can be used to poll for MQTT agent state from other application tasks
 *
 * @return State of the MQTT agent.
 */
MQTTAgentState_t xGetMQTTAgentState( void );

/**
 * @brief Set the current state of MQTT agent.
 * MQTT agent task can be any of the states as defined by the state enum MQTTAgentState_t. The
 * function can be used to set the MQTT agent state from other application tasks
 *
 * @param[in] xAgentState State of the MQTT agent.
 */
void xSetMQTTAgentState( MQTTAgentState_t xAgentState );

/**
 * @brief Wait for MQTT agent to reach the desired state.
 * Function blocks caller task for a timeout period, until MQTT agent reaches the specified state. Function can be
 * called from multiple tasks concurrently.
 *
 * @param[in] xStateToWait The desired MQTT agent state to reach.
 * @param[in] xTicksToWait Ticks to wait for MQTT agent to reach the desired state. Provide portMAX_DELAY
 *                         if the caller has to wait indefinitely.
 * @return pdTRUE if the state is reached, pdFALSE if the timeout has reached waiting for the state.
 */
BaseType_t xWaitForMQTTAgentState( MQTTAgentState_t xStateToWait,
                                   TickType_t xTicksToWait );

/**
 * @brief Add a callback for the topic filter with MQTT agent.
 * Function adds a local subscription for the given topic filter. Each incoming publish received on the connection, will be
 * matched against the topic filter. The provided callback is invoked if the incoming publish topic matches the topic filter.
 * An optional context parameter can be passed into this function from the caller, the context is passed back transparently when
 * the publish callback is invoked. If a topic matches more than one registered topic filter, all matching topic filter callbacks
 * are invoked. The function is thread safe and can be invoked from multiple application tasks once the MQTT agent is
 * initialized.
 *
 *
 * @param pcTopicFilter  Topic filter string for which the publish callback needs to be invoked.
 * @param usTopicFilterLength  Length of the topic filter string.
 * @param pxPublishCallback  Callback to be invoked whan a publish is received on topic filter.
 * @param pvCallbackContext Context to be paased in transparently to the user.
 * @param xManageResubscription  Whether its a managed subscription. Managed subscriptions are resubscribed by
 *                               MQTT agent if the connection is lost and then reconnected.
 * @return pdTRUE if the callback was added successfully. pdFALSE otherwise.
 */
BaseType_t xAddMQTTTopicFilterCallback( const char * pcTopicFilter,
                                        uint16_t usTopicFilterLength,
                                        IncomingPubCallback_t pxPublishCallback,
                                        void * pvCallbackContext,
                                        BaseType_t xManageResubscription );

/**
 * @brief Remove a topic filter callback from the MQTT agent.
 * Function is thread safe and can be invoked by multiple application tasks.
 *
 * @param pcTopicFilter Topic filter string for which the callback needs to be removed.
 * @param usTopicFilterLength Length of the topic filter string.
 */
void vRemoveMQTTTopicFilterCallback( const char * pcTopicFilter,
                                     uint16_t usTopicFilterLength );


#endif /* ifndef _MQTT_AGENT_TASK_H_ */
