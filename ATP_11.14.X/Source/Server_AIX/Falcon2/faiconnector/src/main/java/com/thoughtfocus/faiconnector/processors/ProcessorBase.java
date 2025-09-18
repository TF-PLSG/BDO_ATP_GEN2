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
		if ((message.length() == State.CURRENT.getMessageLength()) || (message.length() == State.CURRENT.getRealTimeMessageLength()))
			return message;
		if ((message.length() == State.CURRENT.getMessageLength() - 1) || (message.length() == State.CURRENT.getRealTimeMessageLength() - 1))
			return message + " ";
		
		log.warn("Message Length = " + message.length() + ", Standard Message Length For Online = " + State.CURRENT.getMessageLength() + ", Standard Message Length For Real-Time = " + State.CURRENT.getRealTimeMessageLength());
		// return null if everything above fails
		return null;
	}
}
