package com.thoughtfocus.faiconnector.managers;

import com.thoughtfocus.faiconnector.jna.FalconNativeAccess;
import com.thoughtfocus.faiconnector.jna.MraconNativeAccess;

import lombok.extern.slf4j.Slf4j;
import static com.thoughtfocus.faiconnector.utils.Constants.*;

@Slf4j
public class JnaManager {
	// Native Access Facade
	private static JnaManager instance;
	private String applicationName;
	
	private JnaManager(String applicationName) {
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
	
	public synchronized String sendRequestMessage(String serviceName) {
		if (applicationName.toLowerCase().equals(FALCON)) {
			return FalconNativeAccess.INSTANCE.FalconHandler_sendRequestMessage(serviceName);
		}
		return MraconNativeAccess.INSTANCE.MraconHandler_sendRequestMessage(serviceName);
	}
	
	public synchronized void sendRequestToService(String que_name, String nType, String nSubType1, 
			String nSubType2, String nAppDataType, 
			String p_data, int nDataSize) {
			log.info("FalconRT: Sending response to xcom: " + p_data.substring(53,84));
			FalconNativeAccess.INSTANCE.Falcon_Send_Request_To_Service(que_name, nType, nSubType1, nSubType2, nAppDataType, p_data, nDataSize);
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