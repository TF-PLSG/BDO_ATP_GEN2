package com.thoughtfocus.faiconnector.routes;

import org.apache.camel.CamelContext;
import org.apache.camel.Exchange;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import com.thoughtfocus.faiconnector.common.Application;
import com.thoughtfocus.faiconnector.managers.XcomManager;
import com.thoughtfocus.faiconnector.processors.FalconXcomMessageProcessor;
import com.thoughtfocus.faiconnector.processors.MraconXcomMessageProcessor;
import com.thoughtfocus.faiconnector.state.State;
import static com.thoughtfocus.faiconnector.utils.Constants.*;

@Component
public class TimerPollXcomRoute extends RouteBuilder {
	private CamelContext camelContext;
	private XcomManager xcomManager;
	private Application application;
	
	@Autowired
	public TimerPollXcomRoute(CamelContext camelContext, XcomManager xcomManager, Application application) {
		this.camelContext = camelContext;
		this.xcomManager = xcomManager;
		this.application = application;
	}
	
	@Override
	public void configure() throws Exception {
		// Define exception strategy
		onException(NoClassDefFoundError.class).handled(true)
		.process(new Processor() {
			@Override
			public void process(Exchange exchange) throws Exception {
				camelContext.getShutdownStrategy().setTimeout(1);
				application.initiateShutdown(255);
			}
		}).stop().end();
		
		// define timer polling. period in milliseconds
		from("timer:active-mq-timer?period=20")
		.process(getMessageProcessor())
		.end();
	}
	
	private Processor getMessageProcessor() {
		switch(State.CURRENT.getAppName()) {
		case MRACON:
			return MraconXcomMessageProcessor.getInstance(camelContext, xcomManager);
		case FALCON:
			return FalconXcomMessageProcessor.getInstance(camelContext, xcomManager);
		default:
			return MraconXcomMessageProcessor.getInstance(camelContext, xcomManager);
		}
	}
}

