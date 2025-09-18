package com.thoughtfocus.faiconnector.jna;

import static com.thoughtfocus.faiconnector.utils.Constants.FaisndNativeAccess.*;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;

public interface FaisndNativeAccess extends Library {
	FaisndNativeAccess INSTANCE = Native.load(getNativeAccessLibraryName(), FaisndNativeAccess.class);
	int FAIsndHandler_initialize(String appName, String serviceName);
	String FAIsndHandler_sendRequestMessage(String serviceName);
	void FAIsndHandler_terminate();
	void faisnd_Send_Request_To_Service(String que_name, String nType, String nSubType1, 
			String nSubType2, String nAppDataType, 
			String p_data, int nDataSize);
	
	static String getNativeAccessLibraryName() {
		if (Platform.isWindows()) {
			return FAISNDHANDLER_DLL;
		} else if (Platform.isAIX()) {
			return LIBFAISNDHANDLER_SO;
		} else if (Platform.isMac()) {
			return LIBFAISNDHANDLER_DYLIB;
		} else {
			return LIBFAISNDHANDLER_SO;
		}
	}
}
