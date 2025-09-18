package com.microservices.actions;

import com.microservices.enums.ArchiveLoggerState;
import java.nio.charset.StandardCharsets;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.PropertySource;
import org.springframework.stereotype.Service;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;

@Slf4j
@Service
@PropertySource("file:${ASCENDENTROOT}/config/faisnd_app_properties.properties")
public class ArchiveLogger {
    private ArchiveLoggerState archiveLoggerState;
    private int logCount = 0;
    private final SemaphoreFileCreator semaphoreFileCreator;
    private final ScpUploadCommandExecutor scpCommandExecutor;
    private String archiveFileName;
	private int displayExceptionLog = 0;

    @Value("${archive.txCount}")
    private int txCount;

    @Value("${archive.file.path}")
    private String archiveFilePath;

    public ArchiveLogger(SemaphoreFileCreator semaphoreFileCreator, ScpUploadCommandExecutor scpUploadCommandExecutor,
    		@Value("${display.exception.log}") int displayExceptionLog) {
        this.semaphoreFileCreator = semaphoreFileCreator;
        this.scpCommandExecutor = scpUploadCommandExecutor;
        this.archiveLoggerState = ArchiveLoggerState.NONE;
        this.displayExceptionLog = displayExceptionLog;
    }

    public synchronized void logData(String data) {
        if (data.equals("<STX>"))
            doStx();
        else if (data.equals("<ETX>"))
            doEtx();
        else
            doLogTxn(data);
    }

    private void doLogTxn(String data) {
        if (archiveLoggerState == ArchiveLoggerState.ETX_LOGGED
                || archiveLoggerState == ArchiveLoggerState.NONE) {
            logSingleTransaction(data);
        } else {
            changeLoggerStateTo(ArchiveLoggerState.TRANSACTION_LOGGED);
            if (logCount == txCount)
                resetLoggerFile();
            logCount++;
            writeTransaction(data, archiveFilePath, archiveFileName);
        }
    }

    private void logSingleTransaction(String data) {
        doStx();
        doLogTxn(data);
        doEtx();
    }

    private void resetLoggerFile() {
        writeFooter(logCount, archiveFilePath, archiveFileName);
        uploadArchiveFileToSftp(archiveFilePath, archiveFileName);
        createNewTransactionFile();
        writeHeader(archiveFilePath, archiveFileName);
        setLogCountTo(0);
    }

    private synchronized void doEtx() {
        if (archiveLoggerState == ArchiveLoggerState.TRANSACTION_LOGGED
                || archiveLoggerState == ArchiveLoggerState.STX_LOGGED) {
            writeFooter(logCount, archiveFilePath, archiveFileName);
            changeLoggerStateTo(ArchiveLoggerState.ETX_LOGGED);
            uploadArchiveFileToSftp(archiveFilePath, archiveFileName);
            setLogCountTo(0);
        }
    }

    private synchronized void doStx() {
        if (archiveLoggerState == ArchiveLoggerState.NONE
                || archiveLoggerState == ArchiveLoggerState.ETX_LOGGED) {
            changeLoggerStateTo(ArchiveLoggerState.STX_LOGGED);
            createNewTransactionFile();
            writeHeader(archiveFilePath, archiveFileName);
            setLogCountTo(0);
        }
    }

    private void writeHeader(String archiveFilePath, String archiveFileName) {
        writeDataToFile(
                "B000000000                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        1.0CRTRAN24PMAX  00000000", archiveFilePath, archiveFileName);
    }

    private void writeFooter(int logCount, String filePath, String fileName) {
        writeDataToFile("E999999999                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        1.0CRTRAN24PMAX  "
                + StringUtils.leftPad(Integer.toString(logCount), 8, '0'), filePath, fileName);
    }

    private void writeTransaction(String txnData, String filePath, String fileName) {
        txnData = "D" + StringUtils.substring(txnData, 69);
        writeDataToFile(txnData, filePath, fileName);
    }

    private String getSemaphoreFileName(String archiveFileName) {
        return archiveFileName + SemaphoreFileCreator.SEMAPHORE_FILE_EXTENSION;
    }

    private void uploadArchiveFileToSftp(String archiveFilePath, String archiveFileName) {
        try {
            String archiveFileAbsolutePath = Paths.get(archiveFilePath, archiveFileName).toAbsolutePath().toString();
            String semaphoreFileAbsolutePath = Paths.get(archiveFilePath, getSemaphoreFileName(archiveFileName))
                    .toAbsolutePath().toString();
            scpCommandExecutor.upload(archiveFileAbsolutePath);
            scpCommandExecutor.upload(semaphoreFileAbsolutePath);
        } catch (Exception ex) {
            log.error("Exception: " + ((displayExceptionLog == 1) ? ex : ex.getMessage()));
        }
    }

    private void writeDataToFile(String data, String filePath, String fileName) {
        Path path = Paths.get(filePath, fileName);
        writeDataInternal(path, data);
    }

    private void writeDataInternal(Path filePath, String data) {
        log.info("From the body of writeDataInternal(" + data + ")");
        try {
            if (data != null && !(data.isEmpty())) {
                data = data + System.getProperty("line.separator");
                Files.write(filePath, data.getBytes(StandardCharsets.UTF_8), StandardOpenOption.APPEND);
            } else {
                log.info("Message is empty" + data);
            }
        } catch (Exception ex) {
            log.error("Exception: " + ((displayExceptionLog == 1) ? ex : ex.getMessage()));
        }
    }

    private void createNewTransactionFile() {
        archiveFileName = semaphoreFileCreator.createSemaphoreFileAndTransactionFile();
    }

    private void changeLoggerStateTo(ArchiveLoggerState archiveLoggerState) {
        log.info("Current Logger State: " + archiveLoggerState.toString());
        this.archiveLoggerState = archiveLoggerState;
    }

    private void setLogCountTo(int logCount) {
        this.logCount = logCount;
    }
}
