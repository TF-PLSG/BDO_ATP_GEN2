package com.paras;

import java.io.IOException;
import java.io.PrintWriter;
import javax.ejb.EJB;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
 
@WebServlet(urlPatterns = {"/TestServlet"})
public class TestServlet extends HttpServlet {
 
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@EJB MessageSender sender;
 
    @EJB MessageReceiverSync receiver;
 
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setContentType("text/html;charset=UTF-8");
        try (PrintWriter out = response.getWriter()) {
            out.println("");
            out.println("");
            out.println("JMS2 Send Message");
            out.println("");
            out.println("");
            out.println("JMS2 Send/Receive Message using JMS2 " + request.getContextPath() + "");
            String m = "Hello there";
            sender.sendMessage(m);
            out.format("Message sent: %1$s.", m);
            out.println("Receiving message...");
            String message = receiver.receiveMessage();
            out.println("Message rx: " + message);
            out.println("");
            out.println("");
        }
    }
 
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }
 
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }
 
    @Override
    public String getServletInfo() {
        return "Short description";
    }
}