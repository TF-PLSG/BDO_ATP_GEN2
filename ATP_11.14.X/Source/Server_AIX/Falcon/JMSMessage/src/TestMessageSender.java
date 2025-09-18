import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Properties;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import logging.MessageTransactionLogging;

/*import org.apache.log4j.FileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
*/
public class TestMessageSender {
	
	private Properties prop;

	/**
	 * @param args
	 */
	public TestMessageSender() {

		prop = new Properties();
		try {   
			String propFilePath = System.getenv("ASCENDENTROOT");
			enableMessageSenderLogs();
			if(propFilePath !=null){
				prop.load(new FileInputStream(propFilePath+"/config/app_properties.properties"));
			}else{
				if(MsgSenderThread.logsCreate){
					logger.info("There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
				}
				//Fine Call
				System.exit(-1);
			}
		} catch (IOException e) { e.printStackTrace(); } 
	
	}
	private void enableMessageSenderLogs() {
		MessageTransactionLogging.enableLogger();
		
	}
	private final static Logger logger = LogManager.getLogger(TestMessageSender.class);
	public static void main(String[] args) {
	// Start Here	
		try{
			System.out.println("Hiii");
			String msg="APC80           CRTRAN242.4  DT_BDO_CR       20120701121800100  8.005111111111111100    5111111111111100                        0000001740784200.001            5111111111111100   A                                     201505300000010000                                  201207011217000000000100.00608000000.021739AP1000000560000063  S           BB0101PHINST                      000000000000YS                                    test                                    testpur                       063  608 608  0000000001000000005111111111111100                                                                                                                                     P211111111112   2222222222222222OO00M                                                                  00    1000000000000001                                                                                                         888700                                    V  ";
			MessageSender messageSender = new MessageSender();
			messageSender.setup();
			InputStream fstream = (InputStream)TestMessageSender.class.getResourceAsStream("CTRANS_msg_cards.txt");
			DataInputStream in = new DataInputStream(fstream);
			BufferedReader br = new BufferedReader(new InputStreamReader(in));
			String strLine;
			while ((strLine = br.readLine()) != null){
				Thread.sleep(1000);
				messageSender.sendMessage(strLine);
				logger.info("Message Sent====================");
			}
			in.close();
			messageSender.closeSession();
		   logger.info("Sent the message successfully===============");
		   String propFilePath = System.getenv("ASCENDENTROOT");
		   if(propFilePath !=null){
			  String loggerPath = propFilePath+"/trace/falcon.log";
			   /*PatternLayout layout =new PatternLayout();
			   layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
			   FileAppender appender = new FileAppender(layout,loggerPath,false);    
			   logger.addAppender(appender);*/
			}
			

			System.out.println("I am inside Main================"+args.length);
			logger.info("I am inside Main================"+args.length);
			logger.error("I am inside Main===errr============="+args.length);
			System.out.println("I am inside Main================"+args.length);
			logger.info("I am inside Main================"+args.length);
		   if(args.length>0){
				for(int i=0;i<args.length;i++){
					//System.out.println("There are args arguments"+args[i]);
					logger.info("There are args arguments"+args[i]);
				}
	    	}else{
	    		logger.info("There are no args arguments");
	    		System.out.println("There are no args arguments");
	    		//InputStream fstream = (InputStream)TestMessageSender.class.getResourceAsStream("D:\\epicportz\\CTRANS_msg_cards.txt");
	    		FileInputStream fstream1 = new FileInputStream("D:\\SemFile\\CRTRAN24_20180514_154207.txt.CRTRAN24");
	    		DataInputStream in1 = new DataInputStream(fstream1);
				BufferedReader br1 = new BufferedReader(new InputStreamReader(in1));
				String strLine1;
				while ((strLine1 = br1.readLine()) != null){
					logger.info("Message Sent===================="+strLine1);
				}
				FileReader fr = new FileReader("D:\\SemFile\\CRTRAN24_20180514_154207.txt.CRTRAN24");
			    BufferedReader br11 = new BufferedReader(fr);
			    String strLine11;
			    while (br11.readLine()!=null) {
			         System.out.println(br11.readLine());
			    }


	    	}
		}catch (Exception e) {
			e.printStackTrace();
		}
// End Here		
		try{
			int delay = 0;
			if(args.length>0){
				delay = Integer.parseInt(args[0]);
			}
			String propFilePath = System.getenv("ASCENDENTROOT");
			if(propFilePath !=null){
			   /*String loggerPath = propFilePath+"/trace/falconArchive.log";
			   PatternLayout layout =new PatternLayout();
			   layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
			   FileAppender appender = new FileAppender(layout,loggerPath,false);    
			   logger.addAppender(appender);*/
			}
			MessageSender messageSender = new MessageSender();
			messageSender.setup();
			String msgTextFile = propFilePath+"/config/app_properties.properties";
			FileReader fr = new FileReader(msgTextFile);
		    BufferedReader br = new BufferedReader(fr);
		    String line = null;
		    while ((line = br.readLine()) != null) {
		    	if(delay != 0){
		    		Thread.sleep(delay);
		    	}
		    	messageSender.sendMessage(line);
		    	logger.info("Message Sent===================="+line);
		    }
		    /*while (br.readLine()!=null) {
		    	i++;
		    	//Thread.sleep(1000);
				//messageSender.sendMessage(br.readLine());
				System.out.println("Message Sent===================="+i+":"+br.readLine());
		    	//logger.info("Message Sent===================="+i+":"+br.readLine());
		    }*/
		    messageSender.closeSession();
		}catch (Exception e) {
			e.printStackTrace();
		}
	}
}
