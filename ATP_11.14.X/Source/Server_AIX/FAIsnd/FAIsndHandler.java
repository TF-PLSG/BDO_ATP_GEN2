public class FAIsndHandler{
	static { 
        System.loadLibrary("FAIsndHandler");
    }
	 public native int initialize(String appName,String serviceName); 
	 public native String sendRequestMessage(String serviceName);
	 public native void receiveResponseMsg(String response); 
	 public native void terminate();
	 public native int getLibFileVersion();
	 public native String SendRequestToService(String que_name, String nType, String nSubType1, 
				String nSubType2, String nAppDataType, 
				String p_data,   String nDataSize);
}
