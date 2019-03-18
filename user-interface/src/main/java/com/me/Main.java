package com.me;

import com.github.strikerx3.jxinput.XInputAxesDelta;
import com.github.strikerx3.jxinput.XInputButtonsDelta;
import com.github.strikerx3.jxinput.XInputComponentsDelta;
import com.github.strikerx3.jxinput.XInputDevice;
import com.github.strikerx3.jxinput.enums.XInputAxis;
import com.github.strikerx3.jxinput.enums.XInputButton;

import javax.swing.*;
import java.awt.*;

public class Main {

    static MainWindow window;
    static JFrame frame;
    static SerialConnection connection;

    static void processLine(String s) {
        window.printMessage(s);
        System.out.println(s);
    }

    static void initializeWindow() {
        window = new MainWindow();
        frame = new JFrame("EEZYbotarm MK2 control");
        frame.setContentPane(window.panel);
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
                    window.printMessage(device.toString() + "\n");
                    controller = device;
                    break;
                }
            }
        } catch(Exception e) {
            e.printStackTrace();
        }

        connection = new SerialConnection("COM4");
        connection.addListener(s -> window.printMessage(s));
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
                    lastGripperValue = 0.0f;
                    dirty = true;
            } else if (buttons.isReleased(XInputButton.RIGHT_SHOULDER)) {
                    lastGripperValue = 90.0f;
                    dirty = true;
            }

            stickPositionX += axes.getLXDelta();
            stickPositionY += axes.getLYDelta();
            stickRPositionY += axes.getRYDelta();

            if(Math.abs(stickPositionX) > 1e-1) {
                float newX = lastBaseValue + -3.5f * stickPositionX;
                newX = Math.max(-45.0f, Math.min(45.0f, newX));
                lastBaseValue = newX;
                dirty = true;
            }

            if(Math.abs(stickPositionY) > 1e-1) {
                float newY = lastDistance + -1.0f * stickPositionY;
                newY = Math.max(0.0f, Math.min(30f, newY));
                lastDistance = newY;
                dirty = true;
            }

            if(Math.abs(stickRPositionY) > 1e-1) {
                float newY = lastHeight + 1.0f * stickRPositionY;
                newY = Math.max(0.0f, Math.min(30f, newY));
                lastHeight = newY;
                dirty = true;
            }

            lastHeight = Math.max(0.0f, Math.min(25.0f, lastHeight));
            lastDistance = Math.max(-4.3f, Math.min(14.7f, lastDistance));

            window.labelStickX.setText("Stick X: " + stickPositionX);
            window.labelStickY.setText("Stick Y: " + stickPositionY);

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

        SerialConnection.MessagePosition msg = connection.new MessagePosition();
        msg.rotation = lastBaseValue;
        msg.gripper = lastGripperValue;
        msg.distance = lastDistance;
        msg.height = lastHeight;
        connection.sendMessagePosition(msg);
    }

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
