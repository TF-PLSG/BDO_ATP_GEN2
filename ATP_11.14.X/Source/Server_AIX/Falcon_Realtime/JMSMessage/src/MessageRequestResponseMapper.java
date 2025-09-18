import java.util.function.Consumer;
import framework.reqresmap.RequestResponseMapper;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

public class MessageRequestResponseMapper implements Consumer<String> {

	private static Logger LOG = LogManager.getLogger(MessageRequestResponseMapper.class);

	private FalconHandler falconHandler;
	private RequestResponseMapper mapper;

	String requestMsgCorrelId;
	String responseMsgCorrelId;

	final int EH_EXT_HDR_StartIndex = PropertyUtil.getInt("EH_EXT_HDR_StartIndex");
	final int EH_EXT_HDR_LastIndex = PropertyUtil.getInt("EH_EXT_HDR_LastIndex");

	public MessageRequestResponseMapper() {
		mapper = RequestResponseMapper.getInstance(this);
	}

	public void requestMapper(String requestMsg) {

		try {
			if (requestMsg != null && requestMsg.startsWith("R")) {
				requestMsgCorrelId = requestMsg.substring(EH_EXT_HDR_StartIndex, EH_EXT_HDR_LastIndex);
				mapper.addToMap(requestMsgCorrelId, requestMsg);

				LOG.info(
						"Message received in REAL_TIME mode timer task running and looking for the repsonse in predefined time.....!!!!");
				LOG.info("Message received in REAL_TIME : " + requestMsg);

			} else if (requestMsg != null && requestMsg.startsWith("O")) {
				// do nothing
				LOG.info(
						"Message received in ONLINE mode Timer task will not start there will be no response message for this transaction.....!!!!");
				LOG.info("Message received in ONLINE mode : " + requestMsg);
			}
		} catch (Exception e) {
			LOG.info("Exception ", e);
		}
	}

	public void responseMapper(String responseMsg) {
		try {
			if (responseMsg != null) {
				responseMsgCorrelId = responseMsg.substring(EH_EXT_HDR_StartIndex, EH_EXT_HDR_LastIndex);
				String requestMessageFromCache = mapper.getFromMap(responseMsgCorrelId);
				if (requestMessageFromCache != null)
					LOG.info("Sending resposne message back to ATP : " +"R"+ responseMsg);
					falconHandler.receiveResponseMsg("R" + responseMsg);
			}
		} catch (Exception e) {
			LOG.info("Exception :", e);
		}
	}

	@Override
	public void accept(String message) {
		LOG.info("Reponse message TIMEOUT happen sending same message to ATP " +"T" +message);
		falconHandler.receiveResponseMsg("T" + message);
	}
}
