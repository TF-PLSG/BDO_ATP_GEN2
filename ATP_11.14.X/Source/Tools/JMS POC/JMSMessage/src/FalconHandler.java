public class FalconHandler {
	static {
		System.loadLibrary("FalconHandler");    // Load native library at runtime
        										// Falconhandler.dll (Windows) or FalconHandler.so (Unixes)
	}

	/*
	 * The native keyword is applied to a method to indicate that the method is
	 * implemented in native code using JNI (Java Native Interface).
	 */
	public native int initialize(String appName, String serviceName);

	public native String sendRequestMessage(String serviceName);

	public native void receiveResponseMsg(String response);

	public native void terminate();

	public native int getLibFileVersion();

	public native void checkNetworkStatus(String status);
}
