import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

public class BossCommunicationManager {
	private CommunicationState currentState;
	private ApacheHttpClient httpClient;
	private static BossCommunicationManager instance;
	private static BossCommunicationManager testableInstance;
	
	
	static boolean logsCreate = false;
	static boolean appenderExist = true;

	private final static Logger LOG = LogManager.getLogger(BossCommunicationManager.class);

	//private static final Logger System.out.println = LoggerFactory.getLogger(BossCommunicationManager.class);
	
	private enum CommunicationState
    {
        NONE, STX_SENT, ETX_SENT;
    }

	/**
	 * Private Constructor for making BossCommunicationManager a singleton
	 */
	private BossCommunicationManager(){
		currentState = CommunicationState.NONE;
		httpClient = new ApacheHttpClient();
	}

	/**
	 * Private Constructor for making BossCommunicationManager a singleton. 
	 * This is for testing purposes only
	 */
	private BossCommunicationManager(ApacheHttpClient client){
		currentState = CommunicationState.NONE;
		httpClient = client;
	}
	
	/**
	 * Singleton Instance!
	 * @return the current instance
	 */
	public static BossCommunicationManager instance() {
		
		if (instance == null) {
			instance = new BossCommunicationManager();
		}
		return instance;
	}
	
	/* send mock httpClient here*/
	/**
	 * Get a testable instance of BossCommunicationManager (used for unit testing)
	 * @param httpClient mocked ApacheHttpClient object
	 * @return a testable instance of BossCommunicationManager
	 */
	public static BossCommunicationManager testableInstance(ApacheHttpClient httpClient) {
		if (testableInstance == null) {
			testableInstance = new BossCommunicationManager(httpClient);
		}
		return testableInstance;
	}
	
	/**
	 * Write archiving transaction data to BOSS
	 * @param data the transaction data
	 */
	public void write(String data) {
		System.out.println("we are in write() method" +data);
		try {
			if (currentState == CommunicationState.NONE
					|| currentState == CommunicationState.ETX_SENT) {
				httpClient.startOfTransaction();
				currentState = CommunicationState.STX_SENT;
				System.out.println("BossCommunication Manager Sending Message To ApacheClient.....!!!!! ");
				httpClient.writeToService(data);
			} else /* if (currentState == CommunicationState.STX_SENT) */{
				System.out.println("BossCommunication Manager (else) Sending Message To ApacheClient.....!!!!! ");
				httpClient.writeToService(data);
			}
		} catch (Exception e) {
			/*e.printStackTrace();*/
			System.out.println("Failed to archive the transaction currentState of message is not matching with communicationState..!!!!");
			System.out.println("exception from write() method.."+e);
		}
	}

	/**
	 * Resets the currentState to ETX_SENT and also sends an ETX if STX was previously sent
	 */
	public void reset() throws Exception {
		if (currentState == CommunicationState.STX_SENT) {
			httpClient.endOfTransaction();
			currentState = CommunicationState.ETX_SENT;
		}
	}
	
	/*public static void enableBossCommunicationLogger() {
		String propFilePath = System.getenv("ASCENDENTROOT");
		try {
			if (propFilePath != null && logsCreate == true) {
				appenderExist = true;
				String loggerPath = propFilePath + "/trace/falcon.log";
				PatternLayout layout = new PatternLayout();
				layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
				String datePattern = "'.'yyyy-MM-dd";
				DailyRollingFileAppender appender = new DailyRollingFileAppender(layout, loggerPath, datePattern);
				System.out.println.addAppender(appender);
				
			}
		} catch (Exception e) {
			e.printStackTrace();
		}*/
	
	}

