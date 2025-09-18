package com.thoughtfocus.faiconnector;

import java.util.Properties;

import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.builder.SpringApplicationBuilder;

import com.thoughtfocus.faiconnector.state.State;

@SpringBootApplication
public class FaiConnectorApplication {
	public static void main(String[] args) {
		String ascendentRoot = System.getenv("ASCENDENTROOT");
		Properties properties = System.getProperties();
		String appName = properties.getProperty("service.app.name");
		String serviceName = properties.getProperty("service.service.name");
		
		State.CURRENT.setAppName(appName);
		State.CURRENT.setServiceName(serviceName);
		
		System.setProperty("jna.library.path", ascendentRoot + "/bin");
		System.setProperty("jna.debug_load", "true");
		new SpringApplicationBuilder(FaiConnectorApplication.class)
				.properties("spring.config.name:falcon_app_properties", "spring.config.location:file:${ASCENDENTROOT}/config/")
				.build().run(args);
	}
}
