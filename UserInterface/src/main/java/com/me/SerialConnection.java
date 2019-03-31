package com.me;

import com.fazecast.jSerialComm.SerialPort;

import java.io.*;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import static com.fazecast.jSerialComm.SerialPort.TIMEOUT_NONBLOCKING;

public class SerialConnection {

    DecimalFormat getDecimalFormat() {
        DecimalFormatSymbols symbols = new DecimalFormatSymbols(Locale.getDefault());
        symbols.setDecimalSeparator('.');
        symbols.setGroupingSeparator(',');
        DecimalFormat decimal = new DecimalFormat("#0.000", symbols);
        return decimal;
    }

    public interface Message {
        String serialize();
    };

    // directly send the positions for all servos
    public class MessageDirect implements  Message {
        public float base;
        public float mainArm;
        public float smallArm;
        public float gripper;

        public String serialize() {
            DecimalFormat decimal = getDecimalFormat();
            StringBuilder builder = new StringBuilder();
            builder.append("SET-DIRECT")
                    .append(" ").append(decimal.format(base))
                    .append(" ").append(decimal.format(mainArm))
                    .append(" ").append(decimal.format(smallArm))
                    .append(" ").append(decimal.format(gripper))
                    .append(";");
            return builder.toString();
        }
    };

    // send the position (in polar "robot space" coordinates) where the gripper should move
    public class MessagePosition implements Message {
        public float rotation; // rotation [-45, 45]
        public float distance; // distance
        public float height; // height
        public float gripper; // gripper opening [0, 1]

        public String serialize() {
            DecimalFormat decimal = getDecimalFormat();
            StringBuilder builder = new StringBuilder();
            builder.append("SET-POSITION")
                .append(" ").append(decimal.format(rotation))
                .append(" ").append(decimal.format(distance))
                .append(" ").append(decimal.format(height))
                .append(" ").append(decimal.format(gripper))
                .append(";");
            return builder.toString();
        }
    };

    // update the current position by a delta
    public class MessageUpdate implements Message {
        public float deltaRotation;
        public float deltaDistance;
        public float deltaHeight;
        public float deltaGripper;

        public String serialize() {
            DecimalFormat decimal = getDecimalFormat();
            StringBuilder builder = new StringBuilder();
            builder.append("UPDATE-POSITION")
                    .append(" ").append(decimal.format(deltaRotation))
                    .append(" ").append(decimal.format(deltaDistance))
                    .append(" ").append(decimal.format(deltaHeight))
                    .append(" ").append(decimal.format(deltaGripper))
                    .append(";");
            return builder.toString();
        }
    };

    public interface EventListener {
        void event(String msg, boolean error);
    };

    public SerialConnection() {
        listeners = new ArrayList<>();
        intermediateResult = new StringBuilder();
        opened = false;
    }

    public void initialize(String port) {
        comPort = SerialPort.getCommPort(port);
        comPort.setComPortParameters(9600, 8, 1, 0);
        comPort.setComPortTimeouts(TIMEOUT_NONBLOCKING, 0, 0);

        if(!comPort.openPort()) {
            emitError("Couldn't open serial connection.");
           opened = false;
        } else {
            emitMessage("Opened " + port + " connection.");
            sendRawString(";"); // terminate any garbage that came before
            opened = true;
        }
    }

    public void finalize() {
        close();
    }

    public void close() {
        if(opened) {
            comPort.closePort();
            opened = false;
        }
    }

    public void processStep() {
        try {
            if (comPort.bytesAvailable() <= 0 || comPort.bytesAvailable() % 4 != 0)
                return;

            byte[] buffer = new byte[comPort.bytesAvailable()];
            int readBytes = comPort.readBytes(buffer, comPort.bytesAvailable());

            for(int i = 0; i < readBytes && i < buffer.length; i++) {
                intermediateResult.append((char)buffer[i]);
            }

            int index = -1;
            String result = intermediateResult.toString();
            while((index = result.indexOf(';')) != -1) {
                String line = result.substring(0, index);
                line.replace("\n", "");
                line.replace("\r", "");
                intermediateResult.delete(0, index + 1);
                result = intermediateResult.toString();
                processLine(line);
            }
        } catch (Exception e) { e.printStackTrace(); }
    }

    public void addListener(EventListener listener) {
        listeners.add(listener);
    }

    public void sendMessage(Message msg) {
        sendRawString(msg.serialize());
    }

    public void sendRawString(String s) {
        OutputStream stream = comPort.getOutputStream();
        OutputStreamWriter writer = new OutputStreamWriter(stream);
        try {
            writer.write(s);
            writer.flush();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    private void processLine(String s) {
        emitMessage(s);
    }

    private void emitMessage(String msg) {
        for(EventListener listener : listeners) {
            listener.event(msg, false);
        }
    }

    private void emitError(String msg) {
        for(EventListener listener : listeners) {
            listener.event(msg, true);
        }
    }


    private StringBuilder intermediateResult;
    private SerialPort comPort;
    private boolean opened;
    private List<EventListener> listeners;
}
