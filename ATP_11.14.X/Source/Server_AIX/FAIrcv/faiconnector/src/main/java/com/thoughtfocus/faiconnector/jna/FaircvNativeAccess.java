package com.thoughtfocus.faiconnector.jna;

import static com.thoughtfocus.faiconnector.utils.Constants.FaircvNativeAccess.*;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;

public interface FaircvNativeAccess extends Library {
	FaircvNativeAccess INSTANCE = Native.load(getNativeAccessLibraryName(), FaircvNativeAccess.class);
	int FAIrcvHandler_initialize(String appName, String serviceName);
	void FAIrcvHandler_terminate();
	void Faircv_read_response_from_FAI(String que_name, String nType, String nSubType1, 
			String nSubType2, String nAppDataType, 
			String p_data, int nDataSize);
	
	static String getNativeAccessLibraryName() {
		if (Platform.isWindows()) {
			return FAIRCVHANDLER_DLL;
		} else if (Platform.isAIX()) {
			return FAIRCVHANDLER_SO;
		} else if (Platform.isMac()) {
			return FAIRCVHANDLER_DYLIB;
		} else {
			return FAIRCVHANDLER_SO;
		}
	}
}
