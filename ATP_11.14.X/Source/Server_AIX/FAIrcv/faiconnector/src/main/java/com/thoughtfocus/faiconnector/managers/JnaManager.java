package com.thoughtfocus.faiconnector.managers;

import com.thoughtfocus.faiconnector.jna.FaircvNativeAccess;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public class JnaManager {
	// Native Access Facade
	private static JnaManager instance;
	
	public static JnaManager getInstance() {
		if (instance == null) {
			instance = new JnaManager();
		}
		return instance;
	}
	
	public int initialize(String appName, String serviceName) {
		return FaircvNativeAccess.INSTANCE.FAIrcvHandler_initialize(appName, serviceName);
	}

	public synchronized void sendResponseToXcom(String que_name, String nType, String nSubType1, String nSubType2,
			String nAppDataType, String p_data, int nDataSize) {
		if (p_data.length() >= 84) {
		    log.info("FaircvRT: Sending response to xcom: " + p_data.substring(53, 84));
		} else {
		    log.warn("FaircvRT: Sending response to xcom: " + p_data);
		}
		FaircvNativeAccess.INSTANCE.Faircv_read_response_from_FAI(que_name, nType, nSubType1, nSubType2, nAppDataType,
				p_data, nDataSize);
	}

	public void terminate() {
		FaircvNativeAccess.INSTANCE.FAIrcvHandler_terminate();
	}
}