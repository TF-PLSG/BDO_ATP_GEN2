package com.thoughtfocus.faiconnector.routes;

import com.thoughtfocus.faiconnector.common.Application;
import com.thoughtfocus.faiconnector.managers.XcomManager;
import com.thoughtfocus.faiconnector.processors.FalconXcomMessageProcessor;
import com.thoughtfocus.faiconnector.processors.MraconXcomMessageProcessor;
import com.thoughtfocus.faiconnector.state.State;
import org.apache.camel.CamelContext;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.thoughtfocus.faiconnector.utils.Constants.*;

@Component
public class TimerPollXcomRoute extends RouteBuilder {
	private final CamelContext camelContext;
	private final XcomManager xcomManager;
	private final Application application;
	
	@Autowired
	public TimerPollXcomRoute(CamelContext camelContext, XcomManager xcomManager, Application application) {
		this.camelContext = camelContext;
		this.xcomManager = xcomManager;
		this.application = application;
	}
	
	@Override
	public void configure() {
		// Define exception strategy
		onException(NoClassDefFoundError.class).handled(true)
		.process(exchange -> {
			camelContext.getShutdownStrategy().setTimeout(1);
			application.initiateShutdown(255);
		}).stop().end();
		
		// define timer polling. period in milliseconds
		from("timer:active-mq-timer?period=20")
		.process(getMessageProcessor())
		.end();
	}
	
	private Processor getMessageProcessor() {
		switch(State.CURRENT.getAppName()) {
			case FALCON:
				return FalconXcomMessageProcessor.getInstance(camelContext, xcomManager);
			case MRACON: 
				return MraconXcomMessageProcessor.getInstance(camelContext, xcomManager);
			default: 
				return FalconXcomMessageProcessor.getInstance(camelContext, xcomManager);
		}
	}
}

