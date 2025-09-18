package com.thoughtfocus.faiconnector.common;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.context.ApplicationContext;
import org.springframework.stereotype.Component;

@Component
public class Application {
	private ApplicationContext applicationContext;
	
	public Application(@Autowired ApplicationContext applicationContext) {
		this.applicationContext = applicationContext;
	}
	
	public void initiateShutdown(int statusCode) {
		SpringApplication.exit(applicationContext, () -> statusCode);
		System.exit(statusCode);
	}
}
