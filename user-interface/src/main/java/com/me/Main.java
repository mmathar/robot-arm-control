package com.me;

import com.github.strikerx3.jxinput.XInputAxesDelta;
import com.github.strikerx3.jxinput.XInputButtonsDelta;
import com.github.strikerx3.jxinput.XInputComponentsDelta;
import com.github.strikerx3.jxinput.XInputDevice;
import com.github.strikerx3.jxinput.enums.XInputButton;

import javax.swing.*;
import java.awt.*;

public class Main {

    static MainWindow window;
    static JFrame frame;
    static SerialConnection connection;

    static void processLine(String s, boolean error) {
        window.printMessage(s, error);
    }

    static void initializeWindow() {
        window = new MainWindow();
        frame = new JFrame("EEZYbotarm MK2 control");
        frame.setContentPane(window.getPanel());
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dimension minSize = new Dimension();
        minSize.setSize(1200, 800);
        frame.setMinimumSize(minSize);
        frame.pack();
        frame.setVisible(true);
        frame.repaint();
    }

    public static void main(String[] args) {
        initializeWindow();

        try {
            XInputDevice[] devices = XInputDevice.getAllDevices();
            for(XInputDevice device : devices) {
                if(device.isConnected()) {
                    window.printMessage("Connected to controller: " + device.toString() + "\n", false);
                    controller = device;
                    break;
                }
            }
        } catch(Exception e) {
            e.printStackTrace();
        }

        connection = new SerialConnection();
        connection.addListener((str, error) -> window.printMessage(str, error));
        connection.initialize("COM4");
        while(frame.isVisible()) {
            connection.processStep();
            dirty = false;
            updateFromControls();
            //updateFromUI();
            if(dirty) {
                sendUpdate();
            }
            try {
                Thread.sleep(5);
            } catch (Exception e) {}
        }
        connection.close();
    }

    private static void updateFromControls() {
        if(controller == null)
            return;

        if (controller.poll()) {
            XInputComponentsDelta delta = controller.getDelta();

            XInputButtonsDelta buttons = delta.getButtons();
            XInputAxesDelta axes = delta.getAxes();

            if (buttons.isPressed(XInputButton.RIGHT_SHOULDER)) {
                    deltaGripper = -90.0f;
                    dirty = true;
            } else if (buttons.isReleased(XInputButton.RIGHT_SHOULDER)) {
                    deltaGripper = 90.0f;
                    dirty = true;
            } else if(Math.abs(deltaGripper) > 1e-2) {
                deltaGripper = 0.0f;
                dirty = true;
            }

            stickPositionX += axes.getLXDelta();
            stickPositionY += axes.getLYDelta();
            stickRPositionY += axes.getRYDelta();

            if(Math.abs(stickPositionX) > 1e-1) {
                deltaBase = -3.5f * stickPositionX;
                dirty = true;
            } else if(Math.abs(deltaBase) > 1e-2) {
                deltaBase = 0.0f;
                dirty = true;
            }

            if(Math.abs(stickPositionY) > 1e-1) {
                deltaDistance = -1.0f * stickPositionY;
                dirty = true;
            } else if(Math.abs(deltaDistance) > 1e-2) {
                deltaDistance  = 0.0f;
                dirty = true;
            }

            if(Math.abs(stickRPositionY) > 1e-1) {
                deltaHeight = 1.0f * stickRPositionY;
                dirty = true;
            } else if(Math.abs(deltaHeight) > 1e-2) {
                deltaHeight = 0.0f;
                dirty = true;
            }

            lastHeight = Math.max(0.0f, Math.min(25.0f, lastHeight));
            lastDistance = Math.max(-4.3f, Math.min(14.7f, lastDistance));

            window.setStickPosition(stickPositionX, stickRPositionY);

        } else {
            // Controller is not connected; display a message
        }
    }

    private static void updateFromUI() {
       /* final double eps = 1e-6;
        final double rotation = window.getDirectBaseRotation();
        final double smallArmRot = window.getDirectSmallArmRotation();
        final double mainArmRot = window.getDirectMainArmRotation();
        final double gripperRot = window.getDirectGripperRotation();
        if(Math.abs(rotation - lastBaseValue) > eps) {
            updateProperty("base", rotation);
            lastBaseValue = rotation;
        } else if(Math.abs(smallArmRot - lastSmallArmValue) > eps) {
            updateProperty("smallArm", smallArmRot);
            lastSmallArmValue = smallArmRot;
        } else if(Math.abs(mainArmRot - lastMainArmValue) > eps) {
            updateProperty("mainArm", mainArmRot);
            lastMainArmValue = mainArmRot;
        } else if(Math.abs(gripperRot - lastGripperValue) > eps) {
            updateProperty("gripper", gripperRot);
            lastGripperValue = gripperRot;
        }*/
    }

    private static void sendUpdate() {
        /*SerialConnection.MessageDirect msg = connection.new MessageDirect();
        msg.base = lastBaseValue;
        msg.mainArm = lastMainArmValue;
        msg.smallArm = lastSmallArmValue;
        msg.gripper = lastGripperValue;
        connection.sendMessageDirect(msg);*/

        SerialConnection.MessageUpdate msg = connection.new MessageUpdate();
        msg.deltaRotation = deltaBase;
        msg.deltaGripper = deltaGripper;
        msg.deltaDistance = deltaDistance;
        msg.deltaHeight = deltaHeight;
        connection.sendMessage((SerialConnection.Message)msg);
    }

    private static float deltaGripper = 0.0f;
    private static float deltaBase = 0.0f;
    private static float deltaDistance = 0.0f;
    private static float deltaHeight = 0.0f;

    private static float lastGripperValue = 0.0f;
    private static float lastSmallArmValue = 0.0f;
    private static float lastMainArmValue = 0.0f;
    private static float lastBaseValue = 0.0f;
    private static float lastDistance = 0.0f;
    private static float lastHeight = 0.0f;
    private static XInputDevice controller;
    private static float stickPositionX = 0.0f;
    private static float stickPositionY = 0.0f;
    private static float stickRPositionY = 0.0f;
    private static boolean dirty = false;
}
