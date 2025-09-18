package com.thoughtfocus.faiconnector.utils;

import lombok.AccessLevel;
import lombok.NoArgsConstructor;

public class Constants {

    @NoArgsConstructor(access = AccessLevel.PRIVATE)
    public static class ProcessorBase {
    	public static final String ERROR_TIMED_OUT_ON_RECEIVE_MESSAGE = "Error:Timed Out on Receive,  error code:";
    	public static final String EXIT_MESSAGE = "1";
    	public static final String ERROR_ON_PTEIPC_RECEIVE_OR_ERROR_IN_RECEVING_MESSAGE = "Error on pteipc_receive OR Error in Receving Message";
    }
    
    @NoArgsConstructor(access = AccessLevel.PRIVATE)
    public static class ActiveMqRouteToXcom {
    	public static final String QUEUE_NAME = "faircvA";
    	public static final String N_TYPE = "126";
    	public static final String N_SUBTYPE1 = "0";
    	public static final String N_SUBTYPE2 = "0";
    	public static final String N_APP_DATA_TYPE = "0";
    	public static final String EMPTY_MESSAGE = "I";
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
    public static class FaircvNativeAccess {
        public static final String FAIRCVHANDLER_DLL = "FAIrcvHandler.dll";
        public static final String FAIRCVHANDLER_SO = "libFAIrcvHandler.so";
        public static final String FAIRCVHANDLER_DYLIB = "libFAIrcvHandler.dylib";
    }
}
