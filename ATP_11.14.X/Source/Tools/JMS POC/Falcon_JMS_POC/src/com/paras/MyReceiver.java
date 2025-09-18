package com.paras;

import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueReceiver;
import javax.jms.QueueSession;
import javax.jms.Session;
import javax.naming.InitialContext;

public class MyReceiver {
	
	public static void main(String[] args) {
		  try{  
	            //1) Create and start connection  
	            InitialContext ctx=new InitialContext();  
	            QueueConnectionFactory f=(QueueConnectionFactory)ctx.lookup("myQueueConnectionFactory");  
	            QueueConnection con=f.createQueueConnection();  
	            con.start();  
	            
	            //2) create Queue session  
	            QueueSession ses=con.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);  
	            
	            //3) get the Queue object  
	            Queue que=(Queue)ctx.lookup("myQueue");  
	            
	            //4)create QueueReceiver  
	            QueueReceiver receiver=ses.createReceiver(que);  
	            
	            //5) Create Listener Object
	            MyListener listener = new MyListener();
	            
	            //6) register the listener object with receiver
	            receiver.setMessageListener(listener);
	            
	            System.out.println("Receiver is ready waiting for messages....");
	            System.out.println("press ctrl+c for shutdown");
	            
	            while (true) {
					Thread.sleep(1000);
				}	            
	              
		} catch (Exception e) {
			System.out.println(e);
		}
	}

}
