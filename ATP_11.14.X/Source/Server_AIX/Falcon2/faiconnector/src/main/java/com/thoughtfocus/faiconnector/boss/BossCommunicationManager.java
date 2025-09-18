package com.thoughtfocus.faiconnector.boss;

import lombok.extern.slf4j.Slf4j;
@Slf4j
public class BossCommunicationManager {
	private CommunicationState currentState;
	private final ApacheHttpClient httpClient;
	private static BossCommunicationManager instance;

	private enum CommunicationState {
		NONE, STX_SENT, ETX_SENT
	}

	/**
	 * Private Constructor for making BossCommunicationManager a singleton
	 */
	private BossCommunicationManager() {
		currentState = CommunicationState.NONE;
		httpClient = new ApacheHttpClient();
	}

	/**
	 * Singleton Instance!
	 * 
	 * @return the current instance
	 */
	public static BossCommunicationManager instance() {
		if (instance == null) {
			instance = new BossCommunicationManager();
		}
		return instance;
	}

	/**
	 * Write archiving transaction data to BOSS
	 * 
	 * @param data
	 *            the transaction data
	 */
	public void write(String data) {
		try {
			if (currentState == CommunicationState.NONE || currentState == CommunicationState.ETX_SENT) {
				httpClient.startOfTransaction();
				currentState = CommunicationState.STX_SENT;
				httpClient.writeToService(data);
			} else {
				httpClient.writeToService(data);
			}
		} catch (Exception e) {
			log.debug(
					"Failed to archive the transaction currentState of message is not matching with communicationState..!!!!");
			log.debug("exception from write() method.." + e);
		}
	}
}
