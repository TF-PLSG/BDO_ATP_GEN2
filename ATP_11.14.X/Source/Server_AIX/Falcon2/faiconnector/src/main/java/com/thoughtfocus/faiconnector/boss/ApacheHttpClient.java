package com.thoughtfocus.faiconnector.boss;

import lombok.extern.slf4j.Slf4j;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpRequest;
import org.apache.http.client.HttpRequestRetryHandler;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.protocol.HttpClientContext;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;

import javax.net.ssl.SSLException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.net.UnknownHostException;
import java.nio.file.Paths;
import java.util.Properties;

import static java.nio.file.Files.newInputStream;

/**
 * @author 104103
 */
@Slf4j
public class ApacheHttpClient {

    /*
     * This method will check the input <STX> if input accepted successfully
     * this will create the Semaphore file and the respective Transaction file
     */
    public void startOfTransaction() throws Exception {
        writeToService("<STX>");
    }

    /*
     * Send message to service
     */
    public void writeToService(String stringData) throws Exception {
        CloseableHttpClient httpClient = HttpClients.custom().setRetryHandler(retryHandler()).build();
        Properties prop = new Properties();
        String propFilePath = System.getenv("ASCENDENTROOT");
        try (InputStream inputStream = newInputStream(Paths.get(propFilePath + "/config/falcon_app_properties.properties"))) {
            if (inputStream != null) {
                prop.load(inputStream);
            } else {
                log.debug(
                        "There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
            }
            String url = prop.getProperty("apacheRestUrl");
            HttpPost postRequest = new HttpPost(url);
            StringEntity data;
            try {
                data = new StringEntity(stringData);
                data.setContentType("plain/text");
                postRequest.setEntity(data);
                httpClient.execute(postRequest);
            } catch (IOException e) {
                e.printStackTrace();
                log.warn("Failed to send the POST request.!!!!!");

            } finally {
                postRequest.releaseConnection();
                httpClient.close();
            }
        }
    }

    private HttpRequestRetryHandler retryHandler() {
        return (exception, executionCount, context) -> {

            log.debug("try request: " + executionCount);

            if (executionCount > 1) {
                // Do not retry if over max retry count
                return false;
            }
            if (exception instanceof InterruptedIOException) {
                // Timeout
                return false;
            }
            if (exception instanceof UnknownHostException) {
                // Unknown host
                return false;
            }
            if (exception instanceof SSLException) {
                // SSL handshake exception
                return false;
            }
            HttpClientContext clientContext = HttpClientContext.adapt(context);
            HttpRequest request = clientContext.getRequest();
            return !(request instanceof HttpEntityEnclosingRequest);
        };
    }
}
