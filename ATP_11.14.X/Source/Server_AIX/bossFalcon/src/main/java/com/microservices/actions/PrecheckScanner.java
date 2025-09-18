package com.microservices.actions;

import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.PropertySource;
import org.springframework.stereotype.Service;

import com.google.common.base.Charsets;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.List;
import java.util.stream.Collectors;

@Slf4j
@Service
@PropertySource("file:${ASCENDENTROOT}/config/faisnd_app_properties.properties")
public class PrecheckScanner {
    private final ScpUploadCommandExecutor scpUploadCommandExecutor;
    private final String archiveFilePath;
    private int displayExceptionLog = 0;

    public PrecheckScanner(ScpUploadCommandExecutor scpUploadCommandExecutor,
                           @Value("${archive.file.path}") String archiveFilePath,
                           @Value("${display.exception.log}") int displayExceptionLog) {
        this.scpUploadCommandExecutor = scpUploadCommandExecutor;
        this.archiveFilePath = archiveFilePath;
        this.displayExceptionLog = displayExceptionLog;
    }

    public void doPrecheck() {
        log.debug("Initiating precheck...");

        File directory = new File(archiveFilePath);
        File[] files = directory.listFiles((dir, name) -> (name.endsWith(".txt")));
        log.debug(String.valueOf(files != null ? files.length : 0));
        if (files != null) {
            for (File file : files) {
                try {
                    List<String> lines = Files.lines(Paths.get(file.getAbsolutePath())).collect(Collectors.toList());
                    if (lines.size() == 1) {
                        Files.delete(Paths.get(file.getAbsolutePath()));
                        Files.delete(Paths.get(file.getAbsolutePath() + SemaphoreFileCreator.SEMAPHORE_FILE_EXTENSION));
                        continue;
                    }
                    if (lines.size() > 1) {
                        if (lines.stream().anyMatch(log -> log.contains("E999999999"))) {
                            scpUploadCommandExecutor.upload(file.getAbsolutePath());
                            scpUploadCommandExecutor.upload(file.getAbsolutePath() + SemaphoreFileCreator.SEMAPHORE_FILE_EXTENSION);
                        } else {
                            String footer = "E999999999                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        1.0CRTRAN24PMAX  "
                                    + StringUtils.leftPad(Long.toString(lines.size() - 1), 8, '0');
                            Files.write(Paths.get(file.getAbsolutePath()), footer.getBytes(Charsets.UTF_8), StandardOpenOption.APPEND);
                            scpUploadCommandExecutor.upload(file.getAbsolutePath());
                            scpUploadCommandExecutor.upload(file.getAbsolutePath() + SemaphoreFileCreator.SEMAPHORE_FILE_EXTENSION);  
                        }
                    }
                } catch (Exception ex) {
                    log.warn("Exception: " + ((displayExceptionLog == 1) ? ex : ex.getMessage()));
                    log.debug("Ignoring exception above..");
                }
            }
        }
    }
}
