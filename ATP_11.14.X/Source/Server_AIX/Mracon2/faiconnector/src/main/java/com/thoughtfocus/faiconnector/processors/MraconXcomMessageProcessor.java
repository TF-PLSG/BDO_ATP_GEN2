package com.thoughtfocus.faiconnector.processors;

import java.io.IOException;

import org.apache.camel.CamelContext;
import org.apache.camel.Exchange;
import org.apache.camel.Processor;
import org.apache.camel.ProducerTemplate;

import com.thoughtfocus.faiconnector.managers.XcomManager;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public class MraconXcomMessageProcessor extends ProcessorBase implements Processor {
	private static MraconXcomMessageProcessor INSTANCE;
	public MraconXcomMessageProcessor(CamelContext camelContext, XcomManager xcomManager) {
		super(camelContext, xcomManager);
		log.debug("Starting MraconXcomMessageProcessor");
	}
	
	public static MraconXcomMessageProcessor getInstance(CamelContext camelContext, XcomManager xcomManager) {
		if(INSTANCE == null) {
			INSTANCE = new MraconXcomMessageProcessor(camelContext, xcomManager);
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
