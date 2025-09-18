import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

public class PropertyUtil {
	private static Logger LOG = LogManager.getLogger(PropertyUtil.class);
	
	public static int getInt(String key) {
		Properties prop = getProperties();
		try {
			int data = Integer.parseInt(prop.getProperty(key));
			return data;
		} catch (Exception ex) {
			LOG.error(ex.getMessage(), ex);
			LOG.warn("RETURNING -1 from PropertyUtil.getInt()");
			return -1;
		}
	}

	public static String getString(String key) {
		Properties prop = getProperties();
		return prop.getProperty(key);
	}

	private static Properties getProperties() {
		Properties prop = new Properties();
		String propFilePath = System.getenv("ASCENDENTROOT");
		if (propFilePath != null) {
			try {
				prop.load(new FileInputStream(propFilePath + "/config/app_properties.properties"));
			} catch (IOException ex) {
				LOG.error(ex.getMessage(), ex);
				LOG.warn("I/O Exception reading app_properties.properties file!");
			}
		} else {
			System.err.println(
					"There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
		}
		return prop;
	}
}
