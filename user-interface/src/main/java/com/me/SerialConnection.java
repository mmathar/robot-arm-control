package com.me;

import com.fazecast.jSerialComm.SerialPort;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import static com.fazecast.jSerialComm.SerialPort.TIMEOUT_NONBLOCKING;

public class SerialConnection {

    // directly send the positions for all servos
    public class MessageDirect {
        public float base;
        public float mainArm;
        public float smallArm;
        public float gripper;
    };

    // send the position (in polar "robot space" coordinates) where the gripper should move
    public class MessagePosition  {
      public float rotation; // rotation [-45, 45]
      public float distance; // distance
      public float height; // height
      public float gripper; // gripper opening [0, 1]
    };

    public interface EventListener {
        void event(String msg);
    };

    public SerialConnection(String port) {
        listeners = new ArrayList<>();
        comPort = SerialPort.getCommPort(port);
        comPort.setComPortParameters(9600, 8, 1, 0);
        comPort.setComPortTimeouts(TIMEOUT_NONBLOCKING, 0, 0);

        if(!comPort.openPort()) {
            emitMessage("Couldn't open port.");
           opened = false;
        }
        emitMessage("Opened " + port + " connection.");
        sendMessage(";"); // terminate any garbage that came before
        opened = true;

        intermediateResult = new StringBuilder();
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

            /*InputStream inputStream = comPort.getInputStream();
            InputStreamReader streamReader = new InputStreamReader(inputStream);
            BufferedReader reader = new BufferedReader(streamReader);

            int c = -1;
            try {
                while ((c = reader.read()) != -1) {
                    intermediateResult.append((char)c);
                }

                int index = -1;
                String result = intermediateResult.toString();
                while((index = result.indexOf(';')) != -1) {
                    result = intermediateResult.toString();
                    String line = result.substring(0, index);
                    intermediateResult.delete(0, index);
                    processLine(line);
                }


            } catch (Exception e) {
                e.printStackTrace();
            }*/
        } catch (Exception e) { e.printStackTrace(); }
    }

    public void addListener(EventListener listener) {
        listeners.add(listener);
    }

    public void sendMessagePosition(MessagePosition msg) {
        sendMessage("SET-POSITION " + String.format("%5.3f", msg.rotation)
                + " " + String.format("%5.3f", msg.distance)
                + " " + String.format("%5.3f", msg.height)
                + " " + String.format("%5.3f", msg.gripper) + ";");
    }

    public void sendMessageDirect(MessageDirect msg) {
        sendMessage("SET-DIRECT " + String.format("%5.3f", msg.base)
                + " " + String.format("%5.3f", msg.mainArm)
                + " " + String.format("%5.3f", msg.smallArm)
                + " " + String.format("%5.3f", msg.gripper) + ";");
    }

    public void sendMessage(String msg) {
        OutputStream stream = comPort.getOutputStream();
        OutputStreamWriter writer = new OutputStreamWriter(stream);
        try {
            writer.write(msg);
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
            listener.event(msg);
        }
    }

    private StringBuilder intermediateResult;
    private SerialPort comPort;
    private boolean opened;
    private List<EventListener> listeners;
}
