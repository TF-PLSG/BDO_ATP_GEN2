import java.io.BufferedReader;
import java.io.FileReader;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

/*import org.apache.log4j.FileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;*/

public class MraconArchive {

	/**
	 * @param args
	 */
	private final static Logger logger = LogManager.getLogger(MraconArchive.class);
	public static void main(String[] args) {
		int sentCount=0;
		try{
			int delay = 0;
			if(args.length>0){
				delay = Integer.parseInt(args[0]);
			}
			String propFilePath = System.getenv("ASCENDENTROOT");
			if(propFilePath !=null){
			   //Stirng 
			   /*String loggerPath = propFilePath+"/trace/mraconArchive"+getCurrentTimeStamp()+".log";
			   PatternLayout layout =new PatternLayout();
			   layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
			   FileAppender appender = new FileAppender(layout,loggerPath,false);    
			   logger.addAppender(appender);*/
			}
			MessageSender messageSender = new MessageSender();
			messageSender.setup();
			String msgTextFile = propFilePath+"/log/Mracon_Archive.txt";
			FileReader fr = new FileReader(msgTextFile);
		    BufferedReader br = new BufferedReader(fr);
		    String line = null;
		    while ((line = br.readLine()) != null) {
		    	if(delay != 0){
		    		Thread.sleep(delay);
		    	}
		    	messageSender.sendMessage(line);
		    	sentCount++;
		    	logger.info("Message Sent===================="+line);
		    }
		    messageSender.closeSession();
		    System.out.println("sentCount============"+sentCount);
		}catch (Exception e) {
			e.printStackTrace();
		}
	}
	public static String getCurrentTimeStamp() {
	    SimpleDateFormat sdfDate = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");//dd/MM/yyyy
	    Date now = new Date();
	    String strDate = sdfDate.format(now);
	    return strDate;
	}
}
