package com.thoughtfocus.faiconnector.managers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import com.thoughtfocus.faiconnector.common.Application;
import com.thoughtfocus.faiconnector.state.State;
import static com.thoughtfocus.faiconnector.utils.Constants.XcomManager.*;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@Component
public class XcomManager {
	private JnaManager jnaManager;
	private Application application;
	
	public XcomManager(@Autowired Application app) {
		this.application = app;
		jnaManager = JnaManager.getInstance();
		log.info("Starting {}-{}", State.CURRENT.getAppName(), State.CURRENT.getServiceName());
	}
	
	public synchronized void initialize() {
		int initializationResult = INDETERMINATE_STATE;
		if (State.CURRENT.getXcomInitialized() == false) {
			initializationResult = jnaManager.initialize(
					State.CURRENT.getAppName(), 
					State.CURRENT.getServiceName());
			
			switch(initializationResult) {
			case QUEUE_CREATION_FAILED:
				processQueueCreationFailed();
				break;
			case APPNAME_OR_SERVICENAME_IS_NULL:
				processNullAppOrServiceNames();
				break;
			case QUEUES_INITIALIZED_SUCCESSFULLY:
				processQueuesInitializedSuccessfully();
				break;
			default:
				exitApp();
				break;
			}
		}
	}
	
	public void writeToXcom(String que_name, String nType, String nSubType1, 
			String nSubType2, String nAppDataType, 
			String p_data, int nDataSize) {
			jnaManager.sendResponseToXcom(que_name, nType, nSubType1, nSubType2, nAppDataType, p_data, nDataSize);
	}
	
	public synchronized void terminate() {
		log.info("Termination signal received from Xcom. Shutting down!");
		jnaManager.terminate();
		application.initiateShutdown(TERMINATION_SIGNAL_RECEIVED);
	}
	
	private void processQueueCreationFailed() {
		log.error("Xcom Queue creation failed with AppName: '{}' & ServiceName: '{}'. Shutting down!", State.CURRENT.getAppName(), 
				State.CURRENT.getServiceName());
		jnaManager.terminate();
		application.initiateShutdown(QUEUE_CREATION_FAILED);
	}
	
	private void processNullAppOrServiceNames() {
		log.error("NULL AppName: '{}' & ServiceName: '{}'. Shutting down!", State.CURRENT.getAppName(), 
				State.CURRENT.getServiceName());
		jnaManager.terminate();
		application.initiateShutdown(APPNAME_OR_SERVICENAME_IS_NULL);
	}
	
	private void processQueuesInitializedSuccessfully() {
		log.info("Xcom Queues initialized successfully!");
		State.CURRENT.setXcomInitialized(true);
	}
	
	private void exitApp() {
		log.error("Xcom Queues were not initialized correctly. Shutting down!");
		application.initiateShutdown(UNKNOWN_ERROR);
	}
}
