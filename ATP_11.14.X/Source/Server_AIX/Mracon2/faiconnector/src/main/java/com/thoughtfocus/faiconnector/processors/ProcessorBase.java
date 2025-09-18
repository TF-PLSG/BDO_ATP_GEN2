package com.thoughtfocus.faiconnector.processors;

import org.apache.camel.CamelContext;

import com.thoughtfocus.faiconnector.managers.XcomManager;
import com.thoughtfocus.faiconnector.state.State;
import static com.thoughtfocus.faiconnector.utils.Constants.ProcessorBase.*;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public abstract class ProcessorBase {
	protected CamelContext camelContext;
	protected XcomManager xcomManager;

	public ProcessorBase(CamelContext camelContext, XcomManager xcommManager) {
		this.camelContext = camelContext;
		this.xcomManager = xcommManager;
	}

	protected String getMessage() {
		xcomManager.initialize();
		String message = xcomManager.readFromXcom();
		// check for empty message
		if (message == null || message.trim().isEmpty()) {
			return null;
		}

		// check for errors or exit messages
		if (message.contains(ERROR_ON_PTEIPC_RECEIVE_OR_ERROR_IN_RECEVING_MESSAGE)) {
			log.warn("There are no more messages in the queue.");
		} else if (message.equals(EXIT_MESSAGE)) {
			log.info("Exit request received from Xcomm. Terminating application");
			xcomManager.terminate();
		} else if (message.contains(ERROR_TIMED_OUT_ON_RECEIVE_MESSAGE)) {
			log.warn("Timeout error received");
		}
		
		// sanitize message by length
		if (message.length() == State.CURRENT.getMessageLength())
			return message;
		if (message.length() == State.CURRENT.getMessageLength() - 1)
			return message + " ";
		
		// return null if everything above fails
		return null;
	}
}
