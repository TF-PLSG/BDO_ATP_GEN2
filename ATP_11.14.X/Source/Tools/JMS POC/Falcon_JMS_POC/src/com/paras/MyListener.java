package com.paras;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.TextMessage;

public class MyListener implements MessageListener{

	@Override
	public void onMessage(Message msg) {
		try {
			TextMessage m = (TextMessage)msg;
			
			System.out.println("Following Message is received:" +m.getText());
		} catch (JMSException e) {
			System.out.println(e);
		}
		
	}

	
}
