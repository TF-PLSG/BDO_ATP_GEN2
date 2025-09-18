package com.thoughtfocus.faiconnector.managers;

import com.thoughtfocus.faiconnector.jna.FalconNativeAccess;
import com.thoughtfocus.faiconnector.jna.MraconNativeAccess;
import static com.thoughtfocus.faiconnector.utils.Constants.*;

public class JnaManager {
	// Native Access Facade
	private static JnaManager instance;
	private String applicationName;
	
	public JnaManager(String applicationName) {
		this.applicationName = applicationName;
	}
	
	public static JnaManager getInstance(String applicationName) {
		if (instance == null) {
			instance = new JnaManager(applicationName);
		}
		return instance;
	}
	
	public int initialize(String appName, String serviceName) {
		if (applicationName.toLowerCase().equals(FALCON))
			return FalconNativeAccess.INSTANCE.FalconHandler_initialize(appName, serviceName);
		return MraconNativeAccess.INSTANCE.MraconHandler_initialize(appName, serviceName);
	}
	
	public String sendRequestMessage(String serviceName) {
		if (applicationName.toLowerCase().equals(FALCON))
			return FalconNativeAccess.INSTANCE.FalconHandler_sendRequestMessage(serviceName);
		return MraconNativeAccess.INSTANCE.MraconHandler_sendRequestMessage(serviceName);
	}
	
//	public void receiveResponseMsg(String responseMessage) {
//		if (applicationName.toLowerCase().equals("falcon"))
//			FalconNativeAccess.INSTANCE.FalconHandler_receiveResponseMsg(responseMessage);
//		MraconNativeAccess.INSTANCE.MraconHandler_receiveResponseMsg(responseMessage);
//	}
	
	public void terminate() {
		if (applicationName.toLowerCase().equals(FALCON))
			FalconNativeAccess.INSTANCE.FalconHandler_terminate();
		MraconNativeAccess.INSTANCE.MraconHandler_terminate();
	}

//	public int getLibFileVersion() {
//		if (applicationName.toLowerCase().equals("falcon"))
//			return FalconNativeAccess.INSTANCE.FalconHandler_getLibFileVersion();
//		return MraconNativeAccess.INSTANCE.MraconHandler_getLibFileVersion();
//	}
}
