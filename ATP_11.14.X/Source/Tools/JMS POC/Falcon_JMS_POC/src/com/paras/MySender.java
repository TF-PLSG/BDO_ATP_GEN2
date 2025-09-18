package com.paras;

import javax.naming.Context;
import javax.naming.InitialContext;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Hashtable;
import java.util.Properties;

import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSender;
import javax.jms.QueueSession;
import javax.jms.Session;
import javax.jms.TextMessage;

public class MySender {
	public static void main(String[] args) {
		try {
			
			/* // Initial Environment with various properties
			  Properties props = new Properties();
			  props.put(Context.INITIAL_CONTEXT_FACTORY, 
			      "com.sun.jndi.fscontext.FSContextFactory");
			  props.put(Context.PROVIDER_URL, ""file:///D:/Falcon_Poc/imq_admin_objects"");
			  props.put(Context.OBJECT_FACTORIES, "myQueueConnectionFactory");
			  props.put("admin", "admin");

			  // Call constructor
			  InitialContext ctx = new InitialContext(props);
*/
			
			//1) Creating and start connection			
			InitialContext ctx = new InitialContext();
			
			QueueConnectionFactory factory = (QueueConnectionFactory)ctx.lookup("myQueueConnectionFactory");
			QueueConnection con = factory.createQueueConnection();
			con.start();
			
			//2) create queue session			
			QueueSession ses = con.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
			
			//3) Get the queue object			
			Queue que = (Queue)ctx.lookup("myQueue");
			
			//4) create QueueSender object			
			QueueSender sender = ses.createSender(que);
			
			//5) create TextMessage Object			
			TextMessage msg = ses.createTextMessage();
			
			//6)Write Message			
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			
			while (true) {
				System.out.println("Enter Msg, end to terminate");
				String str = br.readLine();
				
				if (str.equals("end")) 
					break;
					msg.setText(str);
					
					//7)Send Message
					sender.send(msg);
					System.out.println("Message send successfully....!!!");
			}		
				
				
				
				//8) Connection Close
				con.close();
			
			
		} catch (Exception e) {
			System.out.println(e);
		}
		
	}
}
