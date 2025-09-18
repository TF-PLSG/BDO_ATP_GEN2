public class MraconHandler {
	static { 
        System.loadLibrary("MraconHandler");
    }
	 public native int initialize(String appName,String serviceName); 
	 public native String sendRequestMessage(String serviceName);
	 public native void receiveResponseMsg(String response); 
	 public native void terminate();
	 public native int getLibFileVersion();
	 public native void checkNetworkStatus(String status);
}
