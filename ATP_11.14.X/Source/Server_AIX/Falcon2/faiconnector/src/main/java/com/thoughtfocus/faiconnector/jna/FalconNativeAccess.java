package com.thoughtfocus.faiconnector.jna;

import static com.thoughtfocus.faiconnector.utils.Constants.FalconNativeAccess.*;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;

public interface FalconNativeAccess extends Library {
	FalconNativeAccess INSTANCE = Native.load(getNativeAccessLibraryName(), FalconNativeAccess.class);
	int FalconHandler_initialize(String appName, String serviceName);
	String FalconHandler_sendRequestMessage(String serviceName);
//	void FalconHandler_receiveResponseMsg(String responseMessage);
	void FalconHandler_terminate();
//	int FalconHandler_getLibFileVersion();
	void Falcon_Send_Request_To_Service(String que_name, String nType, String nSubType1, 
			String nSubType2, String nAppDataType, 
			String p_data, int nDataSize);
	
	static String getNativeAccessLibraryName() {
		if (Platform.isWindows()) {
			return FALCONHANDLER_DLL;
		} else if (Platform.isAIX()) {
			return LIBFALCONHANDLER_SO;
		} else if (Platform.isMac()) {
			return LIBFALCONHANDLER_DYLIB;
		} else {
			return LIBFALCONHANDLER_SO;
		}
	}
}
