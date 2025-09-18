package com.microservices.helpers;

public class StringUtilities {
    public static boolean isNotNullOrEmpty(String str) {
        return (str != null && !str.trim().isEmpty());
    }
}
