package com.thoughtfocus.faiconnector.routes;

import com.thoughtfocus.faiconnector.boss.BossCommunicationManager;
import com.thoughtfocus.faiconnector.state.State;
import lombok.extern.slf4j.Slf4j;
import org.apache.camel.Exchange;
import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;
import static com.thoughtfocus.faiconnector.utils.Constants.XcomToActiveMqRoute.*;

import java.util.LinkedHashMap;
import java.util.Map;

@Component
@Slf4j
public class XcomToActiveMqRoute extends RouteBuilder {
	private final ActiveMqRouteToXcom activeMqRouteToXcom;
	private final String activeMqInputQueueName;
	private final LinkedHashMap<String, String> transactionList;
	private boolean requestType;
	private int displayTransactionMsg = 0;
	private int displayTransactionMsgRT = 0;

	private String message = "";

	@Autowired
	public XcomToActiveMqRoute(@Value("${fai.input-queue-name}") String activeMqInputQueueName,
			@Value("${message.length}") int messageLength, @Value("${transaction.map.size}") int MAX,
			@Value("${real-time.message.length}") int realTimeMessageLength, ActiveMqRouteToXcom activeMqRouteToXcom,
			@Value("${display.transaction.msg}") int displayTransactionMsg,
			@Value("${display.transaction.msg.rt}") int displayTransactionMsgRT) {
		log.debug("AppName:{}; ServiceName: {}, Reading from:{}", State.CURRENT.getAppName(),
				State.CURRENT.getServiceName(), activeMqInputQueueName);
		this.activeMqInputQueueName = "activemq:" + activeMqInputQueueName + "?exchangePattern=InOnly";
		State.CURRENT.setMessageLength(messageLength);
		State.CURRENT.setRealTimeMessageLength(realTimeMessageLength);
		this.activeMqRouteToXcom = activeMqRouteToXcom;
		this.displayTransactionMsg = displayTransactionMsg;
		this.displayTransactionMsgRT = displayTransactionMsgRT;
		transactionList = new LinkedHashMap<String, String>() {
			@Override
			protected boolean removeEldestEntry(Map.Entry<String, String> eldest) {
				return size() > MAX;
			}
		};
	}

	@Override
	public void configure() {
		onException(Exception.class).handled(true).process(this::handleException);
		from("direct:xcom")
	    .process(this::processMessage)
	    .choice()
	        .when(body().isNotNull())
	            .to(this.activeMqInputQueueName);
	}

	public void handleException(Exchange exchange) {
		Exception exception = (Exception) exchange.getProperty(Exchange.EXCEPTION_CAUGHT);
		log.error("Exception : {}", exception.getMessage());
		if (!isEmpty(message) && message.length() > 500) {
			log.info("Message Received Also Sending To Archive File: {}", message.substring(1, 101));
			BossCommunicationManager.instance().write(message);
		} else {
			log.warn("Empty message or less byte of message received can not be logged into archive file....!!!!");
		}
		if (requestType == true) {
			activeMqRouteToXcom.writeResponseToXcom(SYSTEM_ERROR + message.substring(1, 101));
		}
	}

	public void processMessage(Exchange exchange) {
		message = exchange.getIn().getBody(String.class);
		if (checkForDuplicateTransactions(message)) {
			exchange.getIn().setBody(null);
			log.warn("Duplicate transaction received: {}", message);
		} else {
			if (message.startsWith(REAL_TIME)) {
				requestType = true;
				log.info("FalconRT: Received the request from xcom size: {} Request: {}", message.length(),
				        (displayTransactionMsgRT == 1) ? message : message.substring(53, 85));
				message = message.length() >= 1061 ? message.substring(0, 1055) : message;
				exchange.getIn().setBody(message.substring(1));
				log.info("Service name: " + State.CURRENT.getServiceName()
						+ ", FalconRT:  Sending request to ActiveMq Size: " + message.length() + " TranId+RRN: "
						+ message.substring(53, 85));
			} else {
				log.info("Received the request from xcom for online: {}",
						(displayTransactionMsg == 1) ? message : message.substring(1, 101));
				message = exchange.getIn().getBody(String.class);
				message = message.length() >= 1030 ? message.substring(0, 1024) : message;
				exchange.getIn().setBody(message.substring(1));
				log.info("Sending request to ActiveMq in online: {}", message.substring(1, 101));
			}
		}
	}

	private boolean checkForDuplicateTransactions(String message) {
		String key = "";
		int cardNumberPosition = 138;
		int cardNumberLength = 20;
		int trxDateTimePosition = 339;
		int trxDateTimeLength = 10;
		int trxAmountPosition = 353;
		int trxAmountLength = 13;
		int terminalIdPosition = 765;
		int terminalIdLength = 16;
		int merchantIdPosition = 858;
		int merchantIdLength = 15;
		int stanPosition = 1024;
		int stanLength = 6;
		int realTimeCardNumberPosition = 169;
		int realTimeCardNumberLength = 20;
		int realTimeTrxDateTimePosition = 370;
		int realTimeTrxDateTimeLength = 10;
		int realTimeTrxAmountPosition = 384;
		int realTimeTrxAmountLength = 13;
		int realTimeTerminalIdPosition = 796;
		int realTimeTerminalIdLength = 16;
		int realTimeMerchantIdPosition = 889;
		int realTimeMerchantIdLength = 15;
		int realTimeStanPosition = 1055;
		int realTimeStanLength = 6;
		if (message.length() == 1030) {
			/* Below positions is related to Online-Mode transactions */
			key = ONLINE + message.substring(cardNumberPosition, cardNumberPosition + cardNumberLength)
					+ message.substring(trxDateTimePosition, trxDateTimePosition + trxDateTimeLength)
					+ message.substring(trxAmountPosition, trxAmountPosition + trxAmountLength)
					+ message.substring(terminalIdPosition, terminalIdPosition + terminalIdLength)
					+ message.substring(merchantIdPosition, merchantIdPosition + merchantIdLength)
					+ message.substring(stanPosition, stanPosition + stanLength);
		} else if (message.length() == 1061) {
			/* Below positions is related to Real-time-Mode transactions */
			key = REAL_TIME
					+ message.substring(realTimeCardNumberPosition,
							realTimeCardNumberPosition + realTimeCardNumberLength)
					+ message.substring(realTimeTrxDateTimePosition,
							realTimeTrxDateTimePosition + realTimeTrxDateTimeLength)
					+ message.substring(realTimeTrxAmountPosition, realTimeTrxAmountPosition + realTimeTrxAmountLength)
					+ message.substring(realTimeTerminalIdPosition,
							realTimeTerminalIdPosition + realTimeTerminalIdLength)
					+ message.substring(realTimeMerchantIdPosition,
							realTimeMerchantIdPosition + realTimeMerchantIdLength)
					+ message.substring(realTimeStanPosition, realTimeStanPosition + realTimeStanLength);
		}
		if (!key.isEmpty()) {
			if (transactionList.containsKey(key))
				return true;
			else {
				transactionList.put(key, message);
				return false;
			}
		}
		return false;
	}

	public boolean isEmpty(String str) {
		return str == null || str.isEmpty() || str.equals(" ");
	}
}
