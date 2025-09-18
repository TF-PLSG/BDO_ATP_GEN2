package com.thoughtfocus.faiconnector.routes;

import org.apache.camel.Exchange;
import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;
import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.*;

import javax.annotation.PostConstruct;

import com.thoughtfocus.faiconnector.managers.XcomManager;
import com.thoughtfocus.faiconnector.state.State;

import lombok.extern.slf4j.Slf4j;

@Component
@Slf4j
public class ActiveMqRouteToXcom extends RouteBuilder {
    private final String activeMqOutputQueueName;
    private final XcomManager xcomManager;
	private int displayTransactionMsgRT = 0;
	private final MessageTimeoutChecker timeoutChecker;
	
    @Autowired
    public ActiveMqRouteToXcom(
            @Value("${fai.output-queue-name}") String activeMqOutputQueueName, XcomManager xcomManager,
            @Value("${display.transaction.msg.rt}") int displayTransactionMsgRT, MessageTimeoutChecker timeoutChecker) {
        this.activeMqOutputQueueName = "activemq:" + activeMqOutputQueueName + "?exchangePattern=InOnly";
        this.xcomManager = xcomManager;
		this.displayTransactionMsgRT = displayTransactionMsgRT;
		this.timeoutChecker = timeoutChecker;
    }

    @PostConstruct
    public void postConstruct() {
        xcomManager.initialize();
    }
    
    @Override
    public void configure() {
        onException(Exception.class)
            .handled(true)
            .process(this::handleException);

        from(this.activeMqOutputQueueName)
            .process(this::processExchange)
            .process(exchange -> {
            	timeoutChecker.updateLastMessageReceivedTime();
            })
            .end();
    }

    private void handleException(Exchange exchange) {
        Exception exception = exchange.getProperty(Exchange.EXCEPTION_CAUGHT, Exception.class);
        log.error("Error occurred while reading the response", exception);
    }

    private void processExchange(Exchange exchange) {
        String response = exchange.getIn().getBody(String.class);
        log.info("Service name: {}, FaircvRT: Received the response from output queue: {}", State.CURRENT.getServiceName(),
                (displayTransactionMsgRT == 1) ? response : response.subSequence(52, 83) + " " + response.subSequence(604, 612));
        writeResponseToXcom(" " + response);
    }
    
    public void writeResponseToXcom(String response) {
    	xcomManager.writeToXcom(QUEUE_NAME, N_TYPE, N_SUBTYPE1, N_SUBTYPE2, N_APP_DATA_TYPE, response, response.length());
    }
}
