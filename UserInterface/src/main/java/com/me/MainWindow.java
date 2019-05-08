package com.me;

import javax.swing.*;
import javax.swing.event.ChangeListener;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import java.awt.*;
import java.awt.event.ActionListener;
import java.util.List;

public class MainWindow {

    public MainWindow() {
        textDebug.setText("");
    }

    public void addCommPortRefreshListener(ActionListener listener) {
        btRefreshArduino.addActionListener(listener);
    }

    public void addCommPortConnectListener(ActionListener listener) {
        btConnectArduino.addActionListener(listener);
    }

    public void addAngleSliderListener(ChangeListener listener) {
        // If the sliders for the angles are moved we need to know
        // (because this should override any controller input)
        sliderDirectRotation.addChangeListener(listener);
        sliderDirectGripper.addChangeListener(listener);
        sliderDirectMainArm.addChangeListener(listener);
        sliderDirectSmallArm.addChangeListener(listener);
    }

    public void setCommPortConnected(boolean connected) {
        // is an arduino connected?
        // without an arduino we can't really do anything useful
        // so we need to disable stuff
        if(connected) {
            btConnectArduino.setText("Disconnect");
            btRefreshArduino.setEnabled(false);
            cbCommPorts.setEnabled(false);
            sliderDirectSmallArm.setEnabled(true);
            sliderDirectMainArm.setEnabled(true);
            sliderDirectGripper.setEnabled(true);
            sliderDirectRotation.setEnabled(true);
        } else {
            btConnectArduino.setText("Connect");
            btRefreshArduino.setEnabled(true);
            cbCommPorts.setEnabled(true);
            sliderDirectSmallArm.setEnabled(false);
            sliderDirectMainArm.setEnabled(false);
            sliderDirectGripper.setEnabled(false);
            sliderDirectRotation.setEnabled(false);
        }
    }

    public void setControllerConnected(boolean connected) {
        if(connected) {
            controllerState.setText("connected.");
        } else {
            controllerState.setText("not connected.");
        }
    }

    public void printMessage(String s, boolean error) {
        StyledDocument doc = textDebug.getStyledDocument();
        SimpleAttributeSet keyWord = new SimpleAttributeSet();

        if (error) {
            StyleConstants.setForeground(keyWord, Color.RED);
            StyleConstants.setBold(keyWord, true);
        }
        try {
            doc.insertString(doc.getLength(), s + '\n', keyWord);

            //panel.revalidate();
            //JScrollBar vertical = debugScrollPane.getVerticalScrollBar();
            //vertical.setValue(vertical.getMaximum());
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public void setComPortList(List<String> commPorts) {
        cbCommPorts.removeAllItems();
        for(String s : commPorts) {
            cbCommPorts.addItem(s);
        }
    }

    public String getSelectedCOMPort() {
        if(cbCommPorts.getSelectedIndex() != -1)
            return cbCommPorts.getSelectedItem().toString();
        return "";
    }

    public float getDirectBaseRotation() {
        return sliderDirectRotation.getValue();
    }

    public float getDirectSmallArmRotation() {
        return sliderDirectSmallArm.getValue();
    }

    public float getDirectMainArmRotation() {
        return sliderDirectMainArm.getValue();
    }

    public float getDirectGripperRotation() {
        return sliderDirectGripper.getValue();
    }

    public JPanel getPanel() {
        return panel;
    }

    // variables created by the form designer
    private JPanel panel;
    private JTextPane textDebug;
    private JTabbedPane tabbedPane;
    private JSlider sliderDirectRotation;
    private JSlider sliderDirectMainArm;
    private JSlider sliderDirectSmallArm;
    private JSlider sliderDirectGripper;
    private JScrollPane debugScrollPane;
    private JComboBox cbCommPorts;
    private JButton btRefreshArduino;
    private JButton btConnectArduino;
    private JLabel controllerState;
    private JPanel directControlTab;
}
