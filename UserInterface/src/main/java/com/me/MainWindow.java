package com.me;

import javax.swing.*;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import java.awt.*;

public class MainWindow {

    public MainWindow()
    {
        textDebug.setText("");
    }

    public void printMessage(String s, boolean error)
    {
        StyledDocument doc = textDebug.getStyledDocument();
        SimpleAttributeSet keyWord = new SimpleAttributeSet();

        if(error) {
            StyleConstants.setForeground(keyWord, Color.RED);
            StyleConstants.setBold(keyWord, true);
        }
        //StyleConstants.setBackground(keyWord, Color.YELLOW);
        //StyleConstants.setBold(keyWord, true);
        try
        {
            doc.insertString(doc.getLength(), s, keyWord );

            //panel.revalidate();
            //JScrollBar vertical = debugScrollPane.getVerticalScrollBar();
            //vertical.setValue(vertical.getMaximum());
        }
        catch(Exception e) { System.out.println(e); }
    }

    public JPanel getPanel() {
        return panel;
    }

    public boolean isModePosition() {
        return tabbedPane.isEnabledAt(0);
    }

    public boolean isModeDirect() {
        return tabbedPane.isEnabledAt(1);
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

    public void setStickPosition(float x, float y) {
        labelStick.setText("Stick position (" + x + ", " + y + ")");
    }


    private JTextPane textDebug;
    private JTabbedPane tabbedPane;
    private JSlider sliderDirectRotation;
    private JSlider sliderDirectMainArm;
    private JSlider sliderDirectSmallArm;
    private JPanel panel;
    private JSlider sliderDirectGripper;
    private JScrollPane debugScrollPane;
    private JLabel labelStick;
}
