 // Copyright (C) 2010 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include "./glue/glue.h"

#define LOGI(...)	((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...)	((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#define LOG_ACCELEROMETER false

// Our saved state data.
struct saved_state {
    float										angle;
    int32_t										x;
    int32_t										y;
};

// Shared state for our app.
struct engine {
    struct glue									* app;

	struct engine_detail_android {
		ASensorManager								* sensorManager;
		const ASensor								* accelerometerSensor;
		ASensorEventQueue							* sensorEventQueue;
	}											detailAndroid;

	struct engine_detail_egl {
		EGLDisplay									display;
		EGLSurface									surface;
		EGLContext									context;
	}											detailEGL;
    int											animating;
    int32_t										width;
    int32_t										height;
    struct saved_state							state;
};

// Initialize an EGL context for the current display.
static int									engine_init_display			(struct engine* _engine)					{
	// initialize OpenGL ES and EGL

	// Here specify the attributes of the desired configuration.
	// Below, we select an EGLConfig with at least 8 bits per color
	// component compatible with on-screen windows
    const EGLint									attribs[]					= 
		{	EGL_SURFACE_TYPE, EGL_WINDOW_BIT
		,	EGL_BLUE_SIZE	, 8
		,	EGL_GREEN_SIZE	, 8
		,	EGL_RED_SIZE	, 8
		,	EGL_NONE
		};
    EGLint											w							= 0
		,											h							= 0
		,											dummy						= 0
		,											format						= 0
    	,											numConfigs					= 0;
		;
    EGLConfig										config						= NULL;
    EGLSurface										surface						= NULL;
    EGLContext										context						= NULL;

    EGLDisplay										display						= eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize		(display, 0, 0);

    // Here, the application chooses the configuration it desires. In this sample, we have a very simplified selection process, where we pick the first EGLConfig that matches our criteria.
    eglChooseConfig		(display, attribs, &config, 1, &numConfigs);

    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    // As soon as we picked a EGLConfig, we can safely reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    eglGetConfigAttrib	(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(_engine->app->window, 0, 0, format);

    surface										= eglCreateWindowSurface	(display, config, _engine->app->window, NULL);
    context										= eglCreateContext			(display, config, NULL, NULL);

    if (eglMakeCurrent	(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface		(display, surface, EGL_WIDTH	, &w);
    eglQuerySurface		(display, surface, EGL_HEIGHT, &h);

    _engine->detailEGL.display					= display;
    _engine->detailEGL.context					= context;
    _engine->detailEGL.surface					= surface;
    _engine->width								= w;
    _engine->height								= h;
    _engine->state.angle						= 0;

    // Initialize GL state.
    glHint			(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable		(GL_CULL_FACE	);
    glShadeModel	(GL_SMOOTH		);
    glDisable		(GL_DEPTH_TEST	);

    return 0;
}

// Just the current frame in the display.
static void									engine_draw_frame			(struct engine* _engine)					{
    if(NULL == _engine->detailEGL.display)
        return;	 // No display.

    // Just fill the screen with a color.
    glClearColor	(((float)_engine->state.x)/_engine->width, _engine->state.angle, ((float)_engine->state.y)/_engine->height, 1);
    glClear		(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers		(_engine->detailEGL.display, _engine->detailEGL.surface);
}

// Tear down the EGL context currently associated with the display.
static void									engine_term_display			(struct engine* _engine)					{
    if (_engine->detailEGL.display != EGL_NO_DISPLAY) {
        eglMakeCurrent	(_engine->detailEGL.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (_engine->detailEGL.context != EGL_NO_CONTEXT)	eglDestroyContext(_engine->detailEGL.display, _engine->detailEGL.context);
        if (_engine->detailEGL.surface != EGL_NO_SURFACE)	eglDestroySurface(_engine->detailEGL.display, _engine->detailEGL.surface);
        eglTerminate	(_engine->detailEGL.display);
    }
    _engine->animating							= 0;
    _engine->detailEGL.display					= EGL_NO_DISPLAY;
    _engine->detailEGL.context					= EGL_NO_CONTEXT;
    _engine->detailEGL.surface					= EGL_NO_SURFACE;
}

// Process the next input event.
static int32_t								engine_handle_input			(struct glue* app, AInputEvent* _event)		{
    struct engine									* _engine					= (struct engine*)app->userData;
    if (AInputEvent_getType(_event) == AINPUT_EVENT_TYPE_MOTION) {
        _engine->animating							= 1;
        _engine->state.x							= AMotionEvent_getX(_event, 0);
        _engine->state.y							= AMotionEvent_getY(_event, 0);
        return 1;
    }
    return 0;
}

// Process the next main command.
static void									engine_handle_cmd			(struct glue* app, int32_t cmd)				{
    struct engine									* _engine					= (struct engine*)app->userData;
	struct saved_state								* savedState				= (struct saved_state*)_engine->app->savedState;
    switch (cmd) {
    case APP_CMD_SAVE_STATE:	// The system has asked us to save our current state. Do so.
        _engine->app->savedState					= malloc(sizeof(struct saved_state));
        *savedState									= _engine->state;
        _engine->app->savedStateSize				= sizeof(struct saved_state);
        break;
    case APP_CMD_INIT_WINDOW:	// The window is being shown, get it ready.
        if (_engine->app->window != NULL) {
            engine_init_display	(_engine);
            engine_draw_frame	(_engine);
        }
        break;
    case APP_CMD_TERM_WINDOW:	// The window is being hidden or closed, clean it up.
        engine_term_display	(_engine);
        break;
    case APP_CMD_GAINED_FOCUS:	// When our app gains focus, we start monitoring the accelerometer.
        if (_engine->detailAndroid.accelerometerSensor != NULL) {	
            ASensorEventQueue_enableSensor(_engine->detailAndroid.sensorEventQueue, _engine->detailAndroid.accelerometerSensor);
            ASensorEventQueue_setEventRate(_engine->detailAndroid.sensorEventQueue, _engine->detailAndroid.accelerometerSensor, (1000L/60)*1000);	// We'd like to get 60 events per second (in us).
        }
        break;
    case APP_CMD_LOST_FOCUS:
        // When our app loses focus, we stop monitoring the accelerometer.
        // This is to avoid consuming battery while not being used.
        if (_engine->detailAndroid.accelerometerSensor != NULL)
            ASensorEventQueue_disableSensor(_engine->detailAndroid.sensorEventQueue, _engine->detailAndroid.accelerometerSensor);

		// Also stop animating.
        _engine->animating							= 0;
        engine_draw_frame(_engine);
        break;
    }
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void										nwol_main					(struct glue* state)					{
    struct engine									_engine;
    // Make sure glue isn't stripped.
    app_dummy();

    memset(&_engine, 0, sizeof(engine));
    state->userData								= &_engine;
    state->onAppCmd								= engine_handle_cmd;
    state->onInputEvent							= engine_handle_input;
    _engine.app									= state;

    // Prepare to monitor accelerometer
    _engine.detailAndroid.sensorManager			= ASensorManager_getInstance();
    _engine.detailAndroid.accelerometerSensor	= ASensorManager_getDefaultSensor(_engine.detailAndroid.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    _engine.detailAndroid.sensorEventQueue		= ASensorManager_createEventQueue(_engine.detailAndroid.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

    if (state->savedState != NULL)
        _engine.state								= *(struct saved_state*)state->savedState;	// We are starting with a previous saved state; restore from it.

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int												ident						= 0;
        int												events						= 0;
        struct android_poll_source						* source					= NULL;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(_engine.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {

            if (source != NULL)	
                source->process(state, source);	// Process this event.

			if (ident == LOOPER_ID_USER) {	// If a sensor has data, process it now.
                if (_engine.detailAndroid.accelerometerSensor != NULL) {
                    ASensorEvent									sensorEvent;
                    while (ASensorEventQueue_getEvents(_engine.detailAndroid.sensorEventQueue, &sensorEvent, 1) > 0) 
                        if (LOG_ACCELEROMETER) {
                            LOGI("accelerometer: x=%f y=%f z=%f"
								, sensorEvent.acceleration.x
								, sensorEvent.acceleration.y
								, sensorEvent.acceleration.z
								);
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&_engine);
                return;
            }
        }

        if (_engine.animating) {		// Done with events; draw next animation frame.
            _engine.state.angle							+= .01f;
            if (_engine.state.angle > 1)			
                _engine.state.angle							= 0;

            engine_draw_frame(&_engine);	// Drawing is throttled to the screen update rate, so there is no need to do timing here.
        }
    }
}
//END_INCLUDE(all)
