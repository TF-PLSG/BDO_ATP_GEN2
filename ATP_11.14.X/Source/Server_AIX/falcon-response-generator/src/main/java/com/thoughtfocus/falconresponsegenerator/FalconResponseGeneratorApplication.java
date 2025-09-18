package com.thoughtfocus.falconresponsegenerator;

import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.builder.SpringApplicationBuilder;

@SpringBootApplication
public class FalconResponseGeneratorApplication {

	public static void main(String[] args) {
		new SpringApplicationBuilder(FalconResponseGeneratorApplication.class)
		.properties("spring.config.name:falcon_app_properties", "spring.config.location:file:${ASCENDENTROOT}/config/")
		.build().run(args);
	}

}
