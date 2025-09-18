package com.thoughtfocus.faiconnector.jna;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import static com.thoughtfocus.faiconnector.utils.Constants.MraconNativeAccess.*;

public interface MraconNativeAccess extends Library {
	MraconNativeAccess INSTANCE = Native.load(getNativeAccessLibraryName(), MraconNativeAccess.class);
	int MraconHandler_initialize(String appName, String serviceName);
	String MraconHandler_sendRequestMessage(String serviceName);
//	void MraconHandler_receiveResponseMsg(String responseMessage);
	void MraconHandler_terminate();
//	int MraconHandler_getLibFileVersion();
	
	static String getNativeAccessLibraryName() {
		if (Platform.isWindows()) {
			return MRACONHANDLER_DLL;
		} else if (Platform.isAIX()) {
			return LIBMRACONHANDLER_SO;
		} else if (Platform.isMac()) {
			return LIBMRACONHANDLER_DYLIB;
		} else {
			return LIBMRACONHANDLER_SO;
		}
	}
}
