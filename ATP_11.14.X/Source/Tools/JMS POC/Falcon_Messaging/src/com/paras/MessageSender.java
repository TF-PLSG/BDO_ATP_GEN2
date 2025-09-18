package com.paras;

import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.jms.*;
 
@Stateless
public class MessageSender {
 
    @Resource(mappedName = "jms/JMSConnectionFactory")
    private ConnectionFactory connectionFactory;
 
    @Resource(mappedName = "jms/myQueue")
    Queue queue;
 
    public void sendMessage(String message) {
        MessageProducer messageProducer;
        TextMessage textMessage;
        try {
            Connection connection = connectionFactory.createConnection();
            Session session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queue);
            textMessage = session.createTextMessage();
            //textMessage.setJMSDeliveryMode(DeliveryMode.NON_PERSISTENT);
 
            textMessage.setText(message);
            messageProducer.send(textMessage);
 
            messageProducer.close();
            session.close();
            connection.close();
        } catch (JMSException e) {
            e.printStackTrace();
        }
    }
}