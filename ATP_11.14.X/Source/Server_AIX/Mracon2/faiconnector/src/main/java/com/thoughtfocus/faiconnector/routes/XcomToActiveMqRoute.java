package com.thoughtfocus.faiconnector.routes;

import org.apache.camel.Exchange;
import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import com.thoughtfocus.faiconnector.state.State;

import lombok.extern.slf4j.Slf4j;

@Component
@Slf4j
public class XcomToActiveMqRoute extends RouteBuilder {

	private final String activeMqInputQueueName;
	private String message = "";

	public XcomToActiveMqRoute(@Value("${fai.input-queue-name}") String activeMqInputQueueName,
			@Value("${mracon.message.length}") int messageLength) {
		this.activeMqInputQueueName = "activemq:" + activeMqInputQueueName + "?exchangePattern=InOnly";
		log.debug("AppName: {}; ServiceName: {}; Reading from: {}", State.CURRENT.getAppName(),
				State.CURRENT.getServiceName(), activeMqInputQueueName);
		State.CURRENT.setMessageLength(messageLength);
	}

	@Override
	public void configure() throws Exception {
		onException(Exception.class).handled(true).process(this::processException);
		from("direct:xcom")
			.process(this::processMessage)
			.to(this.activeMqInputQueueName);
	}

	private void processException(Exchange exchange) {
		Exception exception = exchange.getProperty(Exchange.EXCEPTION_CAUGHT, Exception.class);
		log.error("Exception: {}", exception.getMessage());

		if (message != null && !message.isEmpty() && message.length() > 500) {
			log.debug("Message Received Also Sending To Archive File ======> {}", message);
			BossCommunicationManager.instance().write(message);
		} else {
			log.warn("Empty message or message with less than 500 bytes received, cannot be logged into archive file.");
		}
	}

	private void processMessage(Exchange exchange) {
		message = exchange.getIn().getBody(String.class);
		log.info("Sending to ApacheMQ: {}", message);
		exchange.getIn().setBody(message);
	}
}