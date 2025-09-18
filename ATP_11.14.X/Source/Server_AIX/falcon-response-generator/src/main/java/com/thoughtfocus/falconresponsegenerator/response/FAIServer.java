package com.thoughtfocus.falconresponsegenerator.response;

import org.apache.camel.builder.RouteBuilder;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import lombok.extern.slf4j.Slf4j;

@Component
@Slf4j
public class FAIServer extends RouteBuilder {
    private final String activeMqInputQueueName;
    private final String activeMqOutputQueueName;
    private final String falconResponseFlag;
    public FAIServer(
            @Value("${fai.input-queue-name}") String activeMqInputQueueName,
            @Value("${fai.output-queue-name}") String activeMqOutputQueueName,
            @Value("${falcon.response.flag}") String falconResponseFlag) {
        this.activeMqInputQueueName = "activemq:" + activeMqInputQueueName + "?exchangePattern=InOnly";
        this.activeMqOutputQueueName = "activemq:" + activeMqOutputQueueName + "?exchangePattern=InOnly";
        this.falconResponseFlag = falconResponseFlag;
    }
    private final String approved = "4 1FFM.FRD.CARD             0   4   2   8  77                                                                                                                                                                                                                                                                                                      apc15007                                                                             5origServCase                                                    authAdvice                      Approve                         authAdvice                      Approve                         authAdvice                      Approve                                                                                                                                                                                                                         authAdvice                      Approve                                                                                         authAdvice                      Approve                         scsr";
    private final String declined = "4 1FFM.FRD.CARD             0   4   2   8  77                                                                                                                                                                                                                                                                                                      apc15007                                                                             5origServCase                                                    authAdvice                      Decline                         authAdvice                      Decline                         authAdvice                      Decline                                                                                                                                                                                                                         authAdvice                      Decline                                                                                         authAdvice                      Decline                         scsr";
    private final String emptyDecision = "4 1FFM.FRD.CARD             0 102  20   2   3                                                                                                                                                                                                                                                                                                      apc150a1                                                                             1origServCase                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ";
    @Override
    public void configure() throws Exception {
        from(this.activeMqInputQueueName)
            .process(exchange -> {
                String message = exchange.getIn().getBody(String.class);
                log.info("Received the request from request queue: {}", message);
                if (Character.toString(message.charAt(12)).equals("1")) {
                    String header = message.substring(0, 83);
                    String decision = ("1".equals(falconResponseFlag)) ? approved
                            : ("0".equals(falconResponseFlag)) ? declined : emptyDecision;
                    StringBuilder sb = new StringBuilder(header);
                    sb.append(decision);
                    message = sb.toString();
                    log.debug("Sending response back to the falcon: {}", message);
                    exchange.getIn().setBody(message);
                } else {
                    log.debug("FAI will not respond for online mode");
                    exchange.getIn().setBody(null);
                }
            })
            .choice()
            .when(body().isNotNull())
            .to(this.activeMqOutputQueueName);
    }		
}
