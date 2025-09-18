import java.io.FileInputStream;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.io.UnsupportedEncodingException;
import java.net.UnknownHostException;
import java.util.Properties;

import javax.net.ssl.SSLException;

import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpRequest;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpRequestRetryHandler;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.protocol.HttpClientContext;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
/*import org.apache.log4j.DailyRollingFileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;*/
/*import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
*/
/**
 * @author 104103
 *
 */
public class ApacheHttpClient {

	static boolean logsCreate = false;
	static boolean appenderExist = true;

	private final static Logger logger = LogManager.getLogger(ApacheHttpClient.class);

	/*
	 * This method will check the input <STX> if input accepted successfully
	 * this will create the Semaphore file and the respective Transaction file
	 */
	public void startOfTransaction() throws Exception {
		System.out.println("startOfTransaction() called....");
		writeToService("<STX>");
	}

	/*
	 * Send message to service
	 */
	public void writeToService(String stringData) throws Exception {
		System.out.println("Data ::::::::" + stringData);
		CloseableHttpClient httpClient = HttpClients.custom().setRetryHandler(retryHandler()).build();
		Properties prop = new Properties();
		String propFilePath = System.getenv("ASCENDENTROOT");
		if (propFilePath != null) {
			prop.load(new FileInputStream(propFilePath + "/config/app_properties.properties"));
		} else {
			System.out.println(
					"There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
		}
		String url = prop.getProperty("apacheRestUrl");
		HttpPost postRequest = new HttpPost(url);
		StringEntity data;
		try {
			data = new StringEntity(stringData);
			data.setContentType("plain/text");
			postRequest.setEntity(data);
			CloseableHttpResponse response = httpClient.execute(postRequest);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			System.out.println("Failed to send the POST request.!!!!!");
		} catch (ClientProtocolException e) {
			e.printStackTrace();
			System.out.println("Failed to send the POST request.!!!!!");
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Failed to send the POST request.!!!!!");
		} finally {
			postRequest.releaseConnection();
			httpClient.close();
		}
	}

	private HttpRequestRetryHandler retryHandler() {
		return (exception, executionCount, context) -> {

			System.out.println("try request: " + executionCount);

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
			boolean idempotent = !(request instanceof HttpEntityEnclosingRequest);
			if (idempotent) {
				// Retry if the request is considered idempotent
				return false;// true
			}
			return false;
		};
	}

	/*
	 * This method is responsible for writing transaction to archived file
	 */
	public void writeData(String data) throws Exception {
		writeToService(data);
		logger.info("Transaction is logging into archive file... :" + data);
	}

	/*
	 * This method will look for the <ETX> as soon as it will get the <ETX> it
	 * will transfer the file through SFTP to the FAI system
	 */
	public void endOfTransaction() throws Exception {
		writeToService("<ETX>");
	}
}
