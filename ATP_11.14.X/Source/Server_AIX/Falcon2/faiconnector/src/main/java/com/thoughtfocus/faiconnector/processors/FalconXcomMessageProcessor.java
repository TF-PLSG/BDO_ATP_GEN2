package com.thoughtfocus.faiconnector.processors;

import org.apache.camel.CamelContext;
import org.apache.camel.Exchange;
import org.apache.camel.Processor;
import org.apache.camel.ProducerTemplate;

import com.thoughtfocus.faiconnector.managers.XcomManager;

import lombok.extern.slf4j.Slf4j;

import java.io.IOException;

@Slf4j
public class FalconXcomMessageProcessor extends ProcessorBase implements Processor {

	private static FalconXcomMessageProcessor INSTANCE;
	private FalconXcomMessageProcessor(CamelContext camelContext, XcomManager xcomManager) {
		super(camelContext, xcomManager);
		log.debug("Starting FalconXcomMessageProcessor");
	}

	public static FalconXcomMessageProcessor getInstance(CamelContext camelContext, XcomManager xcomManager) {
		if(INSTANCE == null) {
			INSTANCE = new FalconXcomMessageProcessor(camelContext, xcomManager);
		}
		return INSTANCE;
	}

	@Override
	public void process(Exchange exchange) {
		String message = getMessage();
		try (ProducerTemplate producerTemplate = camelContext.createProducerTemplate()) {
			if (message != null) {
				producerTemplate.requestBody("direct:xcom", message);
			}
		} catch (IOException e) {
			log.debug(e.toString());
		}
	}
}
