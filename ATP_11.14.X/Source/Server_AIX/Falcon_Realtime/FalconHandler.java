public class FalconHandler {
	static { 
        System.loadLibrary("FalconHandler");
    }
	 public native int initialize(String appName,String serviceName); 
	 public native String sendRequestMessage(String serviceName);
	 public native void receiveResponseMsg(String response); 
	 public native void terminate();
	 public native int getLibFileVersion();
}
