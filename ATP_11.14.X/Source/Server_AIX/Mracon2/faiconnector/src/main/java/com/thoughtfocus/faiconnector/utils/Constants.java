package com.thoughtfocus.faiconnector.utils;

import lombok.AccessLevel;
import lombok.NoArgsConstructor;

public class Constants {
    public static final String FALCON = "falcon";
    public static final String MRACON = "mracon";
    
    @NoArgsConstructor(access = AccessLevel.PRIVATE)
    public static class ProcessorBase {
    	public static final String ERROR_TIMED_OUT_ON_RECEIVE_MESSAGE = "Error:Timed Out on Receive,  error code:";
    	public static final String EXIT_MESSAGE = "1";
    	public static final String ERROR_ON_PTEIPC_RECEIVE_OR_ERROR_IN_RECEVING_MESSAGE = "Error on pteipc_receive OR Error in Receving Message";
    }
    
    @NoArgsConstructor(access = AccessLevel.PRIVATE)
    public static class XcomManager {
    	public static final int INDETERMINATE_STATE = 01;
    	public static final int QUEUE_CREATION_FAILED = 01;
    	public static final int APPNAME_OR_SERVICENAME_IS_NULL = 02;
    	public static final int QUEUES_INITIALIZED_SUCCESSFULLY = 00;
    	public static final int UNKNOWN_ERROR = -2;
    	public static final int TERMINATION_SIGNAL_RECEIVED = 03;
    }
    
    @NoArgsConstructor(access = AccessLevel.PRIVATE)
    public static class FalconNativeAccess {
        public static final String FALCONHANDLER_DLL = "FalconHandler.dll";
        public static final String LIBFALCONHANDLER_SO = "libFalconHandler.so";
        public static final String LIBFALCONHANDLER_DYLIB = "libFalconHandler.dylib";
    }
    
    @NoArgsConstructor(access = AccessLevel.PRIVATE)
    public static class MraconNativeAccess {
        public static final String MRACONHANDLER_DLL = "MraconHandler.dll";
        public static final String LIBMRACONHANDLER_SO = "libMraconHandler.so";
        public static final String LIBMRACONHANDLER_DYLIB = "LIBMRACONHANDLER.DYLIB";
    }
}
