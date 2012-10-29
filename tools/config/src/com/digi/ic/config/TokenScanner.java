package com.digi.ic.config;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Scanner;

public class TokenScanner {

    private int lineNumber;
    private Scanner lineScanner;
    private Scanner tokenScanner;

    public TokenScanner(String fileName) throws Exception {
        try {

            File configFile = new File(fileName);

            if (!configFile.exists()) {
                throw new NullPointerException(fileName + " file not found.");
            }

            lineScanner = new Scanner(new FileReader(configFile));

            tokenScanner = null;

        } catch (NullPointerException e) {
            ConfigGenerator.log("Unable to open " + fileName);
            throw new Exception("Error in opening " + fileName);
        }
    }

    public String getToken() {
        String aWord = null;

        do {
            if ((tokenScanner != null) && (!tokenScanner.hasNext())) {
                tokenScanner.close();
                tokenScanner = null;
            }
            if (tokenScanner == null) {
                while (lineScanner.hasNextLine()) {
                    String line = lineScanner.nextLine();
                    lineNumber++;
                    // ConfigGenerator.log("line " + lineNumber + ": " + line);
                    if ((line.length() > 0) && (line.split(" ").length > 0)) {
                        tokenScanner = new Scanner(line);
                        break;
                    }
                }
            }

            if ((tokenScanner != null) && (tokenScanner.hasNext())) {
                aWord = tokenScanner.next();
            }
            
            if (!lineScanner.hasNextLine()) {
                break;
            }
        } while (aWord == null);

        return aWord;
    }

    public int getTokenInt() throws IOException {
        String str = getToken();

        try {
            int anInt = Integer.parseInt(str);
            return anInt;

        } catch (NumberFormatException e) {
            throw new IOException("Not an integer (expect an integer value)");
        }
    }

    public String getTokenInLine(String pattern) {
        String aLine = null;

        if ((tokenScanner != null) && (!tokenScanner.hasNext())) {
            tokenScanner.close();
            tokenScanner = null;
        }
        if (tokenScanner == null) {
            while (lineScanner.hasNextLine()) {
                String line = lineScanner.nextLine();
                lineNumber++;
                // ConfigGenerator.log("line " + lineNumber + ": " + line);
                if ((line.length() > 0) && (line.split(" ").length > 0)) {
                    tokenScanner = new Scanner(line);
                    break;
                }
            }
        }

        if (tokenScanner.hasNext())
            aLine = tokenScanner.findInLine(pattern);

        return aLine;
    }

    public boolean hasToken() {
        boolean token_avail = false;

        if (tokenScanner != null) {
            token_avail = tokenScanner.hasNext();
        }
        if (!token_avail) {
            token_avail = lineScanner.hasNext();
        }

        return token_avail;
    }

    public boolean hasToken(String pattern) {
        boolean token_avail = false;

        if (tokenScanner != null) {
            token_avail = tokenScanner.hasNext(pattern);
        }

        if (!token_avail) {
            token_avail = lineScanner.hasNext(pattern);
        }

        return token_avail;
    }

    public boolean hasTokenInt() {
        boolean token_avail = false;

        if (tokenScanner != null) {
            token_avail = tokenScanner.hasNextInt();
        }

        if (!token_avail) {
            token_avail = lineScanner.hasNextInt();
        }

        return token_avail;
    }

    public int getLineNumber() {
        return lineNumber;
    }

    public void skipCommentLine() {
        tokenScanner.close();
        tokenScanner = null;
    }
    
    public void close() {
        // ensure the underlying stream is always closed
        // this only has any effect if the item passed to the Scanner
        // constructor implements Closeable (which it does in this case).
        if (tokenScanner != null) {
            tokenScanner.close();
        }
        lineScanner.close();

    }

}
