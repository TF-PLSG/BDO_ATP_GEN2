package com.microservices.actions;

import com.microservices.helpers.StreamGlobber;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.stream.Stream;

@Service
/**
 * SCP Upload Command Executor executes the SCP command on the local system.
 * 
 * (Requires SCP Command to be configured for passwordless upload to the
 * destination)
 * 
 * @requires: sftp.host -> the server URI sftp.username -> the scp username
 *            sftp.landingzone.location -> the path on the destination server
 *            where the file will be uploaded
 * 
 *            command format: scp sftpUserName fileName
 *            sftpUserName@:sftpServer/sftpUploadLocation
 * @author Paras Created on Dec 24, 2018
 *
 */
@Slf4j
public class ScpUploadCommandExecutor {
	private final String sftpUserName;
	private final String sftpUploadLocation;
	private final String sftpServer;
	private final String flags;
	private final int retries;
	private final ExecutorService executorService = Executors.newFixedThreadPool(2);
	
	@Value("${move.archive.file.path}")
	private String destFolderPath;

	public ScpUploadCommandExecutor(
			@Value("${sftp.host}") String sftpServer,
			@Value("${sftp.username}") String sftpUserName,
			@Value("${scp.flags}") String scpFlags,
			@Value("${sftp.landingzone.location}") String sftpUploadLocation,
			@Value("${scp.retries}") int retries) {
		this.sftpServer = sftpServer;
		this.sftpUserName = sftpUserName;
		this.flags = scpFlags;
		this.sftpUploadLocation = sftpUploadLocation;
		this.retries = retries;
	}

	public void upload(String fileName) {
		// scp sftpUsername fileName sftpUsername@sftpServer:/sftpFileUploadLocation
	    String[] commandArray = Stream.concat(
	            Stream.of("scp"),
	            Stream.concat(
	                    Arrays.stream(this.flags.split(" ")),
	                    Stream.of(fileName, String.format("%s@%s:%s", this.sftpUserName, this.sftpServer, this.sftpUploadLocation))
	            )
	    ).toArray(String[]::new);
		int exitValue = 1;
		int retriesLocal = retries;
		while (exitValue == 0 || retriesLocal > 0) {
			Process process = null;
			try {
                ProcessBuilder processBuilder = new ProcessBuilder(commandArray);
                process = processBuilder.start();

				StreamGlobber inputStreamGlobber = new StreamGlobber(process.getInputStream(), log::debug);
				StreamGlobber errorStreamGlobber = new StreamGlobber(process.getErrorStream(), log::debug);
                executorService.submit(inputStreamGlobber);
                executorService.submit(errorStreamGlobber);

				boolean completed = process.waitFor(30, TimeUnit.SECONDS);
				if (completed) {
					exitValue = process.exitValue();
					if (exitValue == 0) {
						moveFile(fileName);
						return;
					} else {
						retriesLocal--;
					}
				} else {
					process.destroyForcibly();
				}

			} catch (Exception ex) {
				log.error("Error executing command: " + commandArray, ex);
			} finally {
				if (process != null) {
					process.destroy();
				}
			}
		}
	}
	
	private void moveFile(String fileName) {
		try {
			Files.move(Paths.get(fileName), Paths.get(destFolderPath, new File(fileName).getName()), StandardCopyOption.ATOMIC_MOVE);
			log.info("Successfully moved file :" +fileName);
		} catch (Exception ex) {
			log.error("Error moving file :" +fileName, ex);
		}

	}
}
