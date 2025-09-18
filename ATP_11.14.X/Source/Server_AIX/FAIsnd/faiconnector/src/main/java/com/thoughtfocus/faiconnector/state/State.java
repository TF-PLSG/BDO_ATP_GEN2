package com.thoughtfocus.faiconnector.state;

public class State {
	private boolean xcomInitialized;
	private boolean activeMqInitialized;
	private String appName;
	private String serviceName;
	private static Object syncObject = new Object();
	private int messageLength;
	private int realTimeMessageLength;
	private int ext10Length;

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
	
	public boolean getactiveMqInitialized() {
		return activeMqInitialized;
	}
	
	public void setactiveMqInitialized(boolean value) {
		this.activeMqInitialized = value;
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
	
	public int getRealTimeMessageLength() {
		return realTimeMessageLength;
	}

	public void setRealTimeMessageLength(int realTimeMessageLength) {
		this.realTimeMessageLength = realTimeMessageLength;
	}
	
	public int getExt10Length() {
		return ext10Length;
	}

	public void setExt10Length(int ext10Length) {
		this.ext10Length = ext10Length;
	}
}
