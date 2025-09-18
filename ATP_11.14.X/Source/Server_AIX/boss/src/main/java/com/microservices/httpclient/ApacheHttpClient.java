package com.microservices.httpclient;

import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import java.io.IOException;

/**
 * @author 104103
 */
public class ApacheHttpClient {

    /*
     * This method will check the input <STX> if input accepted successfully
     * this will create the Semaphore file and the respective Transaction file
     */
    public static void startOfTransaction() {
        writeToService("<STX>");
    }

    /*
     * Send message to service
     */
    public static void writeToService(String stringData) {
        try (CloseableHttpClient httpClient = HttpClients.createDefault()) {
            HttpPost postRequest = new HttpPost("http://localhost:8080/archive");
            StringEntity header;
            StringEntity data;
            header = new StringEntity(stringData);
            header.setContentType("palin/text");
            data = new StringEntity(stringData);
            data.setContentType("plain/text");
            postRequest.setEntity(header);
            httpClient.execute(postRequest);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /*
     * This method is responsible for writing transaction to archived file
     */
    public static void writeData() {
        writeToService(
                "APC80           CRTRAN242.4  DT_BDO_CR       20120701121800100  8.005111111111111100    5111111111111100                        0000001740784200.001            5111111111111100   A                                     201505300000010000                                  201207011217000000000100.00608000000.021739AP1000000560000063  S           BB0101PHINST                      000000000000YS                                    test                                    testpur                       063  608 608  0000000001000000005111111111111100                                                                                                                                     P211111111112   2222222222222222OO00M                                                                  00    1000000000000001                                                                                                         888700                                    V");
    }

    /*
     * This method will look for the <ETX> as soon as it will get the <ETX> it
     * will transfer the file through SFTP to the FAI system
     */
    public static void endOfTransaction() {
        writeToService("<ETX>");
    }

    public static void main(String[] args) {
        startOfTransaction();
        for (int i = 0; i < 10; i++)
            writeData();
        endOfTransaction();
    }
}
