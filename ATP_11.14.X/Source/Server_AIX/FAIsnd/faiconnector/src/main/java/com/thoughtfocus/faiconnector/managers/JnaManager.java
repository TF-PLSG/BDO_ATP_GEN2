package com.thoughtfocus.faiconnector.managers;

import com.thoughtfocus.faiconnector.jna.FaisndNativeAccess;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public class JnaManager {
	
	private static JnaManager instance;

	public static JnaManager getInstance() {
		if (instance == null) {
			instance = new JnaManager();
		}
		return instance;
	}
	
	public int initialize(String appName, String serviceName) {
		return FaisndNativeAccess.INSTANCE.FAIsndHandler_initialize(appName, serviceName);
	}
	
	public synchronized String sendRequestMessage(String serviceName) {
		return FaisndNativeAccess.INSTANCE.FAIsndHandler_sendRequestMessage(serviceName);
	}
	
	public synchronized void sendRequestToService(String que_name, String nType, String nSubType1, String nSubType2,
			String nAppDataType, String p_data, int nDataSize) {
		log.info("FaisndRT: Sending response to xcom: " + p_data.substring(53, 84));
		FaisndNativeAccess.INSTANCE.faisnd_Send_Request_To_Service(que_name, nType, nSubType1, nSubType2, nAppDataType,
				p_data, nDataSize);
	}

	public void terminate() {
		FaisndNativeAccess.INSTANCE.FAIsndHandler_terminate();
	}
}