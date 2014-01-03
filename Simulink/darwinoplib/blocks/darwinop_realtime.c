#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME darwinop_realtime

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"

/*
 *  Include the standard ANSI C header for handling time functions:
 *  ---------------------------------------------------------------
 */
#include <time.h>

static void mdlInitializeSizes(SimStruct *S)
{
   ssSetOptions(SS_OPTION_EXCEPTION_FREE_CODE | SS_OPTION_RUNTIME_EXCEPTION_FREE_CODE |
           SS_OPTION_PLACE_ASAP | SS_OPTION_SFUNCTION_INLINED_FOR_RTW );

   ssSetNumSFcnParams(S, 1);  /* Number of expected parameters */

   if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;

   ssSetNumContStates(S, 0);
   ssSetNumDiscStates(S, 0);

   if (!ssSetNumInputPorts(S, 0)) return;

   if (!ssSetNumOutputPorts(S, 0)) return;
   
   ssSetNumSampleTimes(S, 1);
   ssSetNumRWork(S, 2);
   ssSetNumIWork(S, 0);
   ssSetNumPWork(S, 0);
   ssSetNumModes(S, 0);
   ssSetNumNonsampledZCs(S, 0);
}

#define MDL_INITIALIZE_SAMPLE_TIMES
static void mdlInitializeSampleTimes(SimStruct *S)
{
   ssSetSampleTime(S, 0, -1.0);
   ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_START
static void mdlStart(SimStruct *S)
{
    ssSetRWorkValue(S,0,ssGetTStart(S));
    ssSetRWorkValue(S,1,(real_T) clock() / CLOCKS_PER_SEC);
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
   real_T StartTime = ssGetRWorkValue(S,0);
   real_T StartClock = ssGetRWorkValue(S,1);
   real_T *ScaleFactor = mxGetPr(ssGetSFcnParam(S,0));
   time_T SimTime = ssGetT(S);
   real_T StepClock;
   real_T CurrentClock;
   
   /* Desired Delta time */
   StepClock = ((real_T)SimTime - StartTime) * ScaleFactor[0] + StartClock;

   /* Wait to reach the desired time */
   do {
        CurrentClock = (real_T) clock()/CLOCKS_PER_SEC;
        /* Look for wrapup */
        if (CurrentClock < StartTime){
            const DELTA = pow(2.0,sizeof(clock_t)*8.0) / CLOCKS_PER_SEC;
            StartTime -= DELTA;
            StepClock -= DELTA;
            ssSetRWorkValue(S,1,StartTime);
        }
   } while (CurrentClock < StepClock);
}

static void mdlTerminate(SimStruct *S)
{
    UNUSED_ARG(S); /* unused input argument */
}

/*
 *  Required S-function trailer:
 *  ----------------------------
 */
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
