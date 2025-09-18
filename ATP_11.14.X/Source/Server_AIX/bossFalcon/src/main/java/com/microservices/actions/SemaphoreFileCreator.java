package com.microservices.actions;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import lombok.extern.slf4j.Slf4j;

import java.io.File;
import java.io.FileOutputStream;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.attribute.PosixFilePermission;
import java.nio.file.attribute.PosixFilePermissions;
import java.time.LocalDate;
import java.time.LocalTime;
import java.util.Set;

@Service
@Slf4j
public class SemaphoreFileCreator {
    public static final String SEMAPHORE_FILE_EXTENSION = ".CRTRAN24";

    private final String semaphoreFilePath;
	@Value("${archive.file.permissions}")
	private String archiveFilePermissions;
	private int displayExceptionLog = 0;

    public SemaphoreFileCreator(@Value("${archive.file.path}") String semaphoreFilePath,
    		@Value("${display.exception.log}") int displayExceptionLog) {
        this.semaphoreFilePath = semaphoreFilePath;
        this.displayExceptionLog = displayExceptionLog;
    }

    public String getSemaphoreFileName(LocalDate localDate, LocalTime localTime) {
        return String.format("CRTRAN24_%d%02d%02d_%02d%02d%02d_FALCON.txt.CRTRAN24", localDate.getYear(),
                localDate.getMonthValue(), localDate.getDayOfMonth(), localTime.getHour(), localTime.getMinute(),
                localTime.getSecond());
    }
	public void createFile(String fileName) {
		File file = new File(semaphoreFilePath, fileName);

		try {
			new FileOutputStream(file).close();

			// Do the following operations only if the Operating System is POSIX
			// compliant

			if (FileSystems.getDefault().supportedFileAttributeViews().contains("posix")) {
				// using PosixFilePermission to set file permissions 777
				Set<PosixFilePermission> perms = PosixFilePermissions.fromString(this.archiveFilePermissions);
				
				Files.setPosixFilePermissions(Paths.get(file.getAbsolutePath()), perms);
			}
			
		} catch (UnsupportedOperationException uoe) {
			log.warn(
					"Posix File permissions couldn't be set. Are you running this app on a non-POSIX complaiant system?",
					uoe);
		} catch (Exception ex) {
			log.error("Exception: " + ((displayExceptionLog == 1) ? ex : ex.getMessage()));
		}
	}

    public String createSemaphoreFileAndTransactionFile() {
        LocalDate localDate = LocalDate.now();
        LocalTime localTime = LocalTime.now();
        String semaphoreFileName = getSemaphoreFileName(localDate, localTime);
        String transactionFileName = getTransactionFileName(localDate, localTime);
        this.createFile(semaphoreFileName);
        this.createFile(transactionFileName);
        return transactionFileName;
    }

    public String getTransactionFileName(LocalDate localDate, LocalTime localTime) {
        return String.format("CRTRAN24_%d%02d%02d_%02d%02d%02d_FALCON.txt", localDate.getYear(),
                localDate.getMonthValue(), localDate.getDayOfMonth(), localTime.getHour(), localTime.getMinute(),
                localTime.getSecond());
    }
}
