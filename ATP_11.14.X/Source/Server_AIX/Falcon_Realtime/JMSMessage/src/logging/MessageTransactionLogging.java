package logging;

import java.io.FileInputStream;
import java.util.Properties;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

//import org.apache.log4j.Logger;

public class MessageTransactionLogging {
	static boolean logsCreate = true;
	
	private static Properties prop;
	
	public Properties getProp() {
		return prop;
	}

	public void setProp(Properties prop) {
		MessageTransactionLogging.prop = prop;
	}

	//private final static Logger logger = Logger.getLogger(MessageTransactionLogging.class); 
	
	private final static Logger logger = LogManager.getLogger(MessageTransactionLogging.class);
	
	public static void enableLogger() {
		logger.info("Message Transaction Logging Enabled.....!!!!");
		prop = new Properties();
		try {
			String propFilePath = System.getenv("ASCENDENTROOT");
			
			if (propFilePath != null && logsCreate == true) {
				prop.load(new FileInputStream(propFilePath+"/config/app_properties.properties"));
				PropertyConfigurator.configure(prop);
			}else{
				logger.info("There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
			}
		
		} catch (Exception e) {
			e.getMessage();
		}
	}
}
