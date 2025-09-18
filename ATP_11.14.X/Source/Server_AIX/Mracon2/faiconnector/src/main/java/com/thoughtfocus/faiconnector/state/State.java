package com.thoughtfocus.faiconnector.state;

public class State {
	private boolean xcomInitialized;
	private String appName;
	private String serviceName;
	private static Object syncObject = new Object();
	private int messageLength;
	
	private State() {
		synchronized(syncObject) {
			xcomInitialized = false;
		}
	}
	
	public final static State CURRENT = new State();
	
	public boolean getXcomInitialized() {
		return xcomInitialized;
	}
	
	public void setXcomInitialized(boolean value) {
		this.xcomInitialized = value;
	}

	public String getAppName() {
		return appName.trim();
	}

	public void setAppName(String appName) {
		this.appName = appName;
	}

	public String getServiceName() {
		return serviceName.trim();
	}

	public void setServiceName(String serviceName) {
		this.serviceName = serviceName;
	}

	public int getMessageLength() {
		return messageLength;
	}

	public void setMessageLength(int messageLength) {
		this.messageLength = messageLength;
	}
}
