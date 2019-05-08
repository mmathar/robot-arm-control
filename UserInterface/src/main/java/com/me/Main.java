package com.me;

import com.github.strikerx3.jxinput.*;
import com.github.strikerx3.jxinput.enums.XInputButton;
import com.github.strikerx3.jxinput.listener.XInputDeviceListener;

import javax.swing.*;
import java.awt.*;

public class Main {

    public static void main(String[] args) {
        Main app = new Main();
        app.run();
    }

    void run() {
        initializeWindow();
        setupUIListeners();
        createConnection();
        // show initially available COM ports
        onWantToRefreshCOMListing();

        while (frame.isVisible()) {
            readControllerInput();
            if (connection.isConnected()) {
                connection.readMessages();
                sendUpdatedPosition();
            }
            waitABit();
        }

        close();
    }

    void initializeWindow() {
        window = new MainWindow();
        frame = new JFrame("EEZYbotarm MK2 control");
        frame.setContentPane(window.getPanel());
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dimension minSize = new Dimension();
        minSize.setSize(600, 600);
        frame.setMinimumSize(minSize);
        frame.pack();
        frame.setVisible(true);
        frame.repaint();

        window.setControllerConnected(false);
        window.setCommPortConnected(false);
    }

    void setupUIListeners() {
        // if any angle value (sliders) changes mark all as dirty
        window.addAngleSliderListener(x -> dirtyDirectInput = true);
        // dual function too. see above
        window.addCommPortConnectListener(x -> {
            if (connectedCOM)
                onWantToDisconnectCOM();
            else
                onWantToConnectCOM();
        });
        window.addCommPortRefreshListener(x -> onWantToRefreshCOMListing());
    }

    void onWantToRefreshCOMListing() {
        window.setComPortList(connection.getCommPorts());
    }

    void onWantToConnectCOM() {
        if (window.getSelectedCOMPort().isEmpty()) {
            window.printMessage("No COM port selected.", true);
            return;
        }
        connection.addListener((str, error) -> window.printMessage(str, error));
        connection.initialize(window.getSelectedCOMPort());
        window.printMessage("COM connected.", false);
        window.setCommPortConnected(true);
        connectedCOM = true;
    }

    void onWantToDisconnectCOM() {
        window.printMessage("COM disconnected.", false);
        window.setCommPortConnected(false);
        connectedCOM = false;
    }

    void createConnection() {
        connection = new SerialConnection();
    }

    void tryFindGamepad() {
        try {
            controller = XInputDevice14.getDeviceFor(0);
            window.setControllerConnected(controller.isConnected());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // calculate the necessary movement from the controller stick
    // position changes. Only works if a controller is connected.
    private void readControllerInput() {
        // get the best connected gamepad (hot-plug support)
        // Is it inefficient to do this every frame? Possibly.
        // Does it work any other way? Not for me.
        // I tried XInputDeviceListener but the events connected / disconnected just never seem to fire...
        // So here we are.
        tryFindGamepad();

        if(!controller.poll())
            return;

        deltaGripper = 0.0f;
        deltaBase = 0.0f;
        deltaHeight = 0.0f;
        deltaDistance = 0.0f;

        XInputComponentsDelta delta = controller.getDelta();
        XInputButtonsDelta buttons = delta.getButtons();
        if (buttons.isPressed(XInputButton.RIGHT_SHOULDER)) {
            deltaGripper = -90.0f; // close the gripper
            dirtyControllerInput = true;
        } else if (buttons.isReleased(XInputButton.RIGHT_SHOULDER)) {
            deltaGripper = 90.0f; // open the gripper
            dirtyControllerInput = true;
        }

        XInputAxesDelta axes = delta.getAxes();
        stickPosLX += axes.getLXDelta();
        stickPosLY += axes.getLYDelta();
        stickPosRY += axes.getRYDelta();

        // left stick right -> gripper right
        if (Math.abs(stickPosLX) > 1e-1) {
            deltaBase = -3.5f * stickPosLX;
            dirtyControllerInput = true;
        }

        // left stick up -> gripper forward
        if (Math.abs(stickPosLY) > 1e-1) {
            deltaDistance = -1.0f * stickPosLY;
            dirtyControllerInput = true;
        }

        // right stick up = gripper up
        if (Math.abs(stickPosRY) > 1e-1) {
            deltaHeight = 1.0f * stickPosRY;
            dirtyControllerInput = true;
        }
    }

    private void sendUpdatedPosition() {
        if (dirtyDirectInput) {
            SerialConnection.MessageDirect msg = connection.new MessageDirect();
            msg.base = window.getDirectBaseRotation();
            msg.mainArm = window.getDirectMainArmRotation();
            msg.smallArm = window.getDirectSmallArmRotation();
            msg.gripper = window.getDirectGripperRotation();
            connection.sendMessage((SerialConnection.Message) msg);
        } else if (dirtyControllerInput) {
            SerialConnection.MessageUpdate msg = connection.new MessageUpdate();
            msg.deltaRotation = deltaBase;
            msg.deltaGripper = deltaGripper;
            msg.deltaDistance = deltaDistance;
            msg.deltaHeight = deltaHeight;
            connection.sendMessage((SerialConnection.Message) msg);
        }

        dirtyDirectInput = false;
        dirtyControllerInput = false;
    }

    private void waitABit() {
        // Don't overwhelm the poor microcontroller
        // ...sleep for a bit
        try {
            Thread.sleep(5);
        } catch (Exception e) {
        }
    }

    private void close() {
        connection.close();
    }

    MainWindow window;
    JFrame frame;
    SerialConnection connection;

    private boolean connectedCOM = false;

    private float deltaGripper = 0.0f;
    private float deltaBase = 0.0f;
    private float deltaDistance = 0.0f;
    private float deltaHeight = 0.0f;

    private XInputDevice14 controller;
    private float stickPosLX = 0.0f;
    private float stickPosLY = 0.0f;
    private float stickPosRY = 0.0f;

    private boolean dirtyDirectInput = false;
    private boolean dirtyControllerInput = false;
}
