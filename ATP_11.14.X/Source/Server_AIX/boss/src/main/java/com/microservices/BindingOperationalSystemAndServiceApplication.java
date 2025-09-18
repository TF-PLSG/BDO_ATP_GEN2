package com.microservices;

import com.microservices.actions.PrecheckScanner;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.context.ConfigurableApplicationContext;

@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
public class BindingOperationalSystemAndServiceApplication {
	
    public static void main(String[] args) {
    	ConfigurableApplicationContext applicationContext = new SpringApplicationBuilder(BindingOperationalSystemAndServiceApplication.class)
    														.properties("spring.config.name:mracon_app_properties",
    																"spring.config.location:file:${ASCENDENTROOT}/config/")
    														.build()
    														.run(args);
        PrecheckScanner precheckScanner = applicationContext.getBean(PrecheckScanner.class);
        precheckScanner.doPrecheck();
    }
}
