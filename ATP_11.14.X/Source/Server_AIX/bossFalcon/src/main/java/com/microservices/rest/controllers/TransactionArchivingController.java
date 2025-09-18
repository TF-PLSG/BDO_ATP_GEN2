package com.microservices.rest.controllers;

import com.microservices.queue.EnqueueService;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.PropertySource;
import org.springframework.web.bind.annotation.*;

/**
 * Created by Aniket Inge on 3/12/2018.
 * <p>
 * Modified by Paras Charpe
 */

@Configuration
@PropertySource("file:${ASCENDENTROOT}/config/faisnd_app_properties.properties")
@RestController
public class TransactionArchivingController {

    private final EnqueueService enqueueService;

    public TransactionArchivingController(EnqueueService enqueueService) {
        this.enqueueService = enqueueService;
    }

    @PostMapping(value = "/archive")
    public String archive(@RequestBody String data) {
        enqueueService.enqueue(data);
        return "OK";
    }
}
