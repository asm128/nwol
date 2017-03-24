// Copyright (C) 2010 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License"); You may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.
#include "glue.h"
#include <android/log.h>

#include <jni.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#define LOGI(...)		((void)__android_log_print(ANDROID_LOG_INFO		, "threaded_app", __VA_ARGS__))
#define LOGE(...)		((void)__android_log_print(ANDROID_LOG_ERROR	, "threaded_app", __VA_ARGS__))

// For debug builds, always enable the debug traces in this library
#ifndef NDEBUG
#  define LOGV(...)		((void)__android_log_print(ANDROID_LOG_VERBOSE	, "threaded_app", __VA_ARGS__))
#else
#  define LOGV(...)		((void)0)
#endif

static	void								free_saved_state		(struct glue* _glue)										{
    pthread_mutex_lock				(&_glue->detail.mutex);
    if (_glue->savedState != NULL) {
        free							(_glue->savedState);
        _glue->savedState							= NULL;
        _glue->savedStateSize						= 0;
    }
    pthread_mutex_unlock			(&_glue->detail.mutex);
}

		int8_t								glue_read_cmd			(struct glue* _glue)										{
    int8_t											cmd;
    if (read(_glue->detail.msgread, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        switch (cmd) {
        case APP_CMD_SAVE_STATE:
            free_saved_state				(_glue);
            break;
        }
        return cmd;
    } else {
        LOGE("No data on command pipe!");
    }
    return -1;
}

static	void								print_cur_config		(struct glue* _glue)										{
	char											lang	[2]
		,											country	[2]
		;
	AConfiguration_getLanguage		(_glue->config, lang);
	AConfiguration_getCountry		(_glue->config, country);

    LOGV(	"Config: mcc=%d mnc=%d lang=%c%c cnt=%c%c orien=%d touch=%d dens=%d "
        	"keys=%d nav=%d keysHid=%d navHid=%d sdk=%d size=%d long=%d "
        	"modetype=%d modenight=%d"
        ,	AConfiguration_getMcc			(_glue->config)
        ,	AConfiguration_getMnc			(_glue->config)
        ,	lang	[0], lang		[1]
		,	country	[0], country	[1]
        ,	AConfiguration_getOrientation	(_glue->config)
        ,	AConfiguration_getTouchscreen	(_glue->config)
        ,	AConfiguration_getDensity		(_glue->config)
        ,	AConfiguration_getKeyboard		(_glue->config)
        ,	AConfiguration_getNavigation	(_glue->config)
        ,	AConfiguration_getKeysHidden	(_glue->config)
        ,	AConfiguration_getNavHidden		(_glue->config)
        ,	AConfiguration_getSdkVersion	(_glue->config)
        ,	AConfiguration_getScreenSize	(_glue->config)
        ,	AConfiguration_getScreenLong	(_glue->config)
        ,	AConfiguration_getUiModeType	(_glue->config)
        ,	AConfiguration_getUiModeNight	(_glue->config)
		);
}

		void								glue_pre_exec_cmd		(struct glue* _glue, int8_t cmd)							{
    switch (cmd) {
    case APP_CMD_INPUT_CHANGED:
        LOGV("APP_CMD_INPUT_CHANGED\n");
        pthread_mutex_lock				(&_glue->detail.mutex);
        if (NULL != _glue->inputQueue) 
            AInputQueue_detachLooper		(_glue->inputQueue);

        _glue->inputQueue							= _glue->detail.pendingInputQueue;
        if (NULL != _glue->inputQueue) {
            LOGV("Attaching input queue to looper");
            AInputQueue_attachLooper		(_glue->inputQueue, _glue->looper, LOOPER_ID_INPUT, NULL, &_glue->detail.inputPollSource);
        }
        pthread_cond_broadcast			(&_glue->detail.cond);
        pthread_mutex_unlock			(&_glue->detail.mutex);
        break;

    case APP_CMD_INIT_WINDOW:
        LOGV("APP_CMD_INIT_WINDOW\n");
        pthread_mutex_lock				(&_glue->detail.mutex);
        _glue->window								= _glue->detail.pendingWindow;
        pthread_cond_broadcast			(&_glue->detail.cond);
        pthread_mutex_unlock			(&_glue->detail.mutex);
        break;

    case APP_CMD_TERM_WINDOW:
        LOGV("APP_CMD_TERM_WINDOW\n");
        pthread_cond_broadcast			(&_glue->detail.cond);
        break;

    case APP_CMD_RESUME:
    case APP_CMD_START:
    case APP_CMD_PAUSE:
    case APP_CMD_STOP:
        LOGV("activityState=%d\n", cmd);
        pthread_mutex_lock				(&_glue->detail.mutex);
        _glue->activityState						= cmd;
        pthread_cond_broadcast			(&_glue->detail.cond);
        pthread_mutex_unlock			(&_glue->detail.mutex);
        break;

    case APP_CMD_CONFIG_CHANGED:
        LOGV("APP_CMD_CONFIG_CHANGED\n");
        AConfiguration_fromAssetManager	(_glue->config, _glue->activity->assetManager);
        print_cur_config				(_glue);
        break;

    case APP_CMD_DESTROY:
        LOGV("APP_CMD_DESTROY\n");
        _glue->destroyRequested						= 1;
        break;
    }
}

		void								glue_post_exec_cmd		(struct glue* _glue, int8_t cmd)							{
    switch (cmd) {
    case APP_CMD_TERM_WINDOW:
        LOGV("APP_CMD_TERM_WINDOW\n");
        pthread_mutex_lock				(&_glue->detail.mutex);
        _glue->window								= NULL;
        pthread_cond_broadcast			(&_glue->detail.cond);
        pthread_mutex_unlock			(&_glue->detail.mutex);
        break;

    case APP_CMD_SAVE_STATE:
        LOGV("APP_CMD_SAVE_STATE\n");
        pthread_mutex_lock				(&_glue->detail.mutex);
        _glue->detail.stateSaved					= 1;
        pthread_cond_broadcast			(&_glue->detail.cond);
        pthread_mutex_unlock			(&_glue->detail.mutex);
        break;

    case APP_CMD_RESUME:
        free_saved_state				(_glue);
        break;
    }
}

static	void								glue_destroy			(struct glue* _glue)										{
    LOGV("glue_destroy!");
    free_saved_state				(_glue);
    pthread_mutex_lock				(&_glue->detail.mutex);
    if (_glue->inputQueue != NULL)
        AInputQueue_detachLooper		(_glue->inputQueue);

	AConfiguration_delete			(_glue->config);
    _glue->detail.destroyed						= 1;
    pthread_cond_broadcast			(&_glue->detail.cond);
    pthread_mutex_unlock			(&_glue->detail.mutex);
    // Can't touch glue object after this.
}

static	void								process_input			(struct glue* app, struct android_poll_source* source)		{
    AInputEvent										* event					= NULL;
    while (AInputQueue_getEvent(app->inputQueue, &event) >= 0) {
        LOGV("New input event: type=%d\n", AInputEvent_getType(event));
        if (AInputQueue_preDispatchEvent(app->inputQueue, event))
            continue;

        int32_t											handled						= 0;
        if (app->onInputEvent != NULL) 
			handled										= app->onInputEvent(app, event);
        AInputQueue_finishEvent(app->inputQueue, event, handled);
    }
}

static	void								process_cmd				(struct glue* app, struct android_poll_source* source)		{
    int8_t											cmd						= glue_read_cmd(app);
    glue_pre_exec_cmd	(app, cmd);
    if (app->onAppCmd != NULL) 
		app->onAppCmd(app, cmd);
    glue_post_exec_cmd	(app, cmd);
}

static	void*								glue_entry				(void* param)												{
    struct glue										* _glue					= (struct glue*)param;

    _glue->config								= AConfiguration_new();
    AConfiguration_fromAssetManager	(_glue->config, _glue->activity->assetManager);

    print_cur_config				(_glue);

    _glue->detail.cmdPollSource.id				= LOOPER_ID_MAIN;
    _glue->detail.cmdPollSource.app				= _glue;
    _glue->detail.cmdPollSource.process			= process_cmd;
    _glue->detail.inputPollSource.id			= LOOPER_ID_INPUT;
    _glue->detail.inputPollSource.app			= _glue;
    _glue->detail.inputPollSource.process		= process_input;

    ALooper											* looper				= ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd					(looper, _glue->detail.msgread, LOOPER_ID_MAIN, ALOOPER_EVENT_INPUT, NULL, &_glue->detail.cmdPollSource);
    _glue->looper								= looper;

    pthread_mutex_lock				(&_glue->detail.mutex);
    _glue->detail.running						= 1;
    pthread_cond_broadcast			(&_glue->detail.cond);
    pthread_mutex_unlock			(&_glue->detail.mutex);

    nwol_main						(_glue);

    glue_destroy					(_glue);
    return NULL;
}

typedef	ANativeActivity						ANA;

// Native activity interaction (called from main thread)
static	struct glue*						glue_create				(ANA* activity, void* savedState, size_t savedStateSize)	{
    struct glue										* _glue					= (struct glue*)malloc(sizeof(struct glue));
    memset(_glue, 0, sizeof(struct glue));
    _glue->activity								= activity;

    pthread_mutex_init				(&_glue->detail.mutex, NULL);
    pthread_cond_init				(&_glue->detail.cond, NULL);

    if (savedState != NULL) {
        _glue->savedState							= malloc(savedStateSize);
        _glue->savedStateSize						= savedStateSize;
        memcpy							(_glue->savedState, savedState, savedStateSize);
    }

	int												msgpipe[2];
    if (pipe(msgpipe)) {
        LOGE("could not create pipe: %s", strerror(errno));
        return NULL;
    }
    _glue->detail.msgread						= msgpipe[0];
    _glue->detail.msgwrite						= msgpipe[1];

    pthread_attr_t									attr; 
    pthread_attr_init			(&attr);
    pthread_attr_setdetachstate	(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create					(&_glue->detail.thread, &attr, glue_entry, _glue);

    // Wait for thread to start.
    pthread_mutex_lock				(&_glue->detail.mutex);
    while (!_glue->detail.running)
        pthread_cond_wait				(&_glue->detail.cond, &_glue->detail.mutex);

	pthread_mutex_unlock			(&_glue->detail.mutex);

    return _glue;
}

static	void								glue_write_cmd			(struct glue* _glue, int8_t cmd)							{
    if (write(_glue->detail.msgwrite, &cmd, sizeof(cmd)) != sizeof(cmd))
        LOGE("Failure writing glue cmd: %s\n", strerror(errno));
}

static	void								glue_set_input			(struct glue* _glue, AInputQueue* inputQueue)				{
    pthread_mutex_lock				(&_glue->detail.mutex);
    _glue->detail.pendingInputQueue				= inputQueue;
    glue_write_cmd					(_glue, APP_CMD_INPUT_CHANGED);
    while (_glue->inputQueue != _glue->detail.pendingInputQueue)
        pthread_cond_wait				(&_glue->detail.cond, &_glue->detail.mutex);

	pthread_mutex_unlock			(&_glue->detail.mutex);
}

static	void								glue_set_window			(struct glue* _glue, ANativeWindow* window)					{
    pthread_mutex_lock				(&_glue->detail.mutex);
    if (NULL != _glue->detail.pendingWindow)
        glue_write_cmd					(_glue, APP_CMD_TERM_WINDOW);

	_glue->detail.pendingWindow					= window;
    if (NULL != window) 
        glue_write_cmd					(_glue, APP_CMD_INIT_WINDOW);

	while (_glue->window != _glue->detail.pendingWindow)
        pthread_cond_wait				(&_glue->detail.cond, &_glue->detail.mutex);

    pthread_mutex_unlock			(&_glue->detail.mutex);
}

static	void								glue_set_activity_state	(struct glue* _glue, int8_t cmd)							{
    pthread_mutex_lock				(&_glue->detail.mutex);
    glue_write_cmd				(_glue, cmd);
    while (_glue->activityState != cmd) 
        pthread_cond_wait				(&_glue->detail.cond, &_glue->detail.mutex);

	pthread_mutex_unlock			(&_glue->detail.mutex);
}

static	void								glue_free				(struct glue* _glue)										{
    pthread_mutex_lock				(&_glue->detail.mutex);
    glue_write_cmd					(_glue, APP_CMD_DESTROY);
    while (!_glue->detail.destroyed) 
        pthread_cond_wait				(&_glue->detail.cond, &_glue->detail.mutex);

	pthread_mutex_unlock			(&_glue->detail.mutex);

	close							(_glue->detail.msgread);
    close							(_glue->detail.msgwrite);
    pthread_cond_destroy			(&_glue->detail.cond);
    pthread_mutex_destroy			(&_glue->detail.mutex);
    free							(_glue);
}

static	void*								onSaveInstanceState			(ANativeActivity* activity, size_t* outLen)				{
    struct glue										* _glue						= (struct glue*)activity->instance;
    void											* savedState				= NULL;

    LOGV("SaveInstanceState: %p\n", activity);
    pthread_mutex_lock				(&_glue->detail.mutex);
    _glue->detail.stateSaved					= 0;
    glue_write_cmd					(_glue, APP_CMD_SAVE_STATE);
    while (!_glue->detail.stateSaved)
        pthread_cond_wait				(&_glue->detail.cond, &_glue->detail.mutex);

    if (_glue->savedState != NULL) {
        savedState									= _glue->savedState;
        *outLen										= _glue->savedStateSize;
        _glue->savedState							= NULL;
        _glue->savedStateSize						= 0;
    }

    pthread_mutex_unlock			(&_glue->detail.mutex);

    return savedState;
}

		void								app_dummy					()														{}

static	void								onDestroy					(ANativeActivity* act)									{ LOGV("Destroy         : %p\n"			, act			); glue_free				((struct glue*)act->instance);														}
static	void								onStart						(ANativeActivity* act)									{ LOGV("Start           : %p\n"			, act			); glue_set_activity_state	((struct glue*)act->instance, APP_CMD_START);										}
static	void								onResume					(ANativeActivity* act)									{ LOGV("Resume          : %p\n"			, act			); glue_set_activity_state	((struct glue*)act->instance, APP_CMD_RESUME);										}
static	void								onPause						(ANativeActivity* act)									{ LOGV("Pause           : %p\n"			, act			); glue_set_activity_state	((struct glue*)act->instance, APP_CMD_PAUSE);										}
static	void								onStop						(ANativeActivity* act)									{ LOGV("Stop            : %p\n"			, act			); glue_set_activity_state	((struct glue*)act->instance, APP_CMD_STOP);										}
static	void								onConfigurationChanged		(ANativeActivity* act)									{ LOGV("CfgChanged      : %p\n"			, act			); glue_write_cmd			((struct glue*)act->instance, APP_CMD_CONFIG_CHANGED);								}
static	void								onLowMemory					(ANativeActivity* act)									{ LOGV("LowMem          : %p\n"			, act			); glue_write_cmd			((struct glue*)act->instance, APP_CMD_LOW_MEMORY);									}
static	void								onWindowFocusChanged		(ANativeActivity* act, int focus)						{ LOGV("FocusChanged    : %p -- %d\n"	, act, focus	); glue_write_cmd			((struct glue*)act->instance, focus ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);	}
static	void								onNativeWindowCreated		(ANativeActivity* act, ANativeWindow* window)			{ LOGV("WindowCreated   : %p -- %p\n"	, act, window	); glue_set_window			((struct glue*)act->instance, window);												}
static	void								onNativeWindowDestroyed		(ANativeActivity* act, ANativeWindow* window)			{ LOGV("WindowDestroyed : %p -- %p\n"	, act, window	); glue_set_window			((struct glue*)act->instance, NULL);												}
static	void								onInputQueueCreated			(ANativeActivity* act, AInputQueue* queue)				{ LOGV("InputCreated    : %p -- %p\n"	, act, queue	); glue_set_input			((struct glue*)act->instance, queue);												}
static	void								onInputQueueDestroyed		(ANativeActivity* act, AInputQueue* queue)				{ LOGV("InputDestroyed  : %p -- %p\n"	, act, queue	); glue_set_input			((struct glue*)act->instance, NULL);												}

		void								ANativeActivity_onCreate	(ANativeActivity* act, void* savedState, size_t savedStateSize)		{
    LOGV("Creating: %p\n", act);
    act->callbacks->onDestroy					= onDestroy					;
    act->callbacks->onStart						= onStart					;
    act->callbacks->onResume					= onResume					;
    act->callbacks->onSaveInstanceState			= onSaveInstanceState		;
    act->callbacks->onPause						= onPause					;
    act->callbacks->onStop						= onStop					;
    act->callbacks->onConfigurationChanged		= onConfigurationChanged	;
    act->callbacks->onLowMemory					= onLowMemory				;
    act->callbacks->onWindowFocusChanged		= onWindowFocusChanged		;
    act->callbacks->onNativeWindowCreated		= onNativeWindowCreated		;
    act->callbacks->onNativeWindowDestroyed		= onNativeWindowDestroyed	;
    act->callbacks->onInputQueueCreated			= onInputQueueCreated		;
    act->callbacks->onInputQueueDestroyed		= onInputQueueDestroyed		;

    act->instance								= glue_create(act, savedState, savedStateSize);
}
