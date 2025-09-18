package com.thoughtfocus.faiconnector.routes;

import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.N_APP_DATA_TYPE;
import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.N_SUBTYPE1;
import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.N_SUBTYPE2;
import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.N_TYPE;
import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.QUEUE_NAME;
import static com.thoughtfocus.faiconnector.utils.Constants.ActiveMqRouteToXcom.*;

import java.util.concurrent.atomic.AtomicLong;

import org.apache.camel.builder.RouteBuilder;
import org.springframework.stereotype.Component;

import com.thoughtfocus.faiconnector.managers.XcomManager;

@Component
public class MessageTimeoutChecker extends RouteBuilder{

    private final XcomManager xcomManager;
	private final AtomicLong lastMessageReceivedTime = new AtomicLong(System.currentTimeMillis());
	
    public MessageTimeoutChecker(XcomManager xcomManager) {
		this.xcomManager = xcomManager;
	}

	public void updateLastMessageReceivedTime() {
        lastMessageReceivedTime.set(System.currentTimeMillis());
    }
    
	@Override
	public void configure() throws Exception {
        from("timer://messageTimer?fixedRate=true&delay=0&period=1000")
        .bean(this, "checkLastMessageTime");
	}
	
    public void checkLastMessageTime() {
        long currentTime = System.currentTimeMillis();
        long lastReceived = lastMessageReceivedTime.get();

        if (currentTime - lastReceived >= 60000) {
            log.info("No message received within 60 seconds after the last message. Processing empty message.");
            lastMessageReceivedTime.set(currentTime);
            xcomManager.writeToXcom(QUEUE_NAME, N_TYPE, N_SUBTYPE1, N_SUBTYPE2, N_APP_DATA_TYPE, EMPTY_MESSAGE, EMPTY_MESSAGE.length());
        }
    }

}
