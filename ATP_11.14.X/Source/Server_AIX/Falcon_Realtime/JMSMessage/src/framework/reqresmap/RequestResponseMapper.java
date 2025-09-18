package framework.reqresmap;

import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import com.google.common.cache.Cache;
import com.google.common.cache.CacheBuilder;
import com.google.common.cache.RemovalListener;
import com.google.common.cache.RemovalNotification;

public class RequestResponseMapper implements RemovalListener<String, String>{
	
	private static Logger LOG = LogManager.getLogger(RequestResponseMapper.class);	
	
	private static RequestResponseMapper thisInstance;
	private static Object synchronizationObject = new Object();
	private Consumer<String> callbackOnExpiry;
	private Cache<String, String> expiringCache;

	private RequestResponseMapper() {
	} // making this a singleton

	public static RequestResponseMapper getInstance(Consumer<String> callbackOnExpiry) {
		synchronized (synchronizationObject) {
			if (thisInstance == null) {
				thisInstance = new RequestResponseMapper();
				thisInstance.setupExpiringCache();
				thisInstance.setCallbackOnExpiry(callbackOnExpiry);
			}
			return thisInstance;
		}
	}

	public void setCallbackOnExpiry(Consumer<String> callbackOnExpiry) {
		this.callbackOnExpiry = callbackOnExpiry;
	}

	public void setupExpiringCache() {
		expiringCache = CacheBuilder.newBuilder().maximumSize(10000).expireAfterWrite(2L, TimeUnit.SECONDS)
				.removalListener(this).build();
	}

	public void addToMap(String correlationId, String request) {
		synchronized (synchronizationObject) {
			expiringCache.put(correlationId, request);
		}
	}

	public String getFromMap(String correlationId) {
		synchronized (synchronizationObject) {
			String value = expiringCache.getIfPresent(correlationId);
			expiringCache.invalidate(correlationId);
			return value;
		}
	}

	/**
	 * Callback method upon removal of the object from the expiring cache
	 * 
	 * @param removalNotification
	 *            - RemovalNotification object
	 */
	@Override
	public void onRemoval(RemovalNotification<String, String> removalNotification) {
		callbackOnExpiry.accept(removalNotification.getValue()); // returns the complete request message
	}
}
