package com.thoughtfocus.faiconnector.routes;

import com.thoughtfocus.faiconnector.common.Application;
import com.thoughtfocus.faiconnector.managers.XcomManager;
import com.thoughtfocus.faiconnector.processors.FaisndXcomMessageProcessor;
import org.apache.camel.CamelContext;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

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
		return FaisndXcomMessageProcessor.getInstance(camelContext, xcomManager);
	}
}

