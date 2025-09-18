package TestServer;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;

public class SimpleHttpServer {

	public static void main(String[] args) throws Exception {
		final ServerSocket server = new ServerSocket(8000);
		System.out.println("Listening to the port 8000.......");
		while(true){
			try(Socket socket = server.accept()){
				Date todayDate = new Date();
				String httpResponse = "HTTP/1.1 200 ok\r\n\r\n" + todayDate;
				socket.getOutputStream().write(httpResponse.getBytes("UTF-8"));
			}
		}
		
	}
}
